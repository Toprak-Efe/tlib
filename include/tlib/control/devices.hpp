#pragma once

#include <tlib/control/signal.hpp>

template <typename SensorType, typename CommandType> class DeviceInterface {
  using SensorPort = SignalPort<SensorType>;
  using CommandPort = SignalPort<CommandType>;

public:
  DeviceInterface(SensorPort &sensor_port, CommandPort &command_port)
      : sensor_out_(sensor_port), command_out_(command_port) {}
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual ~DeviceInterface() = default;

protected:
  CommandPort &command_out_;
  SensorPort &sensor_out_;
}; // class DeviceInterface
