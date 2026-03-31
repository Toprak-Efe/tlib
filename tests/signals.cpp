#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <tlib/control/concepts/arithmetic.hpp>
#include <tlib/control/concepts/timestamped.hpp>
#include <tlib/control/concepts/vector.hpp>
#include <tlib/control/signal.hpp>
#include <tlib/control/spatial.hpp>

using Catch::Matchers::WithinAbs;

// ============================================================================
// Helpers
// ============================================================================

struct NullTag {};
using Vec = SpatialVector<NullTag>;

constexpr double kEps = 1e-12;

static Vec make_vec(double a, double b, double c, double d, double e,
                    double f, Timestamp ts = {}) {
  return Vec{std::array<double, 6>{a, b, c, d, e, f}, ts};
}

static Timestamp now() { return std::chrono::steady_clock::now(); }

// A type that deliberately fails every concept — used as a negative witness.
struct NotArithmetic {
  int x;
};

// ============================================================================
// Concept satisfaction (compile-time)
// ============================================================================

TEST_CASE("SpatialVector satisfies Vector concept", "[concepts]") {
  // SelfArithmetic
  static_assert(SelfArithmetic<Vec>);
  static_assert(SelfArithmetic<Wrench>);
  static_assert(SelfArithmetic<Twist>);
  static_assert(SelfArithmetic<Displacement>);

  // ScalarArithmetic
  static_assert(ScalarArithmetic<Vec>);
  static_assert(ScalarArithmetic<Wrench>);
  static_assert(ScalarArithmetic<Twist>);
  static_assert(ScalarArithmetic<Displacement>);

  // Combined Vector concept
  static_assert(Vector<Vec>);
  static_assert(Vector<Wrench>);
  static_assert(Vector<Twist>);
  static_assert(Vector<Displacement>);

  SUCCEED("All static assertions passed");
}

TEST_CASE("SpatialVector satisfies Timestamped concept", "[concepts]") {
  static_assert(Timestamped<Vec>);
  static_assert(Timestamped<Wrench>);
  static_assert(Timestamped<Twist>);
  static_assert(Timestamped<Displacement>);

  SUCCEED("All static assertions passed");
}

TEST_CASE("Negative concept witnesses", "[concepts]") {
  static_assert(!SelfArithmetic<NotArithmetic>);
  static_assert(!ScalarArithmetic<NotArithmetic>);
  static_assert(!Vector<NotArithmetic>);
  static_assert(!Timestamped<NotArithmetic>);
  static_assert(!Timestamped<double>);

  SUCCEED("Non-conforming types correctly rejected");
}

// ============================================================================
// SpatialVector — construction
// ============================================================================

TEST_CASE("SpatialVector default constructs to zero", "[spatial]") {
  Vec v;
  for (int i = 0; i < 6; ++i) {
    REQUIRE(v.vec()[i] == 0.0);
  }
}

TEST_CASE("SpatialVector constructs from array", "[spatial]") {
  auto v = make_vec(1, 2, 3, 4, 5, 6);
  REQUIRE_THAT(v.vec()[0], WithinAbs(1.0, kEps));
  REQUIRE_THAT(v.vec()[3], WithinAbs(4.0, kEps));
}

TEST_CASE("SpatialVector constructs from linear + angular", "[spatial]") {
  Vec::Vector3 lin{1, 2, 3};
  Vec::Vector3 ang{4, 5, 6};
  Vec v{lin, ang};
  REQUIRE_THAT(v.linear()[0], WithinAbs(1.0, kEps));
  REQUIRE_THAT(v.angular()[2], WithinAbs(6.0, kEps));
}

TEST_CASE("SpatialVector copy / move", "[spatial]") {
  auto a = make_vec(1, 2, 3, 4, 5, 6);

  SECTION("Copy preserves data") {
    Vec b{a};
    for (int i = 0; i < 6; ++i) {
      REQUIRE(b.vec()[i] == a.vec()[i]);
    }
  }
  SECTION("Move zeroes source") {
    Vec b{std::move(a)};
    REQUIRE_THAT(b.vec()[0], WithinAbs(1.0, kEps));
    for (int i = 0; i < 6; ++i) {
      REQUIRE(a.vec()[i] == 0.0);
    }
  }
}

