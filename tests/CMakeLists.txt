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

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/flock_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/clamping_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/serialization_test.cpp
)

add_executable(tests)
target_sources(tests
 PRIVATE
 ${SOURCES}
)
target_link_libraries(tests PRIVATE
    Catch2::Catch2WithMain
    tlib
)

include(CTest)
include(Catch)
catch_discover_tests(tests)
