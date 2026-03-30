#pragma once

#include <tlib/control/signal.hpp>

template <typename SensorType, typename CommandType> class DeviceInterface {
  using SensorPort = SignalPort<SensorType>;
  using CommandPort = SignalPort<CommandType>;

public:
  DeviceInterface(SensorPort &sensor_port, CommandPort &command_port)
      : sensor_(sensor_port), command_(command_port) {}
  virtual void start() = 0;
  virtual void stop() = 0;
  virtual ~DeviceInterface() = default;

protected:
  CommandPort &command_;
  SensorPort &sensor_;
}; // class DeviceInterface
