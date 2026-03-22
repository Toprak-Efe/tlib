cmake_minimum_required(VERSION 4.0.0)
project(snippets)

find_package(Eigen3 REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(HEADERS
    # Concepts & Data Standards
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snippets/concepts/arithmetic.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snippets/concepts/timestamped.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snippets/concepts/vector.hpp
    # Dynamics & Control
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snippets/control/nthhold.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snippets/control/physics.hpp
    # Memory
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snippets/memory/cache.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snippets/memory/ringbuffer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snippets/memory/triplebuffer.hpp
)
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/concepts.cpp
)

add_library(snippets STATIC)
target_include_directories(snippets PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_sources(snippets
    PUBLIC
    ${HEADERS}
    PRIVATE
    ${SOURCES}
)
target_link_libraries(snippets PUBLIC
    Eigen3::Eigen
)
target_compile_features(snippets PUBLIC
    cxx_std_23
)
