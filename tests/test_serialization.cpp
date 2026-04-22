#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <tlib/control/signal.hpp>
#include <tlib/control/spatial.hpp>

TEST_CASE("SpatialVector", "[Serialization]") {
  std::filesystem::remove_all(std::filesystem::temp_directory_path() /
                              "tlibtelemetry");

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
  SECTION("Push & Read Sample") {
    auto base = std::filesystem::temp_directory_path() / "tlibtelemetry" /
                "test" / "force3";
    std::array<double, 6> expected{{1.0, 2.0, 3.0, 4.0, 5.0, 6.0}};
    {
      SignalPort<Wrench, 0> force{"test/force3"};
      force.push(Wrench{expected});
    }

    auto it = std::filesystem::directory_iterator(base);
    REQUIRE(it != std::filesystem::directory_iterator{});
    auto file_path = it->path();

    std::ifstream ifs(file_path, std::ios::binary);
    REQUIRE(ifs.good());
    std::vector<std::byte> bytes(Wrench::CanonicalSize);
    ifs.read(reinterpret_cast<char *>(bytes.data()),
             static_cast<std::streamsize>(bytes.size()));
    REQUIRE(ifs.gcount() ==
            static_cast<std::streamsize>(Wrench::CanonicalSize));

    Wrench loaded;
    Wrench::serial_load(bytes, loaded);

    std::array<double, 6> actual;
    Eigen::Map<Eigen::Matrix<double, 6, 1>>(actual.data(), actual.size()) =
        loaded.vec();
    REQUIRE(actual == expected);
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
  SECTION("Push & Read Sample") {
    auto base = std::filesystem::temp_directory_path() / "tlibtelemetry" /
                "test" / "force_displacement3";
    std::array<double, 6> wv{{1.0, 2.0, 3.0, 4.0, 5.0, 6.0}};
    std::array<double, 6> dv{{7.0, 8.0, 9.0, 10.0, 11.0, 12.0}};
    {
      SignalPort<Composite> fd{"test/force_displacement3"};
      fd.push(Composite{Wrench{wv}, Displacement{dv}});
    }

    auto it = std::filesystem::directory_iterator(base);
    REQUIRE(it != std::filesystem::directory_iterator{});
    auto file_path = it->path();

    std::ifstream ifs(file_path, std::ios::binary);
    REQUIRE(ifs.good());
    std::vector<std::byte> bytes(Composite::CanonicalSize);
    ifs.read(reinterpret_cast<char *>(bytes.data()),
             static_cast<std::streamsize>(bytes.size()));
    REQUIRE(ifs.gcount() ==
            static_cast<std::streamsize>(Composite::CanonicalSize));

    Composite loaded;
    Composite::serial_load(bytes, loaded);

    std::array<double, 6> actual_w, actual_d;
    Eigen::Map<Eigen::Matrix<double, 6, 1>>(actual_w.data(), actual_w.size()) =
        loaded.get<0>().vec();
    Eigen::Map<Eigen::Matrix<double, 6, 1>>(actual_d.data(), actual_d.size()) =
        loaded.get<1>().vec();
    REQUIRE(actual_w == wv);
    REQUIRE(actual_d == dv);
  }
}
