#pragma once

#include <tlib/control/concepts/timestamped.hpp>
#include <tlib/control/concepts/vector.hpp>

template <typename T>
concept Holdable = Vector<T> && Timestamped<T>;
