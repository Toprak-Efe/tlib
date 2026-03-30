#pragma once

#include <algorithm>
#include <array>
#include <chrono>
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
      : data(vec.data), timestamp(vec.timestamp) {
    vec.data.setZero();
    vec.timestamp = Timepoint{};
  }
  SpatialVector(const SpatialVector &vec)
      : data(vec.data), timestamp(vec.timestamp) {}
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

  SpatialOperator() : m_mat(Matrix6::Identity()) {}
  SpatialOperator(const Matrix6 &mat) : m_mat{mat} {}
  SpatialOperator(const SpatialOperator &oth) : m_mat(oth.m_mat) {}
  SpatialOperator(SpatialOperator &&oth) : m_mat(oth.m_mat) {}

  SpatialVector<ToTag> operator*(const SpatialVector<FromTag> &v) const {
    return SpatialVector<ToTag>(m_mat * v.vec());
  }

private:
  Matrix6 m_mat;
}; // class SpatialOperator

using Impedance = SpatialOperator<TwistTag, WrenchTag>;
using Admittance = SpatialOperator<WrenchTag, TwistTag>;
using Adjoint = SpatialOperator<TwistTag, TwistTag>;
using Coadjoint = SpatialOperator<WrenchTag, WrenchTag>;
using Stiffness = SpatialOperator<DisplacementTag, WrenchTag>;
using PositionGain = SpatialOperator<DisplacementTag, TwistTag>;
