#include <catch2/catch_test_macros.hpp>
#include <tlib/control/signal.hpp>
#include <tlib/control/spatial.hpp>

TEST_CASE("SpatialVector", "[Serialization]") {
  SECTION("Constructor & Destructor") {
    {
      SignalPort<Wrench, 0> force{"test/force1"};
    }
    SUCCEED("Telemetry constructor ran without runtime errors.");
  }

  SECTION("Push 10 Samples") {
    {
      SignalPort<Wrench, 0> force{"test/force2"};
      for (size_t i = 0; i < 10; i++) {
        double d = static_cast<double>(i);
        force.push(Wrench{std::array<double, 6>({d, d, d, d, d, d})});
      }
    }
    SUCCEED("Telemetry constructor ran without runtime errors.");
  }
}

TEST_CASE("CompositeSignal", "[Serialization]") {
  using Composite = CompositeSignal<Wrench, Displacement>;
  SECTION("Constructor & Destructor") {
    {
      SignalPort<Composite> force_displacement{"test/force_diplacement1"};
    }
    SUCCEED("Telemetry constructor ran without runtime errors.");
  }

  SECTION("Push 10 Samples") {
    {
      SignalPort<Composite> force_displacement{"test/force_diplacement2"};
      for (size_t i = 0; i < 10; i++) {
        double d = static_cast<double>(i);
        std::array<double, 6> v{{d, d, d, d, d, d}};
        force_displacement.push(Composite{Wrench{v}, Displacement{v}});
      }
    }
    SUCCEED("Telemetry constructor ran without runtime errors.");
  }
}
