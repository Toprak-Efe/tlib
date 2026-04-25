#pragma once

#include <array>
#include <cstddef>
#include <initializer_list>
#include <ranges>
#include <stdexcept>

template <typename T, std::size_t Size>
  requires(Size > 1)
class CircularBuffer {
public:
  CircularBuffer() = default;
  CircularBuffer(std::initializer_list<T> elements) {
    for (const T &element : elements) {
      (*this)(element);
    }
  }

  void operator()(const T &x) {
    w_ = (w_ + 1) % Size;
    buffer_[w_] = x;
  }

  auto view() {
    return std::views::iota(std::size_t{0}, Size) |
           std::views::transform([this](std::size_t i) -> T & {
             return buffer_[(w_ + Size - i) % Size];
           });
  }

  auto view() const {
    return std::views::iota(std::size_t{0}, Size) |
           std::views::transform([this](std::size_t i) -> const T & {
             return buffer_[(w_ + Size - i) % Size];
           });
  }

  auto begin() { return view().begin(); }
  auto end() { return view().end(); }
  auto begin() const { return view().begin(); }
  auto end() const { return view().end(); }

  T &operator[](std::size_t idx) {
    if (idx >= Size)
      throw std::out_of_range("Index out of range!");
    return buffer_[(w_ + Size - idx) % Size];
  }

  const T &operator[](std::size_t idx) const {
    if (idx >= Size)
      throw std::out_of_range("Index out of range!");
    return buffer_[(w_ + Size - idx) % Size];
  }

  void reset() {
    buffer_.fill(T{});
  }

private:
  std::size_t w_{Size - 1};
  std::array<T, Size> buffer_{};
}; // CircularBuffer
