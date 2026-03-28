cmake_minimum_required(VERSION 4.0.0)
project(tlib)

find_package(Eigen3 REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(HEADERS
    #commons
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tlib/commons/concepts/logpolicy.hpp
    #concurrency
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tlib/concurrency/cache.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tlib/concurrency/flock.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tlib/concurrency/ringbuffer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tlib/concurrency/triplebuffer.hpp
    #control
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tlib/control/clamping.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tlib/control/nthhold.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tlib/control/physics.hpp
    ## concepts
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tlib/control/concepts/arithmetic.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tlib/control/concepts/vector.hpp
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
target_link_libraries(tlib PUBLIC Eigen3::Eigen)
target_compile_features(tlib PUBLIC cxx_std_23)

#add_executable(main)
#target_link_libraries(main PRIVATE tlib)
#target_compile_features(main PUBLIC cxx_std_23)
#target_sources(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp)
