#ifndef DAVOS_STRING_H_INCLUDED
#define DAVOS_STRING_H_INCLUDED

#include <compiler/macros.h>
#include <stddef.h>

BEGIN_CDECLS

int memcmp(const void*, const void*, size_t);
void* memcpy(void *__restrict, const void *__restrict, size_t);
void* memmove(void*, const void*, size_t);
void* memset(void*, int, size_t);
size_t strlen(const char*);
bool strncmp(const char*, const char*, size_t);

END_CDECLS

#endif
