#pragma once

#include <span>
#include <cstddef>
#include <vector>

template <typename T>
concept Serializable = requires (T t, std::vector<std::byte> &b, std::span<const std::byte> s) {
    {T::serial_save(b, t)} -> std::same_as<decltype(b)>;
    {T::serial_load(s, t)} -> std::same_as<T>;
};

