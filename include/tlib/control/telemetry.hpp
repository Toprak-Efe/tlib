#pragma once

#include <chrono>
#include <filesystem>
#include <mutex>
#include <stop_token>
#include <thread>
#include <tlib/common/serialization.hpp>
#include <tlib/concurrency/ringbuffer.hpp>
#include <vector>

class DrainableChannel {
public:
  /**
   * @desc Empty queue into a buffer.
   */
  virtual void drain() = 0;
  virtual ~DrainableChannel() = default;
};

class TelemetrySink {
public:
  static TelemetrySink &instance() {
    static TelemetrySink global_sink;
    return global_sink;
  }

  ~TelemetrySink() {
    worker_.request_stop();
    if (worker_.joinable())
      worker_.join();
  }

  void register_channel(DrainableChannel *chn) {
    std::lock_guard<std::mutex> guard{mutex_};
    channels_.emplace_back(chn);
  }

  void unregister_channel(DrainableChannel *chn) {
    std::lock_guard<std::mutex> guard{mutex_};
    std::erase(channels_, chn);
  }

private:
  TelemetrySink() {
    worker_ = std::jthread([this](std::stop_token s) {
      using namespace std::chrono_literals;
      while (!s.stop_requested()) {
        {
          std::lock_guard<std::mutex> guard{mutex_};
          for (auto chn : channels_)
            chn->drain();
        }
        std::this_thread::sleep_for(100ms);
      }
    });
  }

  std::mutex mutex_;
  std::jthread worker_;
  std::vector<DrainableChannel *> channels_;
}; // class TelemetrySink

template <typename T> class TelemetryChannel : public DrainableChannel {
public:
  TelemetryChannel(const std::string &channel_name)
      : channel_name_{channel_name} {
    buffer_.reserve(1024);
    TelemetrySink::instance().register_channel(this);
  }

  ~TelemetryChannel() {
    TelemetrySink::instance().unregister_channel(this);
    drain();
    flush();
  }

  void push(const T &data) { queue_.add(data); }

  void drain() override {
    T data{};
    while (queue_.get(data)) {
      buffer_.emplace_back(std::move(data));
    }
  }

  void flush() {
    static auto log_folder =
        std::filesystem::temp_directory_path() / std::string("tlibtelemetry");
    static auto filename =
        std::format("{:%Y%m%d%H%M}", std::chrono::system_clock::now());
    auto log_file = log_folder / channel_name_ / filename;
    std::filesystem::create_directories(log_file.parent_path());
    serialize_vector(log_file, buffer_);
  }

private:
  std::string channel_name_;
  RingBuffer<T, 1024> queue_;
  std::vector<T> buffer_;
};
