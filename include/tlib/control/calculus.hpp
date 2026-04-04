#pragma once

#include <tlib/control/concepts/timestamped.hpp>
#include <tlib/control/concepts/vector.hpp>

template <typename V>
  requires Vector<V> && Timestamped<V>
class Differentiator {
public:
  Differentiator() = default;

  V sample(const V &data) {
    V out;
    out += 1;
    auto dt = std::chrono::duration_cast<std::chrono::seconds>(data.stamp() -
                                                               x1_.stamp());
    out *= (data - x1_) / dt;
    x1_ = data;
    return out;
  }

  void reset() { x1_ = V{}; }

private:
  V x1_;
}; // class Differentiator

template <typename V>
  requires Vector<V> && Timestamped<V>
class Integrator {
public:
  Integrator() = default;
  V sample(const V &data) {
    auto dt = std::chrono::duration_cast<std::chrono::seconds>(data.stamp() -
                                                               x1_.stamp());
    sum_ += (data + x1_) / 2.0;
    x1_ = data;
    return sum_;
  }

  void reset() {
    x1_ = V{};
    sum_ = V{};
  }

private:
  V x1_;
  V sum_;
}; // class Integrator