// ============================================================================
// SpatialVector — self arithmetic
// ============================================================================

TEST_CASE("SpatialVector self-addition", "[spatial][arithmetic]") {
  auto a = make_vec(1, 2, 3, 4, 5, 6);
  auto b = make_vec(10, 20, 30, 40, 50, 60);

  auto c = a + b;
  REQUIRE_THAT(c.vec()[0], WithinAbs(11.0, kEps));
  REQUIRE_THAT(c.vec()[5], WithinAbs(66.0, kEps));
}

TEST_CASE("SpatialVector self-subtraction", "[spatial][arithmetic]") {
  auto a = make_vec(10, 20, 30, 40, 50, 60);
  auto b = make_vec(1, 2, 3, 4, 5, 6);

  auto c = a - b;
  REQUIRE_THAT(c.vec()[0], WithinAbs(9.0, kEps));
  REQUIRE_THAT(c.vec()[5], WithinAbs(54.0, kEps));
}

TEST_CASE("SpatialVector element-wise multiplication", "[spatial][arithmetic]") {
  auto a = make_vec(1, 2, 3, 4, 5, 6);
  auto b = make_vec(2, 3, 4, 5, 6, 7);

  auto c = a * b;
  REQUIRE_THAT(c.vec()[0], WithinAbs(2.0, kEps));
  REQUIRE_THAT(c.vec()[5], WithinAbs(42.0, kEps));
}

TEST_CASE("SpatialVector element-wise division", "[spatial][arithmetic]") {
  auto a = make_vec(10, 20, 30, 40, 50, 60);
  auto b = make_vec(2, 4, 5, 8, 10, 12);

  auto c = a / b;
  REQUIRE_THAT(c.vec()[0], WithinAbs(5.0, kEps));
  REQUIRE_THAT(c.vec()[5], WithinAbs(5.0, kEps));
}

// ============================================================================
// SpatialVector — in-place self arithmetic
// ============================================================================

TEST_CASE("SpatialVector in-place +=", "[spatial][arithmetic]") {
  auto a = make_vec(1, 2, 3, 4, 5, 6);
  auto b = make_vec(10, 20, 30, 40, 50, 60);
  a += b;
  REQUIRE_THAT(a.vec()[0], WithinAbs(11.0, kEps));
  REQUIRE_THAT(a.vec()[5], WithinAbs(66.0, kEps));
}

TEST_CASE("SpatialVector in-place -=", "[spatial][arithmetic]") {
  auto a = make_vec(10, 20, 30, 40, 50, 60);
  auto b = make_vec(1, 2, 3, 4, 5, 6);
  a -= b;
  REQUIRE_THAT(a.vec()[0], WithinAbs(9.0, kEps));
}

TEST_CASE("SpatialVector in-place *=", "[spatial][arithmetic]") {
  auto a = make_vec(2, 3, 4, 5, 6, 7);
  auto b = make_vec(3, 4, 5, 6, 7, 8);
  a *= b;
  REQUIRE_THAT(a.vec()[0], WithinAbs(6.0, kEps));
  REQUIRE_THAT(a.vec()[5], WithinAbs(56.0, kEps));
}

TEST_CASE("SpatialVector in-place /=", "[spatial][arithmetic]") {
  auto a = make_vec(10, 20, 30, 40, 50, 60);
  auto b = make_vec(2, 4, 5, 8, 10, 12);
  a /= b;
  REQUIRE_THAT(a.vec()[0], WithinAbs(5.0, kEps));
}

// ============================================================================
// SpatialVector — scalar arithmetic
// ============================================================================

TEST_CASE("SpatialVector scalar addition", "[spatial][arithmetic]") {
  auto a = make_vec(1, 2, 3, 4, 5, 6);
  auto c = a + 10.0;
  for (int i = 0; i < 6; ++i) {
    REQUIRE_THAT(c.vec()[i], WithinAbs(a.vec()[i] + 10.0, kEps));
  }
}

TEST_CASE("SpatialVector scalar subtraction", "[spatial][arithmetic]") {
  auto a = make_vec(10, 20, 30, 40, 50, 60);
  auto c = a - 5.0;
  for (int i = 0; i < 6; ++i) {
    REQUIRE_THAT(c.vec()[i], WithinAbs(a.vec()[i] - 5.0, kEps));
  }
}

