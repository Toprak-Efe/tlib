#include <catch2/catch_test_macros.hpp>
#include <stdexcept>
#include <tlib/concurrency/flock.hpp>

TEST_CASE("Locks and Unlocks.", "[flock]") {
  std::string resource_name = "oxDhqocsFnqTCmXH";

  SECTION("Acquire & Release Lock") {
    {
      FileLock<std::string> flock1{"tlib_test", resource_name};
    }
    try {
      FileLock<std::string> flock2{"tlib_test", resource_name};
    } catch (std::runtime_error &e) {
      FAIL(std::string("Unable to reacquire lock! ") + e.what());
    }
    SUCCEED("Acquired & Released Lock!");
  }

  SECTION("Fail To Acquire Twice") {
    FileLock<std::string> flock1{"tlib_test", resource_name};
    REQUIRE_THROWS_AS((FileLock<std::string>{"tlib_test", resource_name}),
                      std::runtime_error);
  }
}
