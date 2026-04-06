cmake_minimum_required(VERSION 4.0.0)
project(tlib)

find_package(Eigen3 REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HEADERS
    #common
    ## concepts
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tlib/common/concepts/serializable.hpp
    #concurrency
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tlib/concurrency/cache.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tlib/concurrency/flock.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tlib/concurrency/ringbuffer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tlib/concurrency/triplebuffer.hpp
    #control
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tlib/control/clamping.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tlib/control/nthhold.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tlib/control/spatial.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tlib/control/biquad.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tlib/control/calculus.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tlib/control/signal.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tlib/control/telemetry.hpp
    ## concepts
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tlib/control/concepts/arithmetic.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tlib/control/concepts/vector.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tlib/control/concepts/holdable.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tlib/control/concepts/timestamped.hpp
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/concepts.cpp
)

add_library(tlib STATIC)
target_include_directories(tlib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_sources(tlib
    PUBLIC
    ${HEADERS}
    PRIVATE
    ${SOURCES}
)
target_link_libraries(tlib
PUBLIC
    Eigen3::Eigen
)

if(TESTS_ON)
    add_subdirectory(tests)
endif()

