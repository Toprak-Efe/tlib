#pragma once

#include <chrono>

using Timestamp = std::chrono::time_point<std::chrono::steady_clock,
                                          std::chrono::nanoseconds>;
template <typename T>
concept Timestamped = requires(T a) {
  { a.stamp() } -> std::convertible_to<Timestamp>;
};