TEST_CASE("SpatialVector scalar multiplication", "[spatial][arithmetic]") {
  auto a = make_vec(1, 2, 3, 4, 5, 6);
  auto c = a * 3.0;
  REQUIRE_THAT(c.vec()[0], WithinAbs(3.0, kEps));
  REQUIRE_THAT(c.vec()[5], WithinAbs(18.0, kEps));

  // Commutative friend operator
  auto d = 3.0 * a;
  for (int i = 0; i < 6; ++i) {
    REQUIRE_THAT(d.vec()[i], WithinAbs(c.vec()[i], kEps));
  }
}

TEST_CASE("SpatialVector scalar division", "[spatial][arithmetic]") {
  auto a = make_vec(10, 20, 30, 40, 50, 60);
  auto c = a / 10.0;
  REQUIRE_THAT(c.vec()[0], WithinAbs(1.0, kEps));
  REQUIRE_THAT(c.vec()[5], WithinAbs(6.0, kEps));
}

// ============================================================================
// SpatialVector — in-place scalar arithmetic
// ============================================================================

TEST_CASE("SpatialVector in-place scalar +=", "[spatial][arithmetic]") {
  auto a = make_vec(1, 2, 3, 4, 5, 6);
  a += 10.0;
  REQUIRE_THAT(a.vec()[0], WithinAbs(11.0, kEps));
  REQUIRE_THAT(a.vec()[5], WithinAbs(16.0, kEps));
}

TEST_CASE("SpatialVector in-place scalar -=", "[spatial][arithmetic]") {
  auto a = make_vec(10, 20, 30, 40, 50, 60);
  a -= 5.0;
  REQUIRE_THAT(a.vec()[0], WithinAbs(5.0, kEps));
}

TEST_CASE("SpatialVector in-place scalar *=", "[spatial][arithmetic]") {
  auto a = make_vec(1, 2, 3, 4, 5, 6);
  a *= 3.0;
  REQUIRE_THAT(a.vec()[0], WithinAbs(3.0, kEps));
  REQUIRE_THAT(a.vec()[5], WithinAbs(18.0, kEps));
}

TEST_CASE("SpatialVector in-place scalar /=", "[spatial][arithmetic]") {
  auto a = make_vec(10, 20, 30, 40, 50, 60);
  a /= 10.0;
  REQUIRE_THAT(a.vec()[0], WithinAbs(1.0, kEps));
  REQUIRE_THAT(a.vec()[5], WithinAbs(6.0, kEps));
}

// ============================================================================
// SpatialVector — timestamp propagation
// ============================================================================

TEST_CASE("Binary ops propagate latest timestamp", "[spatial][timestamp]") {
  auto t1 = now();
  auto t2 = t1 + std::chrono::milliseconds(100);

  auto a = make_vec(1, 0, 0, 0, 0, 0, t1);
  auto b = make_vec(2, 0, 0, 0, 0, 0, t2);

  SECTION("a + b takes max(t1, t2)") {
    auto c = a + b;
    REQUIRE(c.stamp() == t2);
  }
  SECTION("b + a takes max(t2, t1)") {
    auto c = b + a;
    REQUIRE(c.stamp() == t2);
  }
  SECTION("a - b") {
    REQUIRE((a - b).stamp() == t2);
  }
  SECTION("a * b") {
    REQUIRE((a * b).stamp() == t2);
  }
  SECTION("a / b (b nonzero)") {
    auto b2 = make_vec(1, 1, 1, 1, 1, 1, t2);
    REQUIRE((a / b2).stamp() == t2);
  }
}

TEST_CASE("In-place ops propagate latest timestamp", "[spatial][timestamp]") {
  auto t1 = now();
  auto t2 = t1 + std::chrono::milliseconds(100);

  auto a = make_vec(1, 2, 3, 4, 5, 6, t1);
  auto b = make_vec(1, 1, 1, 1, 1, 1, t2);

  SECTION("+=") {
    a += b;
    REQUIRE(a.stamp() == t2);
  }
  SECTION("-=") {
    a -= b;
    REQUIRE(a.stamp() == t2);
  }
  SECTION("*=") {
    a *= b;
    REQUIRE(a.stamp() == t2);
  }
  SECTION("/=") {
    a /= b;
    REQUIRE(a.stamp() == t2);
  }
}

