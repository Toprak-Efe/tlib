#include <array>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <ranges>
#include <span>
#include <vector>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot3d.h>

#include <tlib/control/spatial.hpp>

using TwistDisplacement = CompositeSignal<Twist, Displacement>;
using WrenchDisplacement = CompositeSignal<Wrench, Displacement>;

std::vector<std::byte>
get_latest_telemetry_bytes_from(const std::filesystem::path &folder) {
  namespace fs = std::filesystem;
  fs::path latest_file;
  for (const auto &entry : fs::directory_iterator(folder)) {
    if (!entry.is_regular_file())
      continue;
    if (latest_file.empty() ||
        entry.last_write_time() > fs::last_write_time(latest_file))
      latest_file = entry.path();
  }
  auto size = fs::file_size(latest_file);
  std::vector<std::byte> out(size);
  std::ifstream f(latest_file, std::ios::binary);
  f.read(reinterpret_cast<char *>(out.data()), size);
  return out;
}

template <typename T>
std::vector<T> reconstruct_series_from(const std::vector<std::byte> &bytes) {
  const size_t count = bytes.size() / T::CanonicalSize;
  std::vector<T> reconstruction;
  reconstruction.reserve(count);
  std::span all(bytes);
  for (size_t i = 0; i < count; i++) {
    T signal;
    T::serial_load(all.subspan(i * T::CanonicalSize, T::CanonicalSize), signal);
    reconstruction.emplace_back(signal);
  }
  return reconstruction;
}

template <>
std::vector<std::array<double, 16>>
reconstruct_series_from(const std::vector<std::byte> &bytes) {
  using TMat = std::array<double, 16>;
  const size_t count = bytes.size() / sizeof(TMat);
  std::vector<TMat> reconstruction;
  reconstruction.reserve(count);
  std::span all(bytes);
  for (size_t i = 0; i < count; i++) {
    TMat signal;
    std::memcpy(signal.data(),
                all.subspan(i * sizeof(TMat), sizeof(TMat)).data(),
                sizeof(TMat));
    reconstruction.emplace_back(signal);
  }
  return reconstruction;
}

template <typename T>
std::vector<T>
get_telemetry_series_from(const std::filesystem::path &telemetry_file) {
  return reconstruct_series_from<T>(
      get_latest_telemetry_bytes_from(telemetry_file));
}

// ── Main ───────────────────────────────────────────────────────────

int main() {
  namespace fs = std::filesystem;

  // Load data
  const fs::path signals_base_folder =
      "/home/toprak/Projects/Active/asclepius/notebook/data";
  auto transforms = get_telemetry_series_from<std::array<double, 16>>(
      signals_base_folder / "transform");

  auto to_vec = [&](int idx) {
    return transforms |
           std::views::transform([idx](const auto &m) { return m[idx]; }) |
           std::ranges::to<std::vector<double>>();
  };
  auto xs = to_vec(13), ys = to_vec(14), zs = to_vec(15);
  const int n = static_cast<int>(xs.size());

  // GLFW + OpenGL init
  if (!glfwInit())
    return 1;
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow *window =
      glfwCreateWindow(1600, 900, "Telemetry Viewer", nullptr, nullptr);
  if (!window)
    return 1;
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  // ImGui + ImPlot3D init
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot3D::CreateContext();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Telemetry", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

    if (ImPlot3D::BeginPlot("Trajectory", ImGui::GetContentRegionAvail())) {
      ImPlot3D::SetupAxes("X", "Y", "Z");
      ImPlot3D::PlotScatter("pos", xs.data(), ys.data(), zs.data(), n);
      ImPlot3D::PlotLine("path", xs.data(), ys.data(), zs.data(), n);
      ImPlot3D::EndPlot();
    }

    ImGui::End();
    ImGui::Render();
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
  }

  // Cleanup
  ImPlot3D::DestroyContext();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
