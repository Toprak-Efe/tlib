#pragma once

#include <concepts>
#include <tlib/control/concepts/holdable.hpp>
#include <utility>

class DifferentiatePolicy {
public:
  template <typename Self, typename T>
  T operator()(this Self &&self, const T &x) {
    return self(std::forward(x));
  }
  template <typename Self> void reset(this Self &&self) { return self.reset(); }

protected:
  DifferentiatePolicy() = default;
}; // DifferentiatePolicy

template <typename Policy, Holdable T>
  requires std::derived_from<Policy, DifferentiatePolicy>
class Differentiator {
public:
  Differentiator() = default;
  template <typename... Args>
  explicit Differentiator(Args &&...args)
      : policy_(std::forward<Args>(args)...) {}
  void reset() { policy_.reset(); }
  T operator()(const T &x) { return policy_(x); }
  T operator()(const T &x) const { return policy_(x); }

private:
  Policy policy_{};
}; // Differentiator

template <Holdable T>
struct FirstDifferenceDifferentiatorPolicy : public DifferentiatePolicy {
public:
  FirstDifferenceDifferentiatorPolicy() = default;

  T operator()(const T &x) {
    if (!init_) {
      init_ = true;
      x1_ = x;
      return T{x.stamp()};
    }

    Timestamp t = x.stamp();
    Timestamp t1 = x1_.stamp();
    auto dt = to_seconds(t - t1);

    T y = (x - x1_) / dt;
    x1_ = x;
    return y;
  }

  T operator()(const T &x) const {
    if (!init_) {
      return T{x.stamp()};
    }

    Timestamp t = x.stamp();
    Timestamp t1 = x1_.stamp();
    auto dt = to_seconds(t - t1);

    return (x - x1_) / dt;
  }

private:
  T x1_{};
  bool init_{false};
}; // FirstDifferenceDifferentiatorPolicy

template <Holdable T>
using FirstDifferenceDifferentiator =
    Differentiator<FirstDifferenceDifferentiatorPolicy<T>, T>;

template <Holdable T>
class CentralDifferenceDifferentiatorPolicy : public DifferentiatePolicy {
public:
  CentralDifferenceDifferentiatorPolicy() = default;
  T operator()(const T &x) {
    if (idx_ != 2) {
      xn_[idx_++] = x;
      return T{x.stamp()};
    }

    Timestamp t = x.stamp();
    Timestamp t1 = xn_[0].stamp();
    auto dt = to_seconds(t - t1);
    T y = (x - xn_[0]) / dt;

    xn_[0] = xn_[1];
    xn_[1] = x;

    return y;
  }

  T operator()(const T &x) const {
    if (idx_ != 2) {
      return T{x.stamp()};
    }

    Timestamp t = x.stamp();
    Timestamp t1 = xn_[0].stamp();
    auto dt = to_seconds(t - t1);
    return (x - xn_[0]) / dt;
  }

private:
  std::array<T, 2> xn_{};
  std::size_t idx_{0};
}; // CentralDifferenceDifferentiatorPolicy

template <Holdable T>
using CentralDifferentiator =
    Differentiator<CentralDifferenceDifferentiatorPolicy<T>, T>;
