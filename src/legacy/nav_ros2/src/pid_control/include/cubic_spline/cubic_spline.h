#ifndef CPPROBOTICS_CUBIC_SPLINE_H
#define CPPROBOTICS_CUBIC_SPLINE_H

#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <Eigen/Dense>
#include <stdexcept>
#include "cpprobotics_types.h"  // 确保cpprobotics_types.h在ROS 2项目中可用

namespace cpprobotics {

// 差分函数
Vec_f vec_diff(const Vec_f& input) {
    Vec_f output;
    for (size_t i = 1; i < input.size(); i++) {
        output.push_back(input[i] - input[i - 1]);
    }
    return output;
}

// 累积和函数
Vec_f cum_sum(const Vec_f& input) {
    Vec_f output;
    float temp = 0;
    for (size_t i = 0; i < input.size(); i++) {
        temp += input[i];
        output.push_back(temp);
    }
    return output;
}

// 三次样条插值类
class Spline {
public:
    Vec_f x;
    Vec_f y;
    int nx;
    Vec_f h;
    Vec_f a;
    Vec_f b;
    Vec_f c;
    Vec_f d;

    Spline() = default;

    // 构造函数
    Spline(const Vec_f& x_, const Vec_f& y_) : x(x_), y(y_), nx(x_.size()), h(vec_diff(x_)), a(y_) {
        Eigen::MatrixXf A = calc_A();
        Eigen::VectorXf B = calc_B();
        Eigen::VectorXf c_eigen = A.colPivHouseholderQr().solve(B);
        c.assign(c_eigen.data(), c_eigen.data() + c_eigen.rows());

        for (int i = 0; i < nx - 1; i++) {
            d.push_back((c[i + 1] - c[i]) / (3.0 * h[i]));
            b.push_back((a[i + 1] - a[i]) / h[i] - h[i] * (c[i + 1] + 2 * c[i]) / 3.0);
        }
    }

    // 计算插值点的值
    float calc(float t) {
        if (t < x.front() || t > x.back()) {
            throw std::invalid_argument("received value out of the pre-defined range");
        }
        int seg_id = bisect(t, 0, nx);
        float dx = t - x[seg_id];
        return a[seg_id] + b[seg_id] * dx + c[seg_id] * dx * dx + d[seg_id] * dx * dx * dx;
    }

    // 计算插值点的一阶导数
    float calc_d(float t) {
        if (t < x.front() || t > x.back()) {
            throw std::invalid_argument("received value out of the pre-defined range");
        }
        int seg_id = bisect(t, 0, nx - 1);
        float dx = t - x[seg_id];
        return b[seg_id] + 2 * c[seg_id] * dx + 3 * d[seg_id] * dx * dx;
    }

    // 计算插值点的二阶导数
    float calc_dd(float t) {
        if (t < x.front() || t > x.back()) {
            throw std::invalid_argument("received value out of the pre-defined range");
        }
        int seg_id = bisect(t, 0, nx);
        float dx = t - x[seg_id];
        return 2 * c[seg_id] + 6 * d[seg_id] * dx;
    }

private:
    // 计算矩阵A
    Eigen::MatrixXf calc_A() {
        Eigen::MatrixXf A(nx, nx);
        A.setZero();
        A(0, 0) = 1;
        for (int i = 0; i < nx - 1; i++) {
            if (i != nx - 2) {
                A(i + 1, i + 1) = 2 * (h[i] + h[i + 1]);
            }
            A(i + 1, i) = h[i];
            A(i, i + 1) = h[i];
        }
        A(0, 1) = 0.0;
        A(nx - 1, nx - 2) = 0.0;
        A(nx - 1, nx - 1) = 1.0;
        return A;
    }

    // 计算向量B
    Eigen::VectorXf calc_B() {
        Eigen::VectorXf B(nx);
        B.setZero();
        for (int i = 0; i < nx - 2; i++) {
            B(i + 1) = 3.0 * (a[i + 2] - a[i + 1]) / h[i + 1] - 3.0 * (a[i + 1] - a[i]) / h[i];
        }
        return B;
    }

    // 二分查找
    int bisect(float t, int start, int end) {
        int mid = (start + end) / 2;
        if (t == x[mid] || end - start <= 1) {
            return mid;
        } else if (t > x[mid]) {
            return bisect(t, mid, end);
        } else {
            return bisect(t, start, mid);
        }
    }
};

// 二维三次样条插值类
class Spline2D {
public:
    Spline sx;
    Spline sy;
    Vec_f s;

    // 构造函数
    Spline2D(const Vec_f& x, const Vec_f& y) {
        s = calc_s(x, y);
        sx = Spline(s, x);
        sy = Spline(s, y);
    }

    // 计算位置
    Poi_f calc_position(float s_t) {
        float x = sx.calc(s_t);
        float y = sy.calc(s_t);
        return {x, y};
    }

    // 计算曲率
    float calc_curvature(float s_t) {
        float dx = sx.calc_d(s_t);
        float ddx = sx.calc_dd(s_t);
        float dy = sy.calc_d(s_t);
        float ddy = sy.calc_dd(s_t);
        return (ddy * dx - ddx * dy) / (dx * dx + dy * dy);
    }

    // 计算航向角
    float calc_yaw(float s_t) {
        float dx = sx.calc_d(s_t);
        float dy = sy.calc_d(s_t);
        return std::atan2(dy, dx);
    }

private:
    // 计算累积弧长
    Vec_f calc_s(const Vec_f& x, const Vec_f& y) {
        Vec_f ds;
        Vec_f out_s{0};
        Vec_f dx = vec_diff(x);
        Vec_f dy = vec_diff(y);

        for (size_t i = 0; i < dx.size(); i++) {
            ds.push_back(std::sqrt(dx[i] * dx[i] + dy[i] * dy[i]));
        }

        Vec_f cum_ds = cum_sum(ds);
        out_s.insert(out_s.end(), cum_ds.begin(), cum_ds.end());
        return out_s;
    }
};

}  // namespace cpprobotics

#endif  // CPPROBOTICS_CUBIC_SPLINE_H