TEST_CASE("Scalar ops preserve original timestamp", "[spatial][timestamp]") {
  auto t1 = now();
  auto a = make_vec(1, 2, 3, 4, 5, 6, t1);

  REQUIRE((a + 1.0).stamp() == t1);
  REQUIRE((a - 1.0).stamp() == t1);
  REQUIRE((a * 2.0).stamp() == t1);
  REQUIRE((a / 2.0).stamp() == t1);
}

// ============================================================================
// SpatialVector — linear / angular accessors
// ============================================================================

TEST_CASE("linear() and angular() map correctly", "[spatial]") {
  auto v = make_vec(1, 2, 3, 4, 5, 6);
  REQUIRE_THAT(v.linear()[0], WithinAbs(1.0, kEps));
  REQUIRE_THAT(v.linear()[2], WithinAbs(3.0, kEps));
  REQUIRE_THAT(v.angular()[0], WithinAbs(4.0, kEps));
  REQUIRE_THAT(v.angular()[2], WithinAbs(6.0, kEps));
}

TEST_CASE("Mutable linear()/angular() write through", "[spatial]") {
  Vec v;
  v.linear()[0] = 42.0;
  v.angular()[2] = 99.0;
  REQUIRE_THAT(v.vec()[0], WithinAbs(42.0, kEps));
  REQUIRE_THAT(v.vec()[5], WithinAbs(99.0, kEps));
}

// ============================================================================
// SpatialOperator
// ============================================================================

TEST_CASE("SpatialOperator identity preserves vector", "[spatial][operator]") {
  Impedance Z; // default = I₆
  Twist tw{std::array<double, 6>{1, 2, 3, 4, 5, 6}};

  Wrench w = Z * tw;
  for (int i = 0; i < 6; ++i) {
    REQUIRE_THAT(w.vec()[i], WithinAbs(tw.vec()[i], kEps));
  }
}

TEST_CASE("SpatialOperator scales vector", "[spatial][operator]") {
  Eigen::Matrix<double, 6, 6> mat = Eigen::Matrix<double, 6, 6>::Identity() * 3.0;
  Stiffness K{mat};
  Displacement d{std::array<double, 6>{1, 1, 1, 1, 1, 1}};

  Wrench w = K * d;
  for (int i = 0; i < 6; ++i) {
    REQUIRE_THAT(w.vec()[i], WithinAbs(3.0, kEps));
  }
}

TEST_CASE("SpatialOperator general matrix multiply", "[spatial][operator]") {
  Eigen::Matrix<double, 6, 6> mat;
  mat.setZero();
  // Swap linear and angular: put I₃ in off-diagonal blocks
  mat.block<3, 3>(0, 3) = Eigen::Matrix3d::Identity();
  mat.block<3, 3>(3, 0) = Eigen::Matrix3d::Identity();

  Adjoint adj{mat};
  Twist tw{std::array<double, 6>{1, 2, 3, 10, 20, 30}};
  Twist out = adj * tw;

  // linear of output should be angular of input, and vice versa
  REQUIRE_THAT(out.linear()[0], WithinAbs(10.0, kEps));
  REQUIRE_THAT(out.angular()[0], WithinAbs(1.0, kEps));
}

// ============================================================================
// CompositeSignal — concept satisfaction
// ============================================================================

TEST_CASE("CompositeSignal satisfies Timestamped", "[composite][concepts]") {
  using Composite = CompositeSignal<Wrench, Twist>;
  static_assert(Timestamped<Composite>);
  SUCCEED();
}

// ============================================================================
// CompositeSignal — stamp() returns max of children
// ============================================================================

TEST_CASE("CompositeSignal::stamp() returns latest child stamp",
          "[composite][timestamp]") {
  auto t1 = now();
  auto t2 = t1 + std::chrono::milliseconds(50);

  Wrench w{std::array<double, 6>{0, 0, 0, 0, 0, 0}, t1};
  Twist tw{std::array<double, 6>{0, 0, 0, 0, 0, 0}, t2};

  CompositeSignal<Wrench, Twist> cs{w, tw};
  REQUIRE(cs.stamp() == t2);
}

