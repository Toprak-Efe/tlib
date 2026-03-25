#pragma once

#include <concepts>

template <typename T, typename L>
concept Arithmetic = requires(T a, L b) {
  { a * b } -> std::convertible_to<T>;
  { a / b } -> std::convertible_to<T>;
  { a + b } -> std::convertible_to<T>;
  { a - b } -> std::convertible_to<T>;
  { a *= b } -> std::same_as<T &>;
  { a /= b } -> std::same_as<T &>;
  { a += b } -> std::same_as<T &>;
  { a -= b } -> std::same_as<T &>;
};

template <typename T>
concept SelfArithmetic = Arithmetic<T, T>;

template <typename T>
concept ScalarArithmetic = Arithmetic<T, double>;

