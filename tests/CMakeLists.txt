include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.1
)
FetchContent_MakeAvailable(Catch2)
# catch2 module fix for FetchContent
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(tests)
target_sources(tests
 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/test_flock.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_clamping.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_devices.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_serialization.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_butterworth.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_nthhold.cpp
)
target_link_libraries(tests PRIVATE
    Catch2::Catch2WithMain
    tlib
)

include(CTest)
include(Catch)
catch_discover_tests(tests)
