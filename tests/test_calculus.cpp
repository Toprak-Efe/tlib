#include "catch2/matchers/catch_matchers.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"
#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <cstdlib>
#include <tlib/control/estimators/differentiate.hpp>
#include <tlib/control/spatial.hpp>

std::vector<double> generate_sinusoidal(std::size_t l = 10000, double fw = 1.0,
                                        double fs = 1000.0, double A = 1.0,
                                        double pho = 0.0) {
  std::vector<double> sinusoidal;
  sinusoidal.reserve(l);

  for (std::size_t i = 0; i < l; i++) {
    double t = i / fs;
    double y = A * std::sin(t * fw * 2 * std::numbers::pi + pho);
    sinusoidal.emplace_back(y);
  }

  return sinusoidal;
}

template <typename Filter> void test_for_90_degrees_shift(double fw = 1.0) {
  constexpr std::size_t samples = 10'000;
  constexpr double amplitude = 1.0;
  constexpr double fs = 1'000;

  Filter filter{fs};
  std::vector<double> sin =
      generate_sinusoidal(samples, fw, fs, amplitude, 0.0);

  std::vector<double> y;
  y.reserve(samples);
  for (std::size_t i = 0; i < samples; i++)
    y.push_back(filter(sin[i]));

  // sin(pi*fw*dt) / (pi*fw*dt)
  double gain_correction =
      std::sin(std::numbers::pi * fw / fs) / (std::numbers::pi * fw / fs);

  for (std::size_t i = 1; i < samples; i++) {
    double t_mid = (i - 0.5) / fs;
    // sin(pi*fw*dt)*2*pi*cos(2*pi*fw*dt*t) / (pi*fw*dt)
    double ref = gain_correction * amplitude * 2.0 * std::numbers::pi * fw *
                 std::cos(2.0 * std::numbers::pi * fw * t_mid);

    CHECK_THAT(y[i], Catch::Matchers::WithinAbs(ref, 1e-4) ||
                         Catch::Matchers::WithinRel(ref, 1e-2));
  }
}

TEST_CASE("FirstDifference", "[Differentiation]") {
  SECTION("Sanity Test") {
    test_for_90_degrees_shift<FirstDifferenceDifferentiator<double>>(
        1.0); // 1 Hz
    test_for_90_degrees_shift<FirstDifferenceDifferentiator<double>>(
        10.0); // 10 Hz
    test_for_90_degrees_shift<FirstDifferenceDifferentiator<double>>(
        50.0); // 50 Hz
    test_for_90_degrees_shift<FirstDifferenceDifferentiator<double>>(
        100.0); // 100 Hz
    test_for_90_degrees_shift<FirstDifferenceDifferentiator<double>>(
        250.0); // 250 Hz
  }
}

template <typename Filter>
void test_frequency_response(double fw, double gain_correction,
                             std::size_t group_delay_samples,
                             std::size_t warmup) {
  constexpr std::size_t samples = 10'000;
  constexpr double amplitude = 1.0;
  constexpr double fs = 1'000;

  Filter filter{fs};
  std::vector<double> sig =
      generate_sinusoidal(samples, fw, fs, amplitude, 0.0);

  std::vector<double> y;
  y.reserve(samples);
  for (std::size_t i = 0; i < samples; i++)
    y.push_back(filter(sig[i]));

  for (std::size_t i = warmup; i < samples; i++) {
    double t_ref = (static_cast<double>(i) - group_delay_samples) / fs;
    double ref = gain_correction * amplitude * 2.0 * std::numbers::pi * fw *
                 std::cos(2.0 * std::numbers::pi * fw * t_ref);

    CHECK_THAT(y[i], Catch::Matchers::WithinAbs(ref, 1e-6) ||
                         Catch::Matchers::WithinRel(ref, 1e-3));
  }
}

TEST_CASE("CentralDifference", "[Differentiation]") {
  constexpr double fs = 1'000;
  auto sinc_central = [](double fw) {
    double x = 2.0 * std::numbers::pi * fw / fs;
    return std::sin(x) / x;
  };

  for (double fw : {1.0, 10.0, 50.0, 100.0, 200.0, 300.0}) {
    test_frequency_response<CentralDifferenceDifferentiator<double>>(
        fw, sinc_central(fw),
        /*group_delay=*/1,
        /*warmup=*/2);
  }
}

TEST_CASE("Calculus", "[Integral]") {}
