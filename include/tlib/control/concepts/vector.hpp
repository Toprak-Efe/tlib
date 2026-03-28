#pragma once

#include <tlib/control/concepts/arithmetic.hpp>

template <typename T>
concept Vector = SelfArithmetic<T> && ScalarArithmetic<T>; 

