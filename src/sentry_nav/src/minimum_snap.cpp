#include "sentry_nav/minimum_snap.hpp"
#include "minimumSnap.hpp"
#include <chrono>
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace sentry_nav {
namespace {
using Clock = std::chrono::steady_clock;
using Bezier = std::vector<Eigen::Vector2d>;
double choose(int n, int k) {
  double v=1; for(int j=1;j<=k;++j) v=v*(n-j+1)/j; return v;
}
Bezier bezier(const QuinticSegment &s) {
  Bezier b(6, Eigen::Vector2d::Zero());
  for(int i=0;i<6;++i) for(int k=0;k<=i;++k)
    b[i] += choose(i,k)/choose(5,k)*std::pow(s.duration,k)*s.coefficients.row(k).transpose();
  return b;
}
std::pair<Bezier,Bezier> split(Bezier b) {
  Bezier l(b.size()),r(b.size());
  for(size_t k=0;k<b.size();++k) {
    l[k]=b[0]; r[b.size()-1-k]=b[b.size()-1-k];
    for(size_t i=0;i+1<b.size()-k;++i) b[i]=(b[i]+b[i+1])*.5;
  }
  return {l,r};
}
// Derivative Bernstein convex hulls bound the complete polynomial, not samples.
double peak(const Bezier &b, int depth=3) {
  if(depth) {auto [l,r]=split(b);return std::max(peak(l,depth-1),peak(r,depth-1));}
  double p=0; for(const auto &v:b) p=std::max(p,v.norm()); return p;
}
Bezier derivative(const Bezier &b,double duration) {
  Bezier d; for(size_t i=1;i<b.size();++i) d.push_back((b[i]-b[i-1])*(b.size()-1)/duration); return d;
}
// If a curve's entire convex hull bounding box is free, every point is free.
// Otherwise subdivide exactly. Unresolved contact is rejected conservatively.
bool collision_free(const Bezier &b,const core::PreparedMap &m,
  const std::function<bool()> &stop,int depth=0) {
  if(stop()) return false;
  auto lo=b.front(),hi=lo;
  for(const auto &p:b) {lo=lo.cwiseMin(p);hi=hi.cwiseMax(p);}
  const auto a=m.source.cell(lo),z=m.source.cell(hi);
  bool free=a && z;
  if(free) for(int y=a->y();y<=z->y() && free;++y) for(int x=a->x();x<=z->x();++x)
    if(m.blocked[m.source.index(x,y)]) {free=false;break;}
  if(free) return true;
  if(depth>=18 || m.is_blocked(b.front()) || m.is_blocked(b.back())) return false;
  auto [l,r]=split(b); return collision_free(l,m,stop,depth+1) && collision_free(r,m,stop,depth+1);
}
}
void SnapConfig::validate() const {
  if(!std::isfinite(timeout_seconds)||timeout_seconds<=0||!std::isfinite(corridor_range)||corridor_range<=0||
    collision_iterations<1||collision_iterations>10||max_segments<1||max_segments>80)
    throw std::invalid_argument("invalid TDT MinimumSnap configuration");
}
SnapResult optimize_snap(const core::PreparedMap &map,const core::PlanRequest &request,
  const core::PlannerConfig &limits,const core::PlanResult &seed,const SnapConfig &config,
  const std::function<bool()> &canceled) {
  config.validate(); limits.validate();
  const auto begin=Clock::now();
  auto elapsed=[&]{return std::chrono::duration<double>(Clock::now()-begin).count();};
  auto stop=[&]{return (canceled && canceled()) || elapsed()>=config.timeout_seconds;};
  SnapResult result;
  auto finish=[&](const std::string &reason){result.reason=reason;result.elapsed_ms=elapsed()*1000;return result;};
  if(stop()) return finish("canceled_or_timeout");
  if(!seed.success||seed.points.size()<2||seed.points.size()>size_t(config.max_segments+1)) return finish("invalid_seed");
  std::vector<MinimumSnap::SolveInput::Item> items;
  for(size_t i=0;i<seed.points.size();++i) {
    const auto &p=seed.points[i].position;
    if(map.is_blocked(p)) return finish("seed_in_obstacle");
    if(i && !core::line_free(map,seed.points[i-1].position,p)) return finish("seed_in_obstacle");
    MinimumSnap::SolveInput::Item item;item.xy=p.cast<float>();
    if(std::find(seed.mandatory_indices.begin(),seed.mandatory_indices.end(),i)!=seed.mandatory_indices.end()) {
      item.autoCorridor=false;item.corridor={item.xy.x(),item.xy.y(),item.xy.x(),item.xy.y()};
    }
    items.push_back(item);
  }
  items.front().useVxvy=true;
  items.back().useVxvy=config.constrain_goal_velocity;
  items.front().vxvy=request.velocity.cast<float>();items.back().vxvy=request.goal_velocity.cast<float>();
  if(!request.velocity.allFinite()||!request.goal_velocity.allFinite()||
    request.velocity.norm()>limits.max_speed+1e-6||
    (config.constrain_goal_velocity && request.goal_velocity.norm()>limits.max_speed+1e-6)) return finish("boundary_velocity_limit");
  MinimumSnap solver;solver.setOrder(6);solver.setMaxDx(3);solver.setStrictCollision(true);solver.canceled=stop;
  MinimumSnap::Map grid(map.source.height,map.source.width);
  for(int y=0;y<map.source.height;++y) for(int x=0;x<map.source.width;++x)
    grid(y,x)=map.blocked[map.source.index(x,y)] ? 0 : 255;
  solver.setMap(grid,map.source.resolution,map.source.origin.x(),map.source.origin.y());
  MinimumSnap::SolveInput input; input.setItems(items);input.setInitVel(request.velocity.cast<float>());
  input.setMaxSpeed(limits.max_speed);input.setMaxAcc(limits.max_acceleration);
  input.setNormTime(true);input.setBackend(MinimumSnap::Backend::OSQPCorridor);
  input.setCollisionCheckIter(config.collision_iterations);input.setMaxCorridorRange(config.corridor_range);
  auto times=MinimumSnap::trapezoidalTimeAllocation(input.getPath(),limits.max_speed,limits.max_acceleration);
  for(int attempt=0;attempt<6 && !stop();++attempt) {
    input.setTimeAllocated(times);solver.setTL(config.timeout_seconds-elapsed());
    const auto solved=solver.solve(input);
    result.corridors = solved.corridor;
    if(stop()) return finish("canceled_or_timeout");
    if(!solved.success||solved.durations.empty()) return finish("tdt_solve_failed");
    if(solved.durations.size()>size_t(config.max_segments)) return finish("segment_limit");
    std::vector<QuinticSegment> segments;
    double scale=1;
    for(size_t i=0;i<solved.durations.size();++i) {
      QuinticSegment s;s.duration=solved.durations[i];
      for(int k=0;k<6;++k) s.coefficients.row(k)<<solved.coefficients_x(i,5-k)/std::pow(s.duration,k),solved.coefficients_y(i,5-k)/std::pow(s.duration,k);
      if(!s.coefficients.allFinite() || !std::isfinite(s.duration) || s.duration<=0) return finish("invalid_coefficients");
      const auto b=bezier(s),v=derivative(b,s.duration),a=derivative(v,s.duration);
      if(!collision_free(b,map,stop)) return finish(stop()?"canceled_or_timeout":"curve_collision");
      scale=std::max({scale,peak(v)/limits.max_speed,std::sqrt(peak(a)/limits.max_acceleration)});
      segments.push_back(s);
    }
    if(scale<=1.00001) {
      result.segments=std::move(segments);result.success=true;return finish("success");
    }
    // Re-solve so physical boundary velocities stay fixed when duration changes.
    for(auto &t:times) t*=std::max(1.1,scale*1.05);
  }
  return finish(stop()?"canceled_or_timeout":"dynamics_limit");
}
}