// ============================================================================
// CompositeSignal — get<I>() accessors
// ============================================================================

TEST_CASE("CompositeSignal::get<I>() accesses correct signal",
          "[composite]") {
  Wrench w{std::array<double, 6>{1, 2, 3, 4, 5, 6}};
  Twist tw{std::array<double, 6>{10, 20, 30, 40, 50, 60}};

  CompositeSignal<Wrench, Twist> cs{w, tw};

  REQUIRE_THAT(cs.get<0>().vec()[0], WithinAbs(1.0, kEps));
  REQUIRE_THAT(cs.get<1>().vec()[0], WithinAbs(10.0, kEps));
}

TEST_CASE("CompositeSignal::get<I>() is mutable", "[composite]") {
  CompositeSignal<Wrench, Twist> cs;
  cs.get<0>().vec()[0] = 42.0;
  REQUIRE_THAT(cs.get<0>().vec()[0], WithinAbs(42.0, kEps));
}

// ============================================================================
// CompositeSignal — in-place arithmetic (these use inplace(), which is correct)
// ============================================================================

TEST_CASE("CompositeSignal in-place += works", "[composite][arithmetic]") {
  Wrench w1{std::array<double, 6>{1, 2, 3, 4, 5, 6}};
  Twist t1{std::array<double, 6>{10, 20, 30, 40, 50, 60}};

  Wrench w2{std::array<double, 6>{1, 1, 1, 1, 1, 1}};
  Twist t2{std::array<double, 6>{2, 2, 2, 2, 2, 2}};

  CompositeSignal<Wrench, Twist> a{w1, t1};
  CompositeSignal<Wrench, Twist> b{w2, t2};

  a += b;

  REQUIRE_THAT(a.get<0>().vec()[0], WithinAbs(2.0, kEps));
  REQUIRE_THAT(a.get<0>().vec()[5], WithinAbs(7.0, kEps));
  REQUIRE_THAT(a.get<1>().vec()[0], WithinAbs(12.0, kEps));
  REQUIRE_THAT(a.get<1>().vec()[5], WithinAbs(62.0, kEps));
}

TEST_CASE("CompositeSignal in-place -= works", "[composite][arithmetic]") {
  Wrench w1{std::array<double, 6>{10, 20, 30, 40, 50, 60}};
  Twist t1{std::array<double, 6>{100, 200, 300, 400, 500, 600}};

  Wrench w2{std::array<double, 6>{1, 2, 3, 4, 5, 6}};
  Twist t2{std::array<double, 6>{10, 20, 30, 40, 50, 60}};

  CompositeSignal<Wrench, Twist> a{w1, t1};
  CompositeSignal<Wrench, Twist> b{w2, t2};

  a -= b;

  REQUIRE_THAT(a.get<0>().vec()[0], WithinAbs(9.0, kEps));
  REQUIRE_THAT(a.get<1>().vec()[0], WithinAbs(90.0, kEps));
}

// ============================================================================
// CompositeSignal — scalar in-place arithmetic
// ============================================================================

TEST_CASE("CompositeSignal scalar in-place += works",
          "[composite][arithmetic]") {
  Wrench w{std::array<double, 6>{1, 2, 3, 4, 5, 6}};
  Twist t{std::array<double, 6>{10, 20, 30, 40, 50, 60}};

  CompositeSignal<Wrench, Twist> cs{w, t};
  cs += 5.0;

  REQUIRE_THAT(cs.get<0>().vec()[0], WithinAbs(6.0, kEps));
  REQUIRE_THAT(cs.get<1>().vec()[0], WithinAbs(15.0, kEps));
}

TEST_CASE("CompositeSignal scalar in-place *= works",
          "[composite][arithmetic]") {
  Wrench w{std::array<double, 6>{1, 2, 3, 4, 5, 6}};
  Twist t{std::array<double, 6>{10, 20, 30, 40, 50, 60}};

  CompositeSignal<Wrench, Twist> cs{w, t};
  cs *= 2.0;

  REQUIRE_THAT(cs.get<0>().vec()[0], WithinAbs(2.0, kEps));
  REQUIRE_THAT(cs.get<1>().vec()[5], WithinAbs(120.0, kEps));
}
