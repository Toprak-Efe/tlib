#pragma once

#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstring>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/src/Core/Map.h>
#include <eigen3/Eigen/src/Geometry/Quaternion.h>
#include <tlib/control/concepts/timestamped.hpp>

struct WrenchTag {};       // Force (F, t)
struct TwistTag {};        // Velocity (V, w)
struct DisplacementTag {}; // Position (P, theta)

template <typename T> class SpatialVector {
public:
  using Scalar = double;
  using Vector6 = Eigen::Matrix<Scalar, 6, 1>;
  using Vector3 = Eigen::Matrix<Scalar, 3, 1>;
  using Clock = std::chrono::steady_clock;
  using Timepoint = Clock::time_point;

  SpatialVector() : timestamp() { data.setZero(); };
  SpatialVector(SpatialVector &&vec)
      : data(std::move(vec.data)), timestamp(std::move(vec.timestamp)) {}
  SpatialVector(const SpatialVector &vec)
      : data(vec.data), timestamp(vec.timestamp) {}
  template <typename F>
        explicit SpatialVector(const SpatialVector<F>& oth) : data{oth.vec()}, timestamp{oth.stamp()}  {}
  SpatialVector &operator=(SpatialVector &&vec) {
    data = std::move(vec.data);
    timestamp = std::move(vec.timestamp);
    return *this;
  }
  SpatialVector &operator=(const SpatialVector &vec) {
    data = vec.data;
    timestamp = vec.timestamp;
    return *this;
  }
  explicit SpatialVector(const Vector6 &vec, const Timepoint &tsp = {})
      : data(vec), timestamp(tsp) {}
  SpatialVector(const Vector3 &linear, const Vector3 &angular,
                const Timepoint &tsp = {})
      : timestamp(tsp) {
    data << linear, angular;
  };
  explicit SpatialVector(const std::array<Scalar, 6> &arr,
                         const Timepoint &tsp = {})
      : timestamp(tsp) {
    data = Eigen::Map<const Vector6>(arr.data());
  }
  ~SpatialVector() = default;

  const Timestamp &stamp() const { return timestamp; }
  Timestamp &stamp() { return timestamp; }
  const Vector6 &vec() const { return data; }
  Vector6 &vec() { return data; }

  auto linear() const { return data.template head<3>(); }
  auto linear() { return data.template head<3>(); }
  auto angular() const { return data.template tail<3>(); }
  auto angular() { return data.template tail<3>(); }

  static constexpr size_t CanonicalSize =
      sizeof(std::array<double, 6>) +
      sizeof(decltype(Timestamp{}.time_since_epoch().count()));

  static void serial_save(std::vector<std::byte> &v, const SpatialVector &s) {
    std::array<double, 6> arr;
    Eigen::Map<Vector6>(arr.data(), arr.size()) = s.vec();
    const auto *v_ptr = reinterpret_cast<const std::byte *>(arr.data());
    v.insert(v.end(), v_ptr, v_ptr + sizeof(arr));

    auto ticks = s.timestamp.time_since_epoch().count();
    const auto *t_ptr = reinterpret_cast<const std::byte *>(&ticks);
    v.insert(v.end(), t_ptr, t_ptr + sizeof(ticks));
  }

  static void serial_load(std::span<const std::byte> v, SpatialVector &s) {
    assert(v.size() >= CanonicalSize);

    std::array<double, 6> arr;
    memcpy(arr.data(), v.data(), sizeof(arr));
    s.vec() = Eigen::Map<Vector6>(arr.data(), arr.size());

    decltype(s.timestamp.time_since_epoch().count()) ticks;
    memcpy(&ticks, v.data() + sizeof(arr), sizeof(ticks));
    decltype(s.timestamp.time_since_epoch()) dur{ticks};
    s.stamp() = Timestamp{dur};
  }

  /* Arithmetic Operators */
  SpatialVector operator+(Scalar s) const {
    return SpatialVector(data.array() + s, timestamp);
  }
  SpatialVector operator+(const SpatialVector &rhs) const {
    return SpatialVector(data + rhs.vec(), std::max(timestamp, rhs.stamp()));
  }
  SpatialVector operator-(Scalar s) const {
    return SpatialVector(data.array() - s, timestamp);
  }
  SpatialVector operator-(const SpatialVector &rhs) const {
    return SpatialVector(data - rhs.vec(), std::max(timestamp, rhs.stamp()));
  }
  SpatialVector operator*(Scalar s) const {
    return SpatialVector(data * s, timestamp);
  }
  friend SpatialVector operator*(Scalar s, const SpatialVector &v) {
    return v * s;
  }
  SpatialVector operator*(const SpatialVector &rhs) const {
    Vector6 v6_out;
    for (size_t i = 0; i < 6; i++) {
      v6_out[i] = data[i] * rhs.data[i];
    }
    return SpatialVector(v6_out, std::max(rhs.stamp(), timestamp));
  }
  SpatialVector operator/(Scalar s) const {
    return SpatialVector(data / s, timestamp);
  }
  SpatialVector operator/(const SpatialVector &rhs) const {
    Vector6 v6_out;
    for (size_t i = 0; i < 6; i++) {
      v6_out[i] = data[i] / rhs.data[i];
    }
    return SpatialVector(v6_out, std::max(rhs.stamp(), timestamp));
  }

  /* In-place Arithmetic Operators */
  SpatialVector &operator/=(Scalar s) {
    data.array() /= s;
    return *this;
  }
  SpatialVector &operator/=(const SpatialVector &v) {
    for (size_t i = 0; i < 6; i++) {
      data[i] /= v.vec()[i];
    }
    timestamp = std::max(timestamp, v.stamp());
    return *this;
  }
  SpatialVector &operator*=(Scalar s) {
    data.array() *= s;
    return *this;
  }
  SpatialVector &operator*=(const SpatialVector &v) {
    for (size_t i = 0; i < 6; i++) {
      data[i] *= v.vec()[i];
    }
    timestamp = std::max(timestamp, v.stamp());
    return *this;
  }
  SpatialVector &operator+=(Scalar s) {
    data.array() += s;
    return *this;
  }
  SpatialVector &operator+=(const SpatialVector &v) {
    data += v.vec();
    timestamp = std::max(timestamp, v.stamp());
    return *this;
  }
  SpatialVector &operator-=(Scalar s) {
    data.array() -= s;
    return *this;
  }
  SpatialVector &operator-=(const SpatialVector &v) {
    data -= v.vec();
    timestamp = std::max(timestamp, v.stamp());
    return *this;
  }

private:
  Vector6 data;
  Timepoint timestamp;
}; // class SpatialVector

