#pragma once

#include <chrono>
#include <concepts>

using Timestamp = std::chrono::time_point<std::chrono::steady_clock,
                                          std::chrono::nanoseconds>;
template <typename T>
concept Timestamped = requires(T &a) {
  { a.stamp() } -> std::convertible_to<Timestamp>;
};

template <std::floating_point T = double, typename Rep, typename Period>
constexpr T to_seconds(const std::chrono::duration<Rep, Period> &dur) noexcept {
  std::chrono::duration<T> seconds =
      std::chrono::duration_cast<std::chrono::duration<T>>(dur);
  return seconds.count();
}

inline auto now() { return std::chrono::steady_clock::now(); }
