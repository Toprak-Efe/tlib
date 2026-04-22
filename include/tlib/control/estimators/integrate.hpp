#pragma once

#include <tlib/control/concepts/holdable.hpp>
#include <utility>

class IntegrationPolicy {
public:
  template <typename Self, typename T>
  T operator()(this Self &&self, const T &x) {
    return self(std::forward(x));
  }
  template <typename Self> void reset(this Self &&self) { return self.reset(); }

protected:
  IntegrationPolicy() = default;
}; // IntegrationPolicy

template <typename Policy, Holdable T>
  requires requires(Policy p) { std::derived_from<Policy, IntegrationPolicy>; }
class Integrator {
public:
  Integrator() = default;
  template <typename... Args>
  explicit Integrator(Args &&...args) : policy_(std::forward<Args>(args)...) {}
  void reset() { policy_.reset(); }
  T operator()(const T &x) { return policy_(x); }
  T operator()(const T &x) const { return policy_(x); }

private:
  Policy policy_{};
}; // Integrator

template <Holdable T> class EulerIntegrationPolicy : public IntegrationPolicy {
public:
  EulerIntegrationPolicy() = default;

  T operator()(const T &x) {
    if (!init_) {
      init_ = true;
      return (y1_ = x);
    }

    Timestamp t = x.stamp();
    Timestamp t1 = y1_.stamp();
    auto dt = (t - t1).count();

    y1_ += x * dt;
    return y1_;
  }

  T operator()(const T &x) const {
    if (!init_) {
      return x;
    }

    Timestamp t = x.stamp();
    Timestamp t1 = y1_.stamp();
    auto dt = (t - t1).count();

    T y = y1_ + x * dt;
    return y;
  }

  void reset() {
    init_ = false;
    y1_ = T{y1_.stamp()};
  }

private:
  T y1_{};
  bool init_{false};
}; // EulerIntegrationPolicy

template <Holdable T> class TrapezoidalIntegrationPolicy : public IntegrationPolicy {
public:
  TrapezoidalIntegrationPolicy() = default;

  T operator()(const T &x) {
    if (!init_) {
      init_ = true;
      return x1_ = (y1_ = x);
    }

    Timestamp t = x.stamp();
    Timestamp t1 = x1_.stamp();
    auto dt = (t - t1).count();

    y1_ += (x + x1_) * dt / 2.0;
    x1_ = x;
    return y1_;
  }

  T operator()(const T &x) const {
    if (!init_) {
      return x;
    }

    Timestamp t = x.stamp();
    Timestamp t1 = x1_.stamp();
    auto dt = (t - t1).count();

    T y = y1_ + (x + x1_) * dt / 2.0;
    return y;
  }

  void reset() {
    init_ = false;
    x1_ = T{x1_.stamp()};
    y1_ = T{y1_.stamp()};
  }

private:
  T x1_{};
  T y1_{};
  bool init_{false};
}; // TrapezoidalIntegrationPolicy
