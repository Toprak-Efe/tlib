#pragma once

#include <cstddef>
#include <initializer_list>
#include <tlib/control/buffer.hpp>
#include <tlib/control/concepts/arithmetic.hpp>

template <ScalarArithmetic T, std::size_t KernelSize>
  requires(KernelSize > 1)
class FIR {
public:
  FIR() = default;
  FIR(const std::array<double, KernelSize> &kernel)
      : kernel_(kernel), buffer_() {}
  FIR(const std::initializer_list<double> &kernel) : kernel_(), buffer_() {
    for (std::size_t i = 0; i < KernelSize; i++)
      kernel_[i] = *(kernel.begin() + i);
  }

  T operator()(const T &x) {
    buffer_(x);
    T out{};
    for (std::size_t i = 0; i < KernelSize; i++) {
      out += kernel_[i] * buffer_[i];
    }
    return out;
  }
  T operator()(const T &x) const {
    T out{};
    for (std::size_t i = 1; i < KernelSize; i++) {
      out += kernel_[i] * buffer_[i - 1];
    }
    return out + kernel_[0] * x;
  }

  void configure(const std::array<double, KernelSize> &kernel) {
    kernel_ = kernel;
  }

  void reset() { buffer_.reset(); }

private:
  std::array<double, KernelSize> kernel_{};
  CircularBuffer<T, KernelSize> buffer_{};

}; // FIR
