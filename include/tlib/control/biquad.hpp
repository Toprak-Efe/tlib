#pragma once

#include <tlib/control/concepts/vector.hpp>

template <Vector T> class Biquad {
public:
  Biquad() : b0_(0), b1_(0), b2_(0), a1_(0), a2_(0) {}

  Biquad(double b0, double b1, double b2, double a1, double a2)
      : b0_{b0}, b1_{b1}, b2_{b2}, a1_{a1}, a2_{a2} {}

  void configure(double b0, double b1, double b2, double a1, double a2) {
    b0_ = b0;
    b1_ = b1;
    b2_ = b2;
    a1_ = a1;
    a2_ = a2;
  }

  T sample(const T &x) {
    T y = x * b0_ + x_1 * b1_ + x_2 * b2_ - y_1 * a1_ - y_2 * a2_;
    y_2 = y_1;
    y_1 = y;
    x_2 = x_1;
    x_1 = x;
    return y;
  }

private:
  double b0_;
  double b1_;
  double b2_;
  double a1_;
  double a2_;
  T x_1;
  T x_2;
  T y_1;
  T y_2;
}; // class Biquad
