#ifndef DAVOS_KERNEL_CONSTANTS
#define DAVOS_KERNEL_CONSTANTS

#include <cstddef>

constexpr auto page_size = std::size_t {0x1000};
constexpr auto frame_size = std::size_t {0x1000};

#endif