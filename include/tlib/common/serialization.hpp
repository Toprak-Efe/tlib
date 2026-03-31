#pragma once

#include <cereal/cereal.hpp>
#include <tlib/control/spatial.hpp>
#include <chrono>

namespace cereal {

template <class Archive, class Scalar, int Rows, int Cols, int Options,
          int MaxRows, int MaxCols>
void save(
    Archive &ar,
    Eigen::Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols> const &m) {
  int rows = m.rows();
  int cols = m.cols();
  ar(rows, cols);
  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
      ar(m(i, j));
}

template <class Archive, class Scalar, int Rows, int Cols, int Options,
          int MaxRows, int MaxCols>
void load(Archive &ar,
          Eigen::Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols> &m) {
  int rows, cols;
  ar(rows, cols);
  m.resize(rows, cols);
  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
      ar(m(i, j));
}

template <class Archive, class Clock, class Duration>
void save(Archive& ar, const std::chrono::time_point<Clock, Duration>& tp) {
    auto d = tp.time_since_epoch().count();
    ar(d);
}

template <class Archive, class Clock, class Duration>
void load(Archive& ar, std::chrono::time_point<Clock, Duration>& tp) {
    typename Duration::rep d;
    ar(d);
    tp = std::chrono::time_point<Clock, Duration>(Duration(d));
}

}; // namespace cereal
