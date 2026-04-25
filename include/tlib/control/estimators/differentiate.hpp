#pragma once

#include <concepts>
#include <tlib/control/concepts/arithmetic.hpp>
#include <tlib/control/concepts/holdable.hpp>
#include <tlib/control/filters/fir.hpp>
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

template <typename Policy, ScalarArithmetic T>
  requires std::derived_from<Policy, DifferentiatePolicy>
class Differentiator {
public:
  Differentiator() = default;
  template <typename... Args>
  explicit Differentiator(Args &&...args)
      : policy_(std::forward<Args>(args)...) {}
  void reset() { policy_.reset(); }
  T operator()(const T &x) { return policy_(x); }
  T operator()(const T &x) const {
    return static_cast<const Policy>(policy_)(x);
  }

private:
  Policy policy_{};
}; // Differentiator

template <ScalarArithmetic T, double KernelScale, double... KernelCoefficients>
  requires(KernelScale > 1e-3 || -1e-3 > KernelScale)
class FIRDifferentiatorPolicy : public DifferentiatePolicy {
  static constexpr std::size_t KernelSize = sizeof...(KernelCoefficients);

public:
  FIRDifferentiatorPolicy() = delete;
  FIRDifferentiatorPolicy(double fs)
      : DifferentiatePolicy(), fs_{fs},
        filter_(std::array<double, KernelSize>({KernelCoefficients...})) {}
  T operator()(const T &x) { return filter_(x) * fs_ / KernelScale; }
  T operator()(const T &x) const {
    return static_cast<const FIR<T, KernelSize>>(filter_)(x) * fs_ /
           KernelScale;
  }

private:
  double fs_;
  FIR<T, KernelSize> filter_;
}; // FIRDifferentiatorPolicy

template <ScalarArithmetic T>
using RickLyonsDifferentiator = Differentiator<
    FIRDifferentiatorPolicy<T, 19.0 / 16.0, 3.0 / 16.0, -31.0 / 32.0, 0.0,
                            31.0 / 32.0, -3.0 / 16.0>,
    T>;

template <ScalarArithmetic T>
using CentralDifferenceDifferentiator =
    Differentiator<FIRDifferentiatorPolicy<T, 2.0, 1.0, 0.0, -1.0>, T>;

template <ScalarArithmetic T>
using FirstDifferenceDifferentiator =
    Differentiator<FIRDifferentiatorPolicy<T, 1.0, 1.0, -1.0>, T>;
