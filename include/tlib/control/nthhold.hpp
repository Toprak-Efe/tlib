#pragma once

#include <chrono>
#include <tlib/control/concepts/timestamped.hpp>
#include <tlib/control/concepts/vector.hpp>

template <typename T>
concept Holdable = Vector<T> && Timestamped<T>;

template <Holdable T, size_t Order> class NthOrderHold {
public:
  void push(const T &sample) {
    for (size_t i = Order; i > 0; i--) {
      m_samples[i] = m_samples[i - 1];
    }
    m_samples[0] = sample;
  }

  T sample() {
    using dur = std::chrono::duration<double>;

    T sample_curr{};
    const auto t_eval = sample_curr.stamp(); 
    for (size_t i = 0; i < Order + 1;
         i++) { // construct lagrange polynomial for index i
                // a*(t-t_1)(t-t_2)...(t-t_{Order}): j->t_j, i!=j
                // a = y_i/(t_i-t_1)...
      T sample_prev_i = m_samples.at(i);

      T a;
      double t_sum{1}, t_i_sum{1};
      for (size_t j = 0; j < Order + 1; j++) {
        if (i == j)
          continue;
        T sample_prev_j = m_samples.at(j);
        t_sum *= dur(t_eval - sample_prev_j.stamp()).count();
        t_i_sum *= dur(sample_prev_i.stamp() - sample_prev_j.stamp()).count();
      }
      a = sample_prev_i / t_i_sum;
      sample_curr += a * t_sum;
    }
    return sample_curr;
  }

private:
  std::array<T, Order + 1> m_samples;
}; // class NthOrderHold

template <Holdable T> class NthOrderHold<T, 0> {
public:
  void push(const T &sample) { m_sample = sample; }
  T sample() { return m_sample; }

private:
  T m_sample;
}; // class NthOrderHold

template <typename T> using ZeroOrderHold = NthOrderHold<T, 0>;
template <typename T> using FirstOrderHold = NthOrderHold<T, 1>;
template <typename T> using SecondOrderHold = NthOrderHold<T, 2>;
