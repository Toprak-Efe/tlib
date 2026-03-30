#pragma once

#include <algorithm>
#include <tlib/control/concepts/timestamped.hpp>
#include <tlib/control/spatial.hpp>

template <typename A, typename B> class SpatialPair {
public:
  using Scalar = double;
  using Clock = std::chrono::steady_clock;
  using Timepoint = Clock::time_point;

  SpatialPair() = default;

  SpatialPair(const A &a, const B &b)
      : a_(a), b_(b), timestamp_(std::max(a.stamp(), b.stamp())) {}

  SpatialPair(A &&a, B &&b)
      : a_(std::move(a)), b_(std::move(b)),
        timestamp_(std::max(a_.stamp(), b_.stamp())) {}

  SpatialPair(const SpatialPair &) = default;
  SpatialPair(SpatialPair &&) = default;
  SpatialPair &operator=(const SpatialPair &) = default;
  SpatialPair &operator=(SpatialPair &&) = default;
  ~SpatialPair() = default;

  const A &first() const { return a_; }
  A &first() { return a_; }
  const B &second() const { return b_; }
  B &second() { return b_; }

  const Timepoint &stamp() const { return timestamp_; }
  Timepoint &stamp() { return timestamp_; }

  SpatialPair operator+(const SpatialPair &rhs) const {
    return {a_ + rhs.a_, b_ + rhs.b_};
  }
  SpatialPair operator-(const SpatialPair &rhs) const {
    return {a_ - rhs.a_, b_ - rhs.b_};
  }
  SpatialPair operator*(const SpatialPair &rhs) const {
    return {a_ * rhs.a_, b_ * rhs.b_};
  }
  SpatialPair operator/(const SpatialPair &rhs) const {
    return {a_ / rhs.a_, b_ / rhs.b_};
  }

  SpatialPair &operator+=(const SpatialPair &rhs) {
    a_ += rhs.a_;
    b_ += rhs.b_;
    timestamp_ = std::max(timestamp_, rhs.timestamp_);
    return *this;
  }
  SpatialPair &operator-=(const SpatialPair &rhs) {
    a_ -= rhs.a_;
    b_ -= rhs.b_;
    timestamp_ = std::max(timestamp_, rhs.timestamp_);
    return *this;
  }
  SpatialPair &operator*=(const SpatialPair &rhs) {
    a_ *= rhs.a_;
    b_ *= rhs.b_;
    timestamp_ = std::max(timestamp_, rhs.timestamp_);
    return *this;
  }
  SpatialPair &operator/=(const SpatialPair &rhs) {
    a_ /= rhs.a_;
    b_ /= rhs.b_;
    timestamp_ = std::max(timestamp_, rhs.timestamp_);
    return *this;
  }

  SpatialPair operator+(Scalar s) const { return {a_ + s, b_ + s}; }
  SpatialPair operator-(Scalar s) const { return {a_ - s, b_ - s}; }
  SpatialPair operator*(Scalar s) const { return {a_ * s, b_ * s}; }
  SpatialPair operator/(Scalar s) const { return {a_ / s, b_ / s}; }

  friend SpatialPair operator*(Scalar s, const SpatialPair &v) { return v * s; }

  SpatialPair &operator+=(Scalar s) {
    a_ += s;
    b_ += s;
    return *this;
  }
  SpatialPair &operator-=(Scalar s) {
    a_ -= s;
    b_ -= s;
    return *this;
  }
  SpatialPair &operator*=(Scalar s) {
    a_ *= s;
    b_ *= s;
    return *this;
  }
  SpatialPair &operator/=(Scalar s) {
    a_ /= s;
    b_ /= s;
    return *this;
  }

private:
  A a_{};
  B b_{};
  Timepoint timestamp_{};
}; // class SpatialPair

using TwistDisplacement = SpatialPair<Twist, Displacement>;
using WrenchDisplacement = SpatialPair<Wrench, Displacement>;
