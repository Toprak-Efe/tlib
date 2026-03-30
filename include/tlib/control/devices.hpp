#pragma once

#include <tlib/control/signal.hpp>

template <typename SensorType, typename CommandType, size_t SensorHold = 0,
          size_t CommandHold = 0>
class DeviceInterface {
  using SensorPort = SignalPort<SensorType, 0>;
  using CommandPort = SignalPort<CommandType, 0>;

public:
  DeviceInterface(SensorPort &sensor_port, CommandPort &command_port)
      : sensor_(sensor_port), command_(command_port) {}
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual ~DeviceInterface() = default;

protected:
  SensorPort &sensor_;
  CommandPort &command_;
}; // class DeviceInterface
