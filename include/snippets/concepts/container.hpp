#pragma once

#include <ranges>

template <typename T>
concept Container = requires(T a) {
  { a.data } -> std::ranges::range;
};
