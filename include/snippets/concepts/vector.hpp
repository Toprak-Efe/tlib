#pragma once

#include <snippets/concepts/arithmetic.hpp>
#include <snippets/concepts/container.hpp>

template <typename T>
concept Vector = SelfArithmetic<T> && ScalarArithmetic<T> && Container<T>;
