#include "catch2/matchers/catch_matchers.hpp"
#include "catch2/matchers/catch_matchers_floating_point.hpp"
#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cstdlib>
#include <ratio>
#include <tlib/control/concepts/holdable.hpp>
#include <tlib/control/concepts/timestamped.hpp>
#include <tlib/control/filters/nthhold.hpp>
#include <tlib/control/spatial.hpp>

template <std::size_t N> void test_nth_order_hold() {
  using namespace Catch::Matchers;
  NthOrderHold<Wrench, N> hold;

  std::array<double, N + 1> constants;
  for (std::size_t i = 0; i < N + 1; i++) {
    constants[i] = ((rand() % 100'000 / 100'000.0) - 0.5) * 100.0;
  }
  std::array<Wrench, N + 2> sequence;
  for (std::size_t j = 0; j < N + 2; j++) {
    Timestamp t = std::chrono::steady_clock::time_point(
        std::chrono::milliseconds(10 * j));
    double value = 0.0;
    for (std::size_t i = 0; i < N + 1; i++) {
      value += std::pow(to_seconds(t.time_since_epoch()), i) * constants[i];
    }
    sequence[j] = Wrench{value, t};
  }

  /* Input up to and extrapolate last element*/
  for (std::size_t i = 0; i < sequence.size() - 1; i++) {
    hold(sequence[i]);
  }

  const Wrench &last = sequence.back();
  Wrench prediction = hold(last.stamp());
  for (std::size_t i = 0; i < 6; i++) {
    CHECK_THAT(prediction.vec()[i], WithinRel(last.vec()[i], 1e-6));
  }
}

TEST_CASE("Extrapolation", "[Nthhold]") {
  SECTION("1 to 32 Order Polynomial Guessing") {
    auto val = []<std::size_t... Idx>(std::index_sequence<Idx...>) {
      (test_nth_order_hold<Idx>(), ...);
      return 0.0;
    }(std::make_index_sequence<32>());
  }
}
