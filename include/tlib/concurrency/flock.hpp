#pragma once

#include <cerrno>
#include <fcntl.h>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <sys/file.h>
#include <sys/stat.h>
#include <type_traits>
#include <unistd.h>

template <typename Identifier>
  requires(std::convertible_to<Identifier, std::string> ||
           requires(Identifier i) {
             { std::to_string(i) };
           })
class FileLock {
public:
  FileLock() = delete;
  FileLock(const FileLock &) = delete;
  FileLock &operator=(const FileLock &) = delete;
  FileLock(FileLock &&oth) {
    if (oth.fd_ < 0) {
      return;
    }
    if (fd_ >= 0) {
      ::flock(fd_, LOCK_UN);
      ::close(fd_);
    }
    fd_ = oth.fd_;
    oth.fd_ = -1;
  }
  FileLock &operator=(FileLock &&rhs) {
    if (this != &rhs) {
      if (fd_ >= 0) {
        ::flock(fd_, LOCK_UN);
        ::close(fd_);
      }
      fd_ = rhs.fd_;
      rhs.fd_ = -1;
    }
    return *this;
  }

  explicit FileLock(std::string prefix, Identifier identifier) {
    const static std::filesystem::path base_path =
        std::filesystem::temp_directory_path();
    std::filesystem::path lockpath;

    if constexpr (std::is_convertible_v<Identifier, std::string>) {
      lockpath = base_path / (prefix + '_' + identifier);
    } else {
      lockpath = base_path / (prefix + '_' + std::to_string(identifier));
    }

    if ((fd_ = ::open(lockpath.c_str(), O_CREAT | O_WRONLY, 644)) < 0) {
      throw std::runtime_error(std::string("Unable to open file ") +
                               lockpath.c_str() +
                               ", ERRNO: " + std::to_string(errno));
    }

    if (::flock(fd_, LOCK_EX | LOCK_NB) < 0) {
      ::close(fd_);
      throw std::runtime_error(std::string("Unable to lock file ") +
                               lockpath.c_str() +
                               ", ERRNO: " + std::to_string(errno));
    }
  }
  ~FileLock() {
    ::flock(fd_, LOCK_UN);
    ::close(fd_);
  }

private:
  int fd_{-1};
}; // class FileLock
