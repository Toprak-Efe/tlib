#include "matplot/core/figure_registry.h"
#include <filesystem>
#include <fstream>
#include <matplot/matplot.h>
#include <ranges>
#include <span>
#include <tlib/control/biquad.hpp>
#include <tlib/control/calculus.hpp>
#include <tlib/control/signal.hpp>
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
        entry.last_write_time() > fs::last_write_time(latest_file)) {
      latest_file = entry.path();
    }
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

template <typename T>
std::vector<T>
get_telemetry_series_from(const std::filesystem::path &telemetry_file) {
  std::vector<std::byte> bytes =
      get_latest_telemetry_bytes_from(telemetry_file);
  std::vector<T> telemetry_series = reconstruct_series_from<T>(bytes);
  return telemetry_series;
}

template <typename T>
std::vector<double>
get_pitch_series_from(const std::vector<SpatialVector<T>> &source) {
  std::vector<double> sink;
  sink.reserve(source.size());
  for (const SpatialVector<T> &d : source) {
    sink.push_back(d.angular().z());
  }
  return sink;
}

template <typename T>
std::vector<double>
get_yaw_series_from(const std::vector<SpatialVector<T>> &source) {
  std::vector<double> sink;
  sink.reserve(source.size());
  for (const SpatialVector<T> &d : source) {
    sink.push_back(d.angular().y());
  }
  return sink;
}

template <typename T>
std::vector<double>
get_roll_series_from(const std::vector<SpatialVector<T>> &source) {
  std::vector<double> sink;
  sink.reserve(source.size());
  for (const SpatialVector<T> &d : source) {
    sink.push_back(d.angular().x());
  }
  return sink;
}

template <typename T>
std::vector<double>
get_x_series_from(const std::vector<SpatialVector<T>> &source) {
  std::vector<double> sink;
  sink.reserve(source.size());
  for (const SpatialVector<T> &d : source) {
    sink.push_back(d.linear().x());
  }
  return sink;
}

template <typename T>
std::vector<double>
get_y_series_from(const std::vector<SpatialVector<T>> &source) {
  std::vector<double> sink;
  sink.reserve(source.size());
  for (const SpatialVector<T> &d : source) {
    sink.push_back(d.linear().y());
  }
  return sink;
}

template <typename T>
std::vector<double>
get_z_series_from(const std::vector<SpatialVector<T>> &source) {
  std::vector<double> sink;
  sink.reserve(source.size());
  for (const SpatialVector<T> &d : source) {
    sink.push_back(d.linear().z());
  }
  return sink;
}
int main(int argc, char *argv[]) {
  namespace fs = std::filesystem;
  const static fs::path signals_base_folder =
      "/home/toprak/Projects/Active/asclepius/notebook/data";
  const fs::path fm_folder(signals_base_folder / "Fm");
  const fs::path vm_folder(signals_base_folder / "Vm");

  std::vector<double> t{};
  std::vector<Twist> twists{};
  std::vector<Twist> twists_calculated{};
  std::vector<Displacement> displacements{};
  std::vector<Displacement> displacements_reconstructed{};

  {
    std::vector<TwistDisplacement> sensor_series =
        get_telemetry_series_from<TwistDisplacement>(vm_folder);
    displacements =
        sensor_series |
        std::ranges::views::transform(
            [](const TwistDisplacement &td) { return td.get<1>(); }) |
        std::ranges::to<std::vector<Displacement>>();
    twists = sensor_series |
             std::ranges::views::transform(
                 [](const TwistDisplacement &td) { return td.get<0>(); }) |
             std::ranges::to<std::vector<Twist>>();
    t = displacements |
        std::ranges::views::transform([&](const Displacement &d) {
          const double t_s = std::chrono::duration<double>(
                                 displacements[0].stamp().time_since_epoch())
                                 .count();
          return std::chrono::duration<double>(d.stamp().time_since_epoch())
                     .count() -
                 t_s;
        }) |
        std::ranges::to<std::vector<double>>();

    Differentiator<Displacement> differentiator(0.0053);
    Biquad<Displacement> low_pass(0.00782021, 0.01564042, 0.00782021,
                                  -1.73472577, 0.7660066);
    DecayingIntegrator<Twist> integrator{0.5};

    twists_calculated.reserve(t.size());
    displacements_reconstructed.reserve(t.size());
    for (const auto &d : displacements) {
      Displacement d1 = differentiator(d);
      Displacement d2 = low_pass(d1);
      Twist t1 = static_cast<Twist>(d2);
      twists_calculated.push_back(t1);
      Twist t2 = integrator(t1);
      displacements_reconstructed.push_back(static_cast<Displacement>(t2));
    }
  }

  std::vector<double> y1 = get_x_series_from(twists_calculated);
  std::vector<double> y2 = get_x_series_from(twists);

  auto f = matplot::figure(false);
  f->backend()->run_command("unset warnings");
  f->ioff();
  f->size(7680, 4320);
  matplot::plot(t, y1, t, y2);
  matplot::save("plot.svg");

  {
    SignalPort<TwistDisplacement> test{"/test/twist_displacement"};
    for (size_t i = 0; i < twists_calculated.size(); i++) {
      auto twist = twists_calculated[i];
      auto displacement = displacements[i];
      test.push(TwistDisplacement(twist, displacement));
    }
  }

  return 0;
}