using Wrench = SpatialVector<WrenchTag>;
using Twist = SpatialVector<TwistTag>;
using Displacement = SpatialVector<DisplacementTag>;

template <typename FromTag, typename ToTag> class SpatialOperator {
public:
  using Scalar = double;
  using Matrix6 = Eigen::Matrix<Scalar, 6, 6>;

  SpatialOperator() : mat_(Matrix6::Identity()) {}
  SpatialOperator(const Matrix6 &mat) : mat_{mat} {}
  SpatialOperator(const SpatialOperator &oth) : mat_(oth.mat_) {}
  SpatialOperator(SpatialOperator &&oth) : mat_(oth.mat_) {}

  SpatialVector<ToTag> operator*(const SpatialVector<FromTag> &v) const {
    return SpatialVector<ToTag>(mat_ * v.vec());
  }

private:
  Matrix6 mat_;

}; // class SpatialOperator

using Impedance = SpatialOperator<TwistTag, WrenchTag>;
using Admittance = SpatialOperator<WrenchTag, TwistTag>;
using Adjoint = SpatialOperator<TwistTag, TwistTag>;
using Coadjoint = SpatialOperator<WrenchTag, WrenchTag>;
using Stiffness = SpatialOperator<DisplacementTag, WrenchTag>;
using PositionGain = SpatialOperator<DisplacementTag, TwistTag>;

template <typename... Signals>
  requires((sizeof...(Signals) > 0) && (Timestamped<Signals> && ...))
class CompositeSignal {
public:
  using Clock = std::chrono::steady_clock;
  using Timepoint = Clock::time_point;
  static constexpr size_t SignalCount = sizeof...(Signals);
  static constexpr size_t CanonicalSize = (Signals::CanonicalSize + ...);

  CompositeSignal() = default;
  explicit CompositeSignal(const Signals &...args) : signals_{args...} {}
  explicit CompositeSignal(Signals &&...args) : signals_{std::move(args)...} {}

  Timestamp stamp() const {
    return std::apply([](const auto &...s) { return std::max({s.stamp()...}); },
                      signals_);
  }

  template <size_t I> auto &get() { return std::get<I>(signals_); }
  template <size_t I> const auto &get() const { return std::get<I>(signals_); }

  static void serial_save(std::vector<std::byte> &v, const CompositeSignal &s) {
    [&]<size_t... Is>(std::index_sequence<Is...>) {
      (std::tuple_element_t<Is, decltype(s.signals_)>::serial_save(
           v, std::get<Is>(s.signals_)),
       ...);
    }(Indices{});
  }

