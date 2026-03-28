#pragma once

#include <concepts>
#include <stdexcept>
#include <tlib/control/concepts/arithmetic.hpp>
#include <tlib/control/physics.hpp>

template <typename T>
  requires SelfArithmetic<T> && std::copy_constructible<T>
class Clamper {
public:
  Clamper() = delete;
  Clamper(const T &min, const T &max) : min_{min}, max_{max} {
    if (min > max) {
      throw std::invalid_argument{
          "Minimum clamp value must be smaller or equal to the maximum."};
    }
  }
  T clamp(const T &val) {
    if (val > max_)
      return max_;
    if (val < min_)
      return min_;
    return val;
  }

private:
  T min_;
  T max_;
}; // struct Clamper

template <typename T> class Clamper<SpatialVector<T>> {
public:
  Clamper() = delete;
  Clamper(const SpatialVector<T> &min, const SpatialVector<T> &max)
      : min_{min}, max_{max} {
    for (std::size_t i = 0; i < 6; i++) {
      if (min.vec()[i] > max.vec()[i]) {
        throw std::invalid_argument{"Minimum clamp values must be smaller or "
                                    "equal to the maximum ones."};
      }
    }
  }

  SpatialVector<T> clamp(const SpatialVector<T> &val) {
    SpatialVector<T> out;
    for (std::size_t i = 0; i < 6; i++) {
      const auto s = val.vec().array()[i];
      const auto max = max_.vec().array()[i];
      const auto min = min_.vec().array()[i];
      double out_v = s;
      if (s > max)
        out_v = max;
      if (min > s)
        out_v = min;
      out.vec().array()[i] = out_v;
    }
    return out;
  }

private:
  SpatialVector<T> min_;
  SpatialVector<T> max_;
}; // struct Clamper
