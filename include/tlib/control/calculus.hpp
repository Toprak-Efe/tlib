#pragma once

#include <cmath>
#include <tlib/control/concepts/holdable.hpp>

template <Holdable T> class Differentiator {
public:
  explicit Differentiator(double tau) : tau_(tau) {}

  T operator()(const T &x) {
    auto dt = std::chrono::duration<double>(x.stamp() - x1_.stamp()).count();

    T out((x - x1_ - y1_ * (dt / 2.0 - tau_)) / (tau_ + dt / 2.0));
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
  }

private:
  double tau_;
  T x1_{};
  T y1_{};
}; // Differentiator

template <Holdable T> class DecayingIntegrator {
public:
  explicit DecayingIntegrator(double decay_per_sec = 0.5)
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
}; // class DecayingIntegrator
