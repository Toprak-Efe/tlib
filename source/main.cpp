#include <snippets/concepts/logpolicy.hpp>
#include <snippets/locks/flock.hpp>
#include <functional>
#include <iostream>
#include <format>

class ConsoleLogger {
public:
  enum LogLevel { INFO, WARNING, ERROR, DEBUG };

  template <LogLevel Level = INFO>
  void log(const std::string &msg) {
    std::format_string fmt = "{} [{}] {}";
    if constexpr (Level == LogLevel::INFO) {
        std::clog << 
    } else if constexpr (Level == LogLevel::WARNING) {
        std::clog <<
    } else if constexpr (Level == LogLevel::ERROR) {
        std::cerr <<
    } else if constexpr (Level == LogLevel::DEBUG) {
        std::clog <<
    }
  }

private:
};

int main(int argc, char *argv[]) { return 0; }
