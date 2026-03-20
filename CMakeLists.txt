cmake_minimum_required(VERSION 4.0.0)
project(snippets)

set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snippets/concepts/arithmetic.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snippets/concepts/container.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/snippets/concepts/timestamped.hpp
)
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/concepts.cpp
)

add_library(${PROJECT_NAME} STATIC)
target_include_directories(${PROJECT_NAME} PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_sources(${PROJECT_NAME}
    PUBLIC
    ${HEADERS}
    PRIVATE
    ${SOURCES}
)
