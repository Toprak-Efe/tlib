#pragma once

#include "tlib/control/concepts/vector.hpp"
#include <cmath>
#include <concepts>
#include <numeric>
#include <stdexcept>
#include <tlib/control/concepts/arithmetic.hpp>
#include <tlib/control/spatial.hpp>
#include <type_traits>
#include <utility>

struct T_ClampingPolicy {}; // struct T_ClampingPolicy

template <typename P, typename T>
concept IsClampingPolicy = requires(P policy, const T &x) {
  { policy(x) } -> std::same_as<T>;
  { policy.reset() } -> std::same_as<void>;
  requires std::same_as<typename P::policy_tag, T_ClampingPolicy>;
};

template <typename Policy, typename T>
  requires IsClampingPolicy<Policy, T>
class Clamper {
public:
  Clamper() = default;

  template <typename... Args>
  explicit Clamper(Args &&...args) : policy_(std::forward<Args>(args)...) {}

  void reset() { policy_.reset(); }

  T operator()(const T &x) { return policy_(x); }
  T operator()(const T &x) const { return policy_(x); }

private:
  Policy policy_{};
};

template <Vector T> struct HardClipPolicy {
  using policy_tag = T_ClampingPolicy;
  HardClipPolicy() = delete;
  HardClipPolicy(const T &max) : max{max} {}

  T operator()(const T &x) const {
    if (max * max < x * x) {
      return std::sqrt(max * max / x * x) * x;
    }
  }

  void reset() {}
  T max{};
}; // struct HardClipPolicy

template <Vector T> struct TanhClipPolicy {
  using policy_tag = T_ClampingPolicy;
  TanhClipPolicy() = delete;
  TanhClipPolicy(const T &max) : max2{max*max} {}

  T operator()(const T &x) const {
    
  }

  void reset() {}
  T max2{};
}; // struct TanhClipPolicy
