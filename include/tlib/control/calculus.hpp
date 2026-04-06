#pragma once

#include <tlib/control/concepts/timestamped.hpp>
#include <tlib/control/concepts/vector.hpp>

template <typename V, typename F>
  requires Vector<V> && Timestamped<V> && Vector<F> && Timestamped<F>
class Differentiator {
public:
  Differentiator() = default;

  F sample(const V &data) {
    F out{};
    out += 1;
    auto dt = std::chrono::duration<double>(data.stamp() - x1_.stamp()).count();
    out *= reinterpret_cast<F>((data - x1_) / dt);
    x1_ = data;
    return out;
  }

  void reset() { x1_ = V{}; }

private:
  V x1_;
}; // class Differentiator

template <typename V, typename F>
  requires Vector<V> && Timestamped<V> && Vector<F> && Timestamped<F>
class LeakyIntegrator {
public:
  LeakyIntegrator() : leak_(0.995), x1_(), sum_() {}
  LeakyIntegrator(double leak) : leak_(leak), x1_(), sum_() {}
  F sample(const V &data) {
    auto dt = std::chrono::duration<double>(data.stamp() - x1_.stamp()).count();
    sum_ *= leak_;
    sum_ += reinterpret_cast<F>((data + x1_) / 2.0);
    x1_ = data;
    return sum_;
  }

  void reset() {
    x1_ = V{};
    sum_ = V{};
  }

private:
  double leak_;
  V x1_;
  F sum_;
}; // class Integrator
