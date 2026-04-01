#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <tlib/concurrency/triplebuffer.hpp>
#include <tlib/control/nthhold.hpp>
#include <tlib/control/telemetry.hpp>

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
