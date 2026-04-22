cmake_minimum_required(VERSION 4.0.0)
project(tlib)

find_package(Eigen3 REQUIRED)

add_library(tlib INTERFACE)
target_include_directories(tlib INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(tlib INTERFACE
    Eigen3::Eigen
)
target_compile_features(tlib INTERFACE
    cxx_std_23
)

if(TESTS_ON)
    add_library(tlib_compile_test STATIC source/tlib.cpp)
    target_link_libraries(tlib_compile_test PRIVATE tlib)
    add_subdirectory(tests)
endif()

