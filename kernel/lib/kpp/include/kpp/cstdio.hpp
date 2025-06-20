#pragma once

#include <cstdarg>

namespace kpp {
int vprintf(const char* fmt, va_list args);
int printf(const char* __restrict, ...);
int putchar(int);
int puts(const char*);
int getchar();
} // namespace kpp
