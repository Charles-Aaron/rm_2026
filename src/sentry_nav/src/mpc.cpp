#include "sentry_nav/mpc.hpp"
#include "tinympc/tiny_api.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <stdexcept>
#include <sstream>
#include <array>
#include <Eigen/Geometry>

namespace sentry_nav {
namespace {
using Clock=std::chrono::steady_clock;
// Scale command state to balance its consensus penalty against acceleration.
constexpr double command_scale = 10.0;
using Polygon=std::vector<Eigen::Vector2d>;
const std::array<Eigen::Vector2d,8> normals=[] {
  std::array<Eigen::Vector2d,8> n;
  for(int i=0;i<8;++i) n[i]={std::cos(i*M_PI/4),std::sin(i*M_PI/4)};
  return n;
}();
Polygon clip(const Polygon &p,const Eigen::Vector2d &n,double bound) {
  Polygon out;if(p.empty()) return out;
  Eigen::Vector2d previous=p.back();double pd=n.dot(previous)-bound;
  for(const auto &v:p) {
    const double d=n.dot(v)-bound;
    if((d<=0)!=(pd<=0)) out.push_back(previous+(v-previous)*(pd/(pd-d)));
    if(d<=0) out.push_back(v);
    previous=v;pd=d;
  }
  return out;
}
Polygon octagon(double radius) {
  Polygon p;for(int i=0;i<8;++i) p.emplace_back(radius*std::cos((i+.5)*M_PI/4),radius*std::sin((i+.5)*M_PI/4));return p;
}
Eigen::Vector2d project(const Eigen::Vector2d &v,const Polygon &p) {
  if(p.empty()) throw std::runtime_error("empty feasible polygon");
  bool inside=true;double best=std::numeric_limits<double>::infinity();Eigen::Vector2d closest=p.front();
  for(size_t i=0;i<p.size();++i) {
    const Eigen::Vector2d edge=p[(i+1)%p.size()]-p[i],offset=v-p[i];
    if(edge.x()*offset.y()-edge.y()*offset.x() < -1e-12) inside=false;
    const Eigen::Vector2d q=p[i]+edge*std::clamp(offset.dot(edge)/std::max(edge.squaredNorm(),1e-24),0.,1.);
    if((q-v).squaredNorm()<best) {best=(q-v).squaredNorm();closest=q;}
  }
  return inside && p.size()>=3 ? v : closest;
}
Polygon physical(double vmax,double amax,const Eigen::Matrix2d &e,const Eigen::Matrix2d &f,
  double dt,const Eigen::Vector2d &v) {
  auto p=octagon(vmax);
  for(const auto &n:normals) {
    const Eigen::Vector2d m=(f.transpose()*n)/dt;
    const double bound=amax*std::cos(M_PI/8)+n.dot((Eigen::Matrix2d::Identity()-e)*v)/dt;
    p=clip(p,m,bound);
  }
  return p;
}
}
void MpcConfig::validate() const {
  if(horizon<5||horizon>60) throw std::invalid_argument("MPC horizon out of range");
  for(double v:{dt,control_dt,response_x,response_y,max_speed,max_acceleration,timeout,position_weight,velocity_weight,delta_weight})
    if(!std::isfinite(v)||v<=0) throw std::invalid_argument("MPC parameters must be finite and positive");
  if(!std::isfinite(spin_model_gain)||spin_model_gain<0||spin_model_gain>2)
    throw std::invalid_argument("spin_model_gain must be between 0 and 2");
  if(spin_model_gain>0 && std::abs(response_x-response_y)>1e-9)
    throw std::invalid_argument("spin model requires equal x/y response times");
  // TinyMPC caches a stationary Riccati recursion: every stage uses one dt.
  if(dt>.2||std::abs(dt-control_dt)>1e-9||timeout>=control_dt) throw std::invalid_argument("TinyMPC model_dt must equal controller period; timeout must be shorter");
}
struct Mpc::Impl {
  MpcConfig c;TinySolver *solver=nullptr;
  Polygon speed,acceleration,slew,first;
  Eigen::Vector2d response;
  Eigen::Matrix2d e = Eigen::Matrix2d::Identity(), f = Eigen::Matrix2d::Zero();
  double cached_omega = 0;
  Clock::time_point deadline;
  explicit Impl(MpcConfig config):c(config) {
    c.validate();
    // x=[relative position, planning-frame velocity, previous command, acceleration],
    // u=command rate. Expressing rate constraints in m/s^2 avoids tiny dt-scaled bounds.
    tinyMatrix a=tinyMatrix::Zero(8,8),b=tinyMatrix::Zero(8,2);
    for(int axis=0;axis<2;++axis) {
      const double decay=std::exp(-c.dt/(axis==0?c.response_x:c.response_y));
      response[axis]=(1-decay)/c.dt;
      a(axis,axis)=1;a(axis,axis+2)=c.dt*decay;a(axis,axis+4)=c.dt*(1-decay)/command_scale;
      a(axis+2,axis+2)=decay;a(axis+2,axis+4)=(1-decay)/command_scale;
      a(axis+4,axis+4)=1;a(axis+6,axis+2)=-response[axis];a(axis+6,axis+4)=response[axis]/command_scale;
      b(axis,axis)=c.dt*c.dt*(1-decay);b(axis+2,axis)=c.dt*(1-decay);b(axis+4,axis)=command_scale*c.dt; b(axis+6,axis)=c.dt*response[axis];
    }
    for(int axis=0;axis<2;++axis) {
      e(axis,axis)=std::exp(-c.dt/(axis==0?c.response_x:c.response_y));
      f(axis,axis)=1-e(axis,axis);
    }
    tinyMatrix q=tinyMatrix::Zero(8,8),r=tinyMatrix::Identity(2,2)*c.delta_weight*c.dt*c.dt;
    q.diagonal()<<c.position_weight,c.position_weight,c.velocity_weight,c.velocity_weight,.01/(command_scale*command_scale),.01/(command_scale*command_scale),0,0;
    const int status=tiny_setup(&solver,a,b,tinyVector::Zero(8),q,r,.2,8,2,c.horizon+1,0);
    if(status) {release();throw std::runtime_error("TinyMPC setup failed");}
    auto &s=*solver->settings;
    s.en_state_bound=s.en_input_bound=0;s.adaptive_rho=0;s.relaxation=1.6;
    // ADMM tolerances are numerical; the final rollout reprojects to the exact
    // physical sets and rejects any command correction larger than 2 mm/s.
    s.abs_pri_tol=5e-3;s.abs_dua_tol=2e-3;s.max_iter=1500;s.check_termination=1;
    solver->context=this;
    solver->stop=[](void *ctx){return Clock::now()>=static_cast<Impl*>(ctx)->deadline;};
    solver->project=[](tinyMatrix &x,tinyMatrix &u,void *ctx) {
      auto &d=*static_cast<Impl*>(ctx);
      // First-step physical bounds are already in the exact input polygon.
      for(int k=2;k<x.cols();++k) {
        x.block<2,1>(4,k)=project(x.block<2,1>(4,k),d.speed);
        x.block<2,1>(6,k)=project(x.block<2,1>(6,k),d.acceleration);
      }
      for(int k=0;k<u.cols();++k) u.col(k)=project(u.col(k),k==0?d.first:d.slew);
    };
  }
  bool update_spin_model(double omega) {
    // Quantization limits cache recomputation from sensor noise; both the
    // optimizer and physical rollout use the same quantized model.
    const double requested = std::clamp(omega*c.spin_model_gain,-8.0,8.0);
    // Hysteresis avoids rebuilding the Riccati cache on every noisy sample.
    if (std::abs(requested-cached_omega) <= 0.075) {return true;}
    const double modeled = std::round(requested*10.0)/10.0;
    if (modeled == cached_omega) {return true;}
    const Eigen::Matrix2d rotation = Eigen::Rotation2Dd(modeled*c.dt).toRotationMatrix();
    Eigen::Matrix2d decay = Eigen::Matrix2d::Zero();
    decay(0,0)=std::exp(-c.dt/c.response_x); decay(1,1)=std::exp(-c.dt/c.response_y);
    const Eigen::Matrix2d next_e = rotation*decay;
    const Eigen::Matrix2d next_f = rotation*(Eigen::Matrix2d::Identity()-decay);
    tinyMatrix a=tinyMatrix::Zero(8,8),b=tinyMatrix::Zero(8,2);
    a.block<2,2>(0,0).setIdentity();
    a.block<2,2>(0,2)=c.dt*next_e; a.block<2,2>(0,4)=c.dt*next_f/command_scale;
    a.block<2,2>(2,2)=next_e; a.block<2,2>(2,4)=next_f/command_scale;
    a.block<2,2>(4,4).setIdentity();
    a.block<2,2>(6,2)=(next_e-Eigen::Matrix2d::Identity())/c.dt;
    a.block<2,2>(6,4)=next_f/(c.dt*command_scale);
    b.block<2,2>(0,0)=c.dt*c.dt*next_f; b.block<2,2>(2,0)=c.dt*next_f;
    b.block<2,2>(4,0)=command_scale*c.dt*Eigen::Matrix2d::Identity();
    b.block<2,2>(6,0)=next_f;
    auto &w=*solver->work;
    TinyCache updated=*solver->cache;
    if(tiny_precompute_and_set_cache(&updated,a,b,w.fdyn,w.Q.asDiagonal(),w.R.asDiagonal(),8,2,
      solver->cache->rho,0)!=0 || !updated.Kinf.allFinite() || !updated.Pinf.allFinite()) {return false;}
    *solver->cache=std::move(updated); w.Adyn=a; w.Bdyn=b;
    e=next_e; f=next_f; cached_omega=modeled;
    return true;
  }
  void release() {if(solver) {delete solver->work;delete solver->settings;delete solver->cache;delete solver->solution;delete solver;solver=nullptr;}}
  ~Impl(){release();}
};
Mpc::Mpc(MpcConfig c):impl_(std::make_unique<Impl>(c)){}
Mpc::~Mpc()=default;
void Mpc::reset() {
  auto &w=*impl_->solver->work;w.x.setZero();w.u.setZero();w.v.setZero();w.vnew.setZero();w.z.setZero();w.znew.setZero();w.g.setZero();w.y.setZero();
}
MpcResult Mpc::solve(const Eigen::Vector2d &position,const Eigen::Vector2d &measured,
  const Eigen::Vector2d &previous,const TimedTrajectory &trajectory,double time,double speed_limit,double angular_velocity) {
  auto &d=*impl_;const auto &c=d.c;auto &w=*d.solver->work;
  const auto begin=Clock::now();d.deadline=begin+std::chrono::duration_cast<Clock::duration>(std::chrono::duration<double>(c.timeout));
  MpcResult out;
  auto reject=[&](const std::string &reason) {
    std::ostringstream s;s<<" tinympc_status="<<reason<<" iter="<<w.iter<<" primal_residual="<<w.primal_residual_state
      <<" input_primal="<<w.primal_residual_input<<" input_dual="<<w.dual_residual_input<<" dual_residual="<<w.dual_residual_state<<" pose=("<<position.transpose()<<") measured_v=("<<measured.transpose()
      <<") previous_cmd=("<<previous.transpose()<<") solve_ms="<<std::chrono::duration<double,std::milli>(Clock::now()-begin).count();
    out.diagnostics+=s.str();out.reason=reason;reset();return out;
  };
  if(!position.allFinite()||!measured.allFinite()||!previous.allFinite()||!std::isfinite(angular_velocity)) return reject("nonfinite_state");
  if(!d.update_spin_model(angular_velocity)) return reject("spin_model_update_failed");
  if(!std::isfinite(time)||trajectory.segments.empty()) return reject("invalid_trajectory");
  for(const auto &s:trajectory.segments) if(!std::isfinite(s.duration)||s.duration<=0||!s.coefficients.allFinite()) return reject("invalid_trajectory");
  if(!std::isfinite(speed_limit)||speed_limit<=0) return reject("invalid_speed_limit");
  const double vmax=std::min(c.max_speed,speed_limit),rate=c.max_acceleration*c.dt*std::cos(M_PI/8);
  auto feasible=physical(vmax,c.max_acceleration,d.e,d.f,c.dt,measured);
  if(feasible.empty()) return reject("physical_bounds_infeasible");
  auto strict=feasible;
  for(const auto &n:normals) strict=clip(strict,n,n.dot(previous)+rate);
  std::array<double,8> first_bounds;first_bounds.fill(rate);
  if(strict.empty()) {
    const Eigen::Vector2d witness=project(previous,feasible);
    for(int j=0;j<8;++j) {
      first_bounds[j]=std::max(rate,normals[j].dot(witness-previous)+.01*c.max_acceleration*c.dt);
      out.first_command_rate_relaxation=std::max(out.first_command_rate_relaxation,(first_bounds[j]-rate)/c.dt);
    }
  }
  d.first=feasible;
  for(int j=0;j<8;++j) d.first=clip(d.first,normals[j],normals[j].dot(previous)+first_bounds[j]);
  if(d.first.empty()) return reject("command_bounds_infeasible");
  for(auto &v:d.first) v=(v-previous)/c.dt;
  d.speed=octagon(command_scale*vmax);d.acceleration=d.slew=octagon(c.max_acceleration);
  // Rebase position only; velocity, command history and constraints retain fixed axes.
  reset();w.x.col(0)<<0,0,measured.x(),measured.y(),command_scale*previous.x(),command_scale*previous.y(),0,0;
  w.vnew.col(0)=w.x.col(0);
  for(int k=0;k<=c.horizon;++k) {
    const auto ref=trajectory.reference(time+k*c.dt);
    w.Xref.block<2,1>(0,k)=ref.position-position;
    w.Xref.block<2,1>(2,k)=ref.velocity;
    const Eigen::Vector2d acc=ref.acceleration;
    const Eigen::Vector2d reference_command = d.cached_omega == 0 ?
      (ref.velocity+Eigen::Vector2d(c.response_x*acc.x(),c.response_y*acc.y())).eval() :
      (d.f.inverse()*(trajectory.reference(time+(k+1)*c.dt).velocity-d.e*ref.velocity)).eval();
    w.Xref.block<2,1>(4,k)=command_scale*reference_command;
    w.Xref.block<2,1>(6,k)=acc;
  }
  // Seed ADMM with a feasible forward simulation, including disturbance recovery.
  // Starting every state/input at zero is especially poor after an external stop.
  Eigen::Vector2d seed_command=previous,seed_velocity=measured;
  for(int k=0;k<c.horizon;++k) {
    const Eigen::Vector2d desired=w.Xref.block<2,1>(4,k+1)/command_scale +
      (w.Xref.block<2,1>(0,k+1)-w.x.block<2,1>(0,k));
    auto admissible=physical(vmax,c.max_acceleration,d.e,d.f,c.dt,seed_velocity);
    for(int j=0;j<8;++j) admissible=clip(admissible,normals[j],normals[j].dot(seed_command)+(k==0?first_bounds[j]:rate));
    if(admissible.empty()) return reject("seed_constraints");
    const Eigen::Vector2d next=project(desired,admissible);
    w.u.col(k)=(next-seed_command)/c.dt;
    w.x.col(k+1)=w.Adyn*w.x.col(k)+w.Bdyn*w.u.col(k);
    seed_command=next;seed_velocity=w.x.block<2,1>(2,k+1);
  }
  w.v=w.vnew=w.x;w.z=w.znew=w.u;
  w.Uref.setZero();
  if(tiny_solve(d.solver)!=0) return reject(Clock::now()>=d.deadline?"solver_timeout":"solver_not_converged");
  // Roll out actual commands, correcting only numerical projection residuals.
  // Returned positions use this dynamically consistent rollout.
  Eigen::Vector2d p=position,v=measured,cmd=previous;
  for(int k=0;k<c.horizon;++k) {
    const Eigen::Vector2d raw=cmd+c.dt*d.solver->solution->u.col(k);
    auto admissible=physical(vmax,c.max_acceleration,d.e,d.f,c.dt,v);
    for(int j=0;j<8;++j) admissible=clip(admissible,normals[j],normals[j].dot(cmd)+(k==0?first_bounds[j]:rate));
    if(admissible.empty()) return reject("rollout_constraints");
    const Eigen::Vector2d next=project(raw,admissible);
    if((next-raw).norm()>2e-3) return reject("rollout_residual");
    cmd=next;v=d.e*v+d.f*cmd;
    const Eigen::Vector2d next_p=p+c.dt*v;
    if(k==0) out.command=cmd;
    out.predicted_positions.push_back(next_p);p=next_p;
  }
  if(Clock::now()>=d.deadline) return reject("solver_timeout");
  out.success=true;out.reason="success";return out;
}
}