  static void serial_load(std::span<const std::byte> v, CompositeSignal &s) {
    size_t offset = 0;
    [&]<size_t... Is>(std::index_sequence<Is...>) {
      (([&] {
         using S = std::tuple_element_t<Is, decltype(signals_)>;
         auto span = std::span(v).subspan(offset);
         S::serial_load(span, std::get<Is>(s.signals_));
         offset += S::CanonicalSize;
       }()),
       ...);
    }(Indices{});
  }

  CompositeSignal operator-(const CompositeSignal &rhs) const {
    return binary([](const auto &a, const auto &b) { return a - b; }, rhs,
                  Indices{});
  }
  CompositeSignal operator+(const CompositeSignal &rhs) const {
    return binary([](const auto &a, const auto &b) { return a + b; }, rhs,
                  Indices{});
  }
  CompositeSignal operator*(const CompositeSignal &rhs) const {
    return binary([](const auto &a, const auto &b) { return a * b; }, rhs,
                  Indices{});
  }
  CompositeSignal operator/(const CompositeSignal &rhs) const {
    return binary([](const auto &a, const auto &b) { return a / b; }, rhs,
                  Indices{});
  }

  CompositeSignal &operator+=(const CompositeSignal &rhs) {
    return inplace([&](auto &a, const auto &b) { a += b; }, rhs, Indices{});
  }
  CompositeSignal &operator-=(const CompositeSignal &rhs) {
    return inplace([&](auto &a, const auto &b) { a -= b; }, rhs, Indices{});
  }
  CompositeSignal &operator*=(const CompositeSignal &rhs) {
    return inplace([&](auto &a, const auto &b) { a *= b; }, rhs, Indices{});
  }
  CompositeSignal &operator/=(const CompositeSignal &rhs) {
    return inplace([&](auto &a, const auto &b) { a /= b; }, rhs, Indices{});
  }

  CompositeSignal operator+(const double s) {
    return scalar_binary([&](const auto &a, const auto &b) { return a + b; }, s,
                         Indices{});
  }
  CompositeSignal operator-(const double s) {
    return scalar_binary([&](const auto &a, const auto &b) { return a - b; }, s,
                         Indices{});
  }
  CompositeSignal operator*(const double s) {
    return scalar_binary([&](const auto &a, const auto &b) { return a * b; }, s,
                         Indices{});
  }
  CompositeSignal operator/(const double s) {
    return scalar_binary([&](const auto &a, const auto &b) { return a / b; }, s,
                         Indices{});
  }

  CompositeSignal &operator+=(const double s) {
    return scalar_inplace([&](auto &a, const auto &b) { return a += b; }, s,
                          Indices{});
  }
  CompositeSignal &operator-=(const double s) {
    return scalar_inplace([&](auto &a, const auto &b) { return a -= b; }, s,
                          Indices{});
  }
  CompositeSignal &operator*=(const double s) {
    return scalar_inplace([&](auto &a, const auto &b) { return a *= b; }, s,
                          Indices{});
  }
  CompositeSignal &operator/=(const double s) {
    return scalar_inplace([&](auto &a, const auto &b) { return a /= b; }, s,
                          Indices{});
  }

private:
  using Tuple = std::tuple<Signals...>;
  using Indices = std::index_sequence_for<Signals...>;

  template <typename F, size_t... Is>
  CompositeSignal binary(F &&f, const CompositeSignal &rhs,
                         std::index_sequence<Is...>) {
    CompositeSignal out;
    ((std::get<Is>(out.signals_) =
          f(std::get<Is>(signals_), std::get<Is>(rhs.signals_))),
     ...);
    return out;
  }

  template <typename F, size_t... Is>
  CompositeSignal &inplace(F &&f, const CompositeSignal &rhs,
                           std::index_sequence<Is...>) {
    ((f(std::get<Is>(this->signals_), std::get<Is>(rhs.signals_))), ...);
    return *this;
  }

  template <typename F, size_t... Is>
  CompositeSignal scalar_binary(F &&f, const double s,
                                std::index_sequence<Is...>) {
    CompositeSignal out;
    ((std::get<Is>(out.signals_) = f(std::get<Is>(signals_), s)), ...);
    return out;
  }

  template <typename F, size_t... Is>
  CompositeSignal &scalar_inplace(F &&f, const double s,
                                  std::index_sequence<Is...>) {
    ((f(std::get<Is>(this->signals_), s)), ...);
    return *this;
  }

  std::tuple<Signals...> signals_;
}; // class CompositeSignal
