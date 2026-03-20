#pragma once

#include <chrono>

using Timestamp = std::chrono::time_point<std::chrono::steady_clock>;
template <typename T>
concept Timestamped = requires(T a) {
  { a.timestamp } -> std::same_as<Timestamp &>;
};
