#pragma once

#include <cstddef>
#include <tlib/control/concepts/arithmetic.hpp>
#include <tlib/control/physics.hpp>

template <typename T> struct Clamper {
  static_assert(SelfComparable<T>);
  Clamper() = delete;
  template <T Min, T Max> static T clamp(const T &val) {
    static_assert(Max >= Min);
    if (val > Max)
      return Max;
    if (val < Min)
      return Min;
    return val;
  }
}; // struct Clamper

template <typename T> struct Clamper<SpatialVector<T>> {
  static_assert(SelfComparable<T>);
  Clamper() = delete;
  template <SpatialVector<T> Min, SpatialVector<T> Max>
  static SpatialVector<T> clamp(const SpatialVector<T> &val) {
    const auto &min_vec = Min.vec();
    const auto &max_vec = Max.vec();
    for (std::size_t i = 0; i < 6; i++) {
      static_assert(max_vec.array()[i] >= min_vec.array()[i]);
    }

    SpatialVector<T> out;
    for (std::size_t i = 0; i < 6; i++) {
      const auto s = val.vec().array()[i];
      const auto max = Max.vec().array()[i];
      const auto min = Min.vec().array()[i];
      double out_v = s;
      if (s > max)
        out_v = max;
      if (min > s)
        out_v = min;
      out.vec().array()[i] = out_v;
    }
    return out;
  }

}; // struct Clamper
