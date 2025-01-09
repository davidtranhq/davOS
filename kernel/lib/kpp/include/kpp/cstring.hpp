#pragma once

#include <cstddef>

namespace kpp {

int memcmp(const void*, const void*, std::size_t);

// memcpy is declared extern "C" here because it is linked-to by
// built-in functions (e.g. "new")
extern "C" void* memcpy(void* __restrict, const void* __restrict, size_t);

void* memmove(void*, const void*, size_t);
void* memset(void*, int, size_t);
size_t strlen(const char*);
int strncmp(const char*, const char*, size_t);

} // namespace kpp
