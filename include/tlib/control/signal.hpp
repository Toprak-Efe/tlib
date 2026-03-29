#pragma once

#include <memory>
#include <string>
#include <tlib/concurrency/triplebuffer.hpp>
#include <tlib/control/telemetry.hpp>

template <typename Signal> class SignalPort {
  static constexpr std::size_t QueueSize{1024};

public:
  SignalPort() = default;
  explicit SignalPort(const std::string &channel_name) {
    telemetry_ = std::make_unique<TelemetryChannel<Signal>>(channel_name);
  }

  bool sample(Signal &signal) { return buffer_.get(signal); }

  void push(const Signal &signal) {
    buffer_.add(signal);
    if (telemetry_) {
      telemetry_.push(signal);
    }
  }

private:
  TripleBuffer<Signal> buffer_;
  std::unique_ptr<TelemetryChannel<Signal>> telemetry_;
}; // class SignalPort

