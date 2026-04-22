#pragma once

#include <array>
#include <cmath>
#include <tlib/control/biquad.hpp>
#include <tlib/control/concepts/holdable.hpp>

template <Vector T, size_t Order> class Butterworth {
  static_assert(Order != 0);
  static constexpr size_t NumOfBiquads = Order / 2;
  static constexpr bool HasFirstFilter = Order % 2 == 1;

public:
  Butterworth(double fs_c, double fs) : fob_() {
    double wc_d = 2 * M_PI * fs_c;
    const double K = 2.0 * fs;
    const double K_2 = K * K;
    const double wc = K * std::tan(wc_d / K);

    if constexpr (HasFirstFilter) {
      const double wcT = wc / fs;
      const double denom = (2 + wcT);
      const double b0 = wcT / denom;
      const double b1 = b0;
      const double b2 = 0.0;
      const double a1 = (wcT - 2.0) / denom;
      const double a2 = 0.0;
      fob_.configure(b0, b1, b2, a1, a2);
    }

    const double wc2 = wc * wc;
    for (size_t m = 0; m < NumOfBiquads; m++) {
      const size_t idx = NumOfBiquads - 1 - m;
      const double theta = M_PI_2 + M_PI * (2 * idx + 1) / (2 * Order);
      const double b = -2.0 * wc * std::cos(theta);
      const double D = K_2 + b * K + wc2;
      const double D_m = K_2 - b * K + wc2;
      const double b0 = wc2 / D;
      const double b1 = b0 * 2.0;
      const double b2 = b0;
      const double a1 = 2.0 * (wc2 - K_2) / D;
      const double a2 = D_m / D;
      biquads_[m] = Biquad<T>(b0, b1, b2, a1, a2);
    }
  }

  T operator()(const T &x) {
    T y = x;
    if constexpr (HasFirstFilter) {
      y = fob_(y);
    }
    for (auto &biquad : biquads_) {
      y = biquad(y);
    }
    return y;
  }

  std::array<BiquadCoefficients, NumOfBiquads> biquad_coefficients() const {
    std::array<BiquadCoefficients, NumOfBiquads> result;
    for (size_t m = 0; m < NumOfBiquads; ++m) {
      result[m] = biquads_[m].coefficients();
    }
    return result;
  }

  BiquadCoefficients first_order_coefficients() const
    requires(HasFirstFilter)
  {
    return fob_.coefficients();
  }

private:
  Biquad<T> fob_; // unused for even Order
  std::array<Biquad<T>, NumOfBiquads> biquads_;
}; // class Butterworth
