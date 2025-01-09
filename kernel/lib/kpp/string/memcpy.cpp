#include <kpp/cstring.hpp>

// memcpy is declared extern "C" instead of inside the kpp namespace here because
// it is linked-to by built-in facilities (e.g. "new")
extern "C"
void* memcpy(void* __restrict dstptr, const void* __restrict srcptr, size_t size)
{
    unsigned char* dst = (unsigned char*)dstptr;
    const unsigned char* src = (const unsigned char*)srcptr;
    for (size_t i = 0; i < size; i++)
        dst[i] = src[i];
    return dstptr;
}

