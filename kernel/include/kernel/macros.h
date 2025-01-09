#pragma once

#include <kpp/cstdio.hpp>

#ifdef DEBUG_BUILD

#define DEBUG(fmt, ...) \
    kpp::printf("[DEBUG] %s:%d:" fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#define ERROR(fmt, ...) \
    kpp::printf("[ERROR] %s:%d:" fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#else

#define DEBUG(fmt, ...)
#define ERROR(fmt, ...)

#endif
