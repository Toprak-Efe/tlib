#include <catch2/catch_test_macros.hpp>
#include <tlib/control/clamping.hpp>
#include <tlib/control/spatial.hpp>

struct NullTag {}; // struct NullTag
using Vec = SpatialVector<NullTag>;

TEST_CASE("Vectors.", "[clamping]") {
  Vec minimum{std::array<double, 6>{-5.0, -5.0, -5.0, -2.5, -2.5, -2.5}};
  Vec maximum{std::array<double, 6>{5.0, 5.0, 5.0, 2.5, 2.5, 2.5}};
  Clamper<Vec> clamper{minimum, maximum};

  SECTION("Clamps Maximal") {
    Vec vec{std::array<double, 6>{10.0, 10.0, 10.0, 5.0, 5.0, 5.0}};
    Vec out = clamper.clamp(vec);
    for (size_t i = 0; i < 6; i++) {
      REQUIRE(out.vec()[i] == maximum.vec()[i]);
    }
  }
  SECTION("Clamps Minimal") {
    Vec vec{std::array<double, 6>{-10.0, -10.0, -10.0, -5.0, -5.0, -5.0}};
    Vec out = clamper.clamp(vec);
    for (size_t i = 0; i < 6; i++) {
      REQUIRE(out.vec()[i] == minimum.vec()[i]);
    }
  }
  SECTION("Clamps Nothing") {
    Vec vec{std::array<double, 6>{0.0, 0.0, 0.0, 0.0, 0.0, 0.0}};
    Vec out = clamper.clamp(vec);
    for (size_t i = 0; i < 6; i++) {
      REQUIRE(out.vec()[i] == 0.0);
    }
  }
}

TEST_CASE("Doubles", "[clamping]") {
  double maximum{5.0};
  double minimum{-5.0};
  Clamper<double> clamper{minimum, maximum};

  SECTION("Clamps Maixmal") {
    double val{10.0};
    double out = clamper.clamp(val);
    REQUIRE(out == maximum);
  }
  SECTION("Clamps Minimal") {
    double val{-10.0};
    double out = clamper.clamp(val);
    REQUIRE(out == minimum);
  }
  SECTION("Clamps Nothing") {
    double val{0.0};
    double out = clamper.clamp(val);
    REQUIRE(out == val);
  }
}
