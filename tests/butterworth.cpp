#include <array>
#include <cmath>
#include <complex>
#include <numbers>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <tlib/control/biquad.hpp>
#include <tlib/control/butterworth.hpp>

using Catch::Matchers::WithinAbs;
using Catch::Matchers::WithinRel;

namespace {

std::complex<double> biquad_response(const BiquadCoefficients &c,
                                     double omega) {
  using namespace std::complex_literals;
  const std::complex<double> z1 = std::exp(-1i * omega);
  const std::complex<double> z2 = std::exp(-2.0i * omega);
  const std::complex<double> num = c.b0 + c.b1 * z1 + c.b2 * z2;
  const std::complex<double> den = 1.0 + c.a1 * z1 + c.a2 * z2;
  return num / den;
}

std::complex<double> biquad_cascade_response(const auto &coeffs_range,
                                             double omega) {
  std::complex<double> H = 1.0;
  for (const auto &c : coeffs_range) {
    H *= biquad_response(c, omega);
  }
  return H;
}

template <typename T, size_t Order>
std::complex<double> eval_H(const Butterworth<T, Order> &filter, double freq_hz,
                            double fs_hz) {
  const double omega = 2.0 * M_PI * freq_hz / fs_hz;
  std::complex<double> H = 1.0;
  if constexpr (Order % 2 == 1) {
    H *= biquad_response(filter.first_order_coefficients(), omega);
  }
  H *= biquad_cascade_response(filter.biquad_coefficients(), omega);
  return H;
}

template <typename T, size_t Order>
double magnitude(const Butterworth<T, Order> &filter, double freq_hz,
                 double fs_hz) {
  return std::abs(eval_H(filter, freq_hz, fs_hz));
}

template <typename T, size_t Order>
double magnitude_db(const Butterworth<T, Order> &filter, double freq_hz,
                    double fs_hz) {
  return 20.0 * std::log10(magnitude(filter, freq_hz, fs_hz));
}

template <typename T, size_t Order>
double find_cutoff_hz(const Butterworth<T, Order> &filter, double fs_hz,
                      double target_magnitude = 1.0 /
                                                std::numbers::sqrt2_v<double>) {
  double lo = 0.0;
  double hi = fs_hz / 2.0;
  for (int iter = 0; iter < 100; ++iter) {
    const double mid = 0.5 * (lo + hi);
    if (magnitude(filter, mid, fs_hz) > target_magnitude) {
      lo = mid;
    } else {
      hi = mid;
    }
    if (hi - lo < 1e-9)
      break;
  }
  return 0.5 * (lo + hi);
}

template <size_t Order>
void check_butterworth_properties(double fc_target, double fs) {
  Butterworth<double, Order> filter{fc_target, fs};

  INFO("Order=" << Order << " fc=" << fc_target << " fs=" << fs);

  // DC gain = 1
  CHECK_THAT(magnitude(filter, 0.0, fs), WithinAbs(1.0, 1e-10));

  // Nyquist gain = 0 (zeros at z = -1 from bilinear transform)
  CHECK_THAT(magnitude(filter, fs / 2.0, fs), WithinAbs(0.0, 1e-10));

  // Cutoff gain = 1/sqrt(2) by Butterworth definition
  const double sqrt2_inv = 1.0 / std::numbers::sqrt2_v<double>;
  CHECK_THAT(magnitude(filter, fc_target, fs), WithinAbs(sqrt2_inv, 1e-6));

  // Measured cutoff matches target cutoff (within 0.1%)
  const double fc_measured = find_cutoff_hz(filter, fs);
  CHECK_THAT(fc_measured, WithinRel(fc_target, 1e-3));

  // Phase at DC = 0 for real-coefficient filter
  CHECK_THAT(std::arg(eval_H(filter, 0.0, fs)), WithinAbs(0.0, 1e-10));

  // // Asymptotic rolloff check: valid only when we have room past cutoff to
  // reach the asymptote, and room below Nyquist to avoid warping.
  const double f_lo = 10.0 * fc_target;
  const double f_hi = 20.0 * fc_target;
  if (f_hi < 0.4 * fs && f_lo > fc_target * 2.0) {
    const double slope_per_octave =
        magnitude_db(filter, f_hi, fs) - magnitude_db(filter, f_lo, fs);
    const double expected_slope = -6.0 * static_cast<double>(Order);
    CHECK_THAT(slope_per_octave, WithinAbs(expected_slope, 5.0));
  }

  // Monotonic magnitude decrease from DC to Nyquist
  double prev = magnitude(filter, 0.0, fs);
  for (double f = 1.0; f < fs / 2.0; f *= 1.1) {
    const double m = magnitude(filter, f, fs);
    CHECK(m <= prev + 1e-10);
    prev = m;
  }
}

} // namespace

TEST_CASE("Butterworth satisfies filter properties across orders",
          "[Butterworth]") {
  SECTION("Order 1") { check_butterworth_properties<1>(20.0, 1000.0); }
  SECTION("Order 2") { check_butterworth_properties<2>(100.0, 1000.0); }
  SECTION("Order 3 (FO + 1 biquad)") {
    check_butterworth_properties<3>(50.0, 1000.0);
  }
  SECTION("Order 4") { check_butterworth_properties<4>(20.0, 1000.0); }
  SECTION("Order 5 (FO + 2 biquads)") {
    check_butterworth_properties<5>(100.0, 1000.0);
  }
  SECTION("Order 6") { check_butterworth_properties<6>(150.0, 1000.0); }
  SECTION("Order 7 (FO + 3 biquads)") {
    check_butterworth_properties<7>(80.0, 1000.0);
  }
  SECTION("Order 8") { check_butterworth_properties<8>(100.0, 1000.0); }
}

TEST_CASE("Butterworth cutoff accuracy across frequencies", "[Butterworth]") {
  constexpr double fs = 48000.0;

  SECTION("Very low cutoff") { check_butterworth_properties<4>(50.0, fs); }
  SECTION("Low cutoff") { check_butterworth_properties<4>(500.0, fs); }
  SECTION("Mid cutoff") { check_butterworth_properties<4>(5000.0, fs); }
  SECTION("High cutoff") { check_butterworth_properties<4>(15000.0, fs); }
  SECTION("Near-Nyquist") { check_butterworth_properties<4>(20000.0, fs); }
}

TEST_CASE("Butterworth time-domain sanity", "[Butterworth][time]") {
  SECTION("Step response converges to DC gain") {
    Butterworth<double, 4> filter{20.0, 1000.0};
    double y = 0.0;
    for (int n = 0; n < 10000; ++n) {
      y = filter(1.0);
    }
    CHECK_THAT(y, WithinAbs(1.0, 1e-6));
  }

  SECTION("Impulse response is finite and decays") {
    constexpr size_t N = 2048;
    Butterworth<double, 4> filter{20.0, 1000.0};

    std::array<double, N> h;
    h[0] = filter(1.0);
    for (size_t n = 1; n < N; ++n)
      h[n] = filter(0.0);

    for (double sample : h) {
      CHECK(std::isfinite(sample));
    }
    CHECK(std::abs(h[N - 1]) < 1e-3);
  }
}
