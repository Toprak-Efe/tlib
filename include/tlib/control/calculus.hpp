#pragma once

#include <chrono>
#include <cmath>
#include <tlib/control/concepts/holdable.hpp>

template <Holdable T> class Differentiator {
public:
  Differentiator() = default;

  void reset() {
    init_ = false;
    x1_ = T{};
  }

  T operator()(const T &x) {
    T out;
    if (init_) {
      auto dt = std::chrono::duration<double>(x.stamp() - x1_.stamp()).count();
      out = (x - x1_) / dt;
    } else {
      init_ = true;
      out = T{};
      out.stamp() = x.stamp();
    }
    x1_ = x;
    return out;
  }

private:
  T x1_{};
  bool init_{false};
}; // Differentiator

template <Holdable T> class DirtyDifferentiator {
public:
  explicit DirtyDifferentiator(double tau) : tau_(tau) {}

  T operator()(const T &x) {
    if (!init_) {
      init_ = true;
      T out{};
      out.stamp() = x.stamp();
      return out;
    }
    auto dt = std::chrono::duration<double>(x.stamp() - x1_.stamp()).count();
    T out = (x - x1_ - y1_ * (dt / 2.0 - tau_)) / (tau_ + dt / 2.0);
    out.stamp() = x.stamp();
    x1_ = x;
    y1_ = out;
    return out;
  }

  void reset() {
    T zero{};
    zero.stamp() = x1_.stamp();
    x1_ = zero;
    zero.stamp() = y1_.stamp();
    y1_ = zero;
    init_ = false;
  }

private:
  bool init_;
  double tau_;
  T x1_{};
  T y1_{};
}; // DirtyDifferentiator

template <Holdable T> class Integrator {
public:
  Integrator() { sum_.stamp() = std::chrono::steady_clock::now(); }

  void reset() {
    sum_ = T{};
    sum_.stamp() = std::chrono::steady_clock::now();
  }

  T operator()(const T &x) {

  }

  T operator()() const {
      return sum_;
  }

private:
  T y1_;
}; // Integrator

template <Holdable T> class LeakyIntegrator {
public:
  explicit LeakyIntegrator(double decay_per_sec = 0.5)
      : decay_(decay_per_sec), init_(false) {}

  T operator()(const T &data) {
    if (!init_) {
      x1_ = data;
      init_ = true;
      accum_.stamp() = data.stamp();
      return accum_;
    }

    auto dt = std::chrono::duration<double>(data.stamp() - x1_.stamp()).count();

    if (dt > 1e-9) {
      accum_ = accum_ * pow(decay_, dt) + (data + x1_) * 0.5 * dt;
      accum_.stamp() = data.stamp();
    }
    x1_ = data;
    return accum_;
  }

  void reset() {
    x1_ = T{};
    accum_ = T{};
    init_ = false;
  }

private:
  bool init_;
  double decay_;
  T x1_{};
  T accum_{};
}; // LeakyIntegrator
