include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        v1.91.8
)

FetchContent_Declare(
  implot3d
  GIT_REPOSITORY https://github.com/brenocq/implot3d.git
  GIT_TAG        v0.2
)
FetchContent_Declare(
    matplot 
    GIT_REPOSITORY https://github.com/alandefreitas/matplotplusplus
    GIT_TAG v1.2.2
)

FetchContent_MakeAvailable(imgui implot3d matplot)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# imgui core + GLFW/OpenGL3 backends
add_library(imgui STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

# implot3d
add_library(implot3d STATIC
  ${implot3d_SOURCE_DIR}/implot3d.cpp
  ${implot3d_SOURCE_DIR}/implot3d_items.cpp
  ${implot3d_SOURCE_DIR}/implot3d_meshes.cpp
)
target_include_directories(implot3d PUBLIC ${implot3d_SOURCE_DIR})
target_link_libraries(implot3d PUBLIC imgui)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE
    tlib
    imgui
    implot3d
    Eigen3::Eigen
)
add_executable(twist_test twist_test.cpp)
target_link_libraries(twist_test PRIVATE
    tlib
    matplot
    Eigen3::Eigen
)
