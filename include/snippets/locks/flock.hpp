#pragma once

#include <cstdlib>
#include <fcntl.h>
#include <filesystem>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/unistd.h>
#include <unistd.h>

template <typename Identifier>
  requires requires(Identifier i) {
    { std::to_string(i) };
  }
class FileLock {
  FileLock(std::string prefix, Identifier identifier) {
    static std::filesystem::path base_path =
        std::filesystem::temp_directory_path();
    std::filesystem::path lockpath =
        base_path / (prefix + std::to_string(identifier));

    if (!(fd_ = ::open(lockpath.c_str(), O_APPEND))) {
      throw std::runtime_error(
          std::string("Unable to open file ") + lockpath.root_path().c_str() +
          " for reading. ERRNO: " + std::to_string(errno) + "\n");
    }

    if (!flock(fd_, LOCK_EX)) {
      throw std::runtime_error(
          std::string("Unable to lock file ") + lockpath.root_path().c_str() +
          ", hardware already in control. ERRNO: " + std::to_string(errno) + "\n");
    }

    return;
  }
  ~FileLock() {
    if (!flock(fd_, LOCK_UN)) {
        // not our problem lul
    }
  }

private:
  int fd_;
}; // namespace FileLOck
