#include <catch2/catch_test_macros.hpp>
#include <tlib/control/devices.hpp>
#include <tlib/control/spatial.hpp>

class MockBasicDevice : DeviceInterface<Twist, Wrench> {
public:
  MockBasicDevice() = delete;
  MockBasicDevice(const MockBasicDevice &) = delete;
  MockBasicDevice(SignalPort<Twist> *sensor_port,
                  SignalPort<Wrench> *command_port)
      : DeviceInterface{sensor_port, command_port} {}
  MockBasicDevice(MockBasicDevice &&rhs)
      : DeviceInterface{rhs.sensor_, rhs.command_} {
    rhs.sensor_ = nullptr;
    rhs.command_ = nullptr;
  }

  void start() { return; }
  void stop() { return; }

private:
}; // class MockDevice

class MockComplexDevice
    : DeviceInterface<CompositeSignal<Twist, Displacement>, Wrench> {
public:
  MockComplexDevice() = delete;
  MockComplexDevice(const MockComplexDevice &) = delete;
  MockComplexDevice(
      SignalPort<CompositeSignal<Twist, Displacement>> *sensor_port,
      SignalPort<Wrench> *command_port)
      : DeviceInterface{sensor_port, command_port} {}
  MockComplexDevice(MockComplexDevice &&rhs)
      : DeviceInterface{rhs.sensor_, rhs.command_} {
    rhs.sensor_ = nullptr;
    rhs.command_ = nullptr;
  }

  void start() { return; }
  void stop() { return; }

private:
}; // class MockComplexDevice

TEST_CASE("Mock Device Interface", "[Devices]") {
  SECTION("Simple Signal Device") {
    SignalPort<Twist> sensor_port;
    SignalPort<Wrench> command_port;
    MockBasicDevice device(&sensor_port, &command_port);
    device.start();
    device.stop();
    SUCCEED("Reached the end without exceptions.");
  }

  SECTION("Complex Signal Device") {
    SignalPort<CompositeSignal<Twist, Displacement>> sensor_port;
    SignalPort<Wrench> command_port;
    MockComplexDevice device(&sensor_port, &command_port);
    device.start();
    device.stop();
    SUCCEED("Reached the end without exceptions.");
  }
}
