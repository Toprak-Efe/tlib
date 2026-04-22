#pragma once

#include <array>
#include <tlib/control/concepts/vector.hpp>

template <Vector T, std::size_t WindowSize> class MovingAverage {
  static_assert(WindowSize > 1, "WindowSize must be larger than 1.");

public:
  MovingAverage() = default;
  T operator()(const T &x) {
    T y = x;
    for (const auto &sample : samples_) {
      y += sample;
    }
    y /= static_cast<double>(WindowSize);

    for (std::size_t i = WindowSize - 2; 0 < i; i--) {
      samples_[i] = samples_[i - 1];
    }
    samples_[0] = x;

    return y;
  }

private:
  std::array<T, WindowSize - 1> samples_;
}; // class MovingAverage
