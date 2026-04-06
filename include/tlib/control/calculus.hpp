#pragma once

#include <tlib/control/concepts/holdable.hpp>

template <Holdable T> class Differentiator {
public:
  T operator()(const T &data) {
    auto dt = std::chrono::duration<double>(data.stamp() - x1_.stamp()).count();

    T out{};
    if (dt > 1e-9) {
      out = (data - x1_) / dt;
      out.stamp() = data.stamp();
    }
    x1_ = data;
    return out;
  }

  void reset() {
    T zero{};
    zero.stamp() = x1_.stamp();
    x1_ = zero;
  }

private:
  T x1_{};
  bool first_{true};
}; // Differentiator

template <Holdable T> class DecayingIntegrator {
public:
  explicit DecayingIntegrator(double decay = 0.995) : decay_(decay) {}

  T operator()(const T &data) {
    auto dt = std::chrono::duration<double>(data.stamp() - x1_.stamp()).count();

    if (dt > 1e-9) {
      accum_ *= decay_;
      accum_ += (data + x1_) * (0.5 * dt);
      accum_.stamp() = data.stamp();
    }
    x1_ = data;
    return accum_;
  }

private:
  double decay_;
  T x1_{};
  T accum_{};
}; // class DecayingIntegrator
