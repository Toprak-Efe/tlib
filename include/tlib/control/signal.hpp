#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <tlib/concurrency/triplebuffer.hpp>
#include <tlib/control/nthhold.hpp>
#include <tlib/control/telemetry.hpp>
#include <tuple>
#include <utility>

template <typename... Signals>
  requires((sizeof...(Signals) > 0) && (Timestamped<Signals> && ...))
class CompositeSignal {
public:
  using Clock = std::chrono::steady_clock;
  using Timepoint = Clock::time_point;
  static constexpr size_t SignalCount = sizeof...(Signals);

  CompositeSignal() = default;
  explicit CompositeSignal(const Signals &...args) : signals_{args...} {}
  explicit CompositeSignal(Signals &&...args) : signals_{std::move(args)...} {}

  Timestamp stamp() const {
    return std::apply([](const auto &...s) { return std::max({s.stamp()...}); },
                      signals_);
  }

  template <size_t I> auto &get() { return std::get<I>(signals_); }
  template <size_t I> const auto &get() const { return std::get<I>(signals_); }

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
    return inplace([&](auto &a, const auto &b) { a += b; }, rhs,
                   Indices{});
  }
  CompositeSignal &operator-=(const CompositeSignal &rhs) {
    return inplace([&](auto &a, const auto &b) { a -= b; }, rhs,
                   Indices{});
  }
  CompositeSignal &operator*=(const CompositeSignal &rhs) {
    return inplace([&](auto &a, const auto &b) { a *= b; }, rhs,
                   Indices{});
  }
  CompositeSignal &operator/=(const CompositeSignal &rhs) {
    return inplace([&](auto &a, const auto &b) { a /= b; }, rhs,
                   Indices{});
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
    return scalar_inplace([&](auto &a, const auto &b) { return a += b; },
                          s, Indices{});
  }
  CompositeSignal &operator-=(const double s) {
    return scalar_inplace([&](auto &a, const auto &b) { return a -= b; },
                          s, Indices{});
  }
  CompositeSignal &operator*=(const double s) {
    return scalar_inplace([&](auto &a, const auto &b) { return a *= b; },
                          s, Indices{});
  }
  CompositeSignal &operator/=(const double s) {
    return scalar_inplace([&](auto &a, const auto &b) { return a /= b; },
                          s, Indices{});
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

  Timepoint timestamp_;
  std::tuple<Signals...> signals_;
}; // class CompositeSignal

template <typename Signal, size_t HoldOrder = 0> class SignalPort {
  static constexpr std::size_t QueueSize{1024};

public:
  SignalPort() = default;
  explicit SignalPort(const std::string &channel_name) {
    telemetry_ = std::make_unique<TelemetryChannel<Signal>>(channel_name);
  }

  Signal sample() {
    Signal signal;
    if (buffer_.get(signal)) {
      hold_.push(signal);
    } else {
      signal = hold_.sample();
    }
    return signal;
  }

  void push(const Signal &signal) {
    buffer_.add(signal);
    if (telemetry_) {
      telemetry_->push(signal);
    }
  }

private:
  TripleBuffer<Signal> buffer_;
  NthOrderHold<Signal, HoldOrder> hold_;
  std::unique_ptr<TelemetryChannel<Signal>> telemetry_;
}; // class SignalPort
