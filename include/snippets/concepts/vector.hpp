#pragma once

#include <snippets/concepts/arithmetic.hpp>

template <typename T>
concept Vector = SelfArithmetic<T> && ScalarArithmetic<T>; 
