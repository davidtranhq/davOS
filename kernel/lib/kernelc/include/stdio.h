#ifndef DAVOS_STDIO_H_INCLUDED
#define DAVOS_STDIO_H_INCLUDED

#include <compiler/macros.h>
#include <stdarg.h>

#define EOF (-1)

BEGIN_CDECLS

int vprintf(const char *fmt, va_list args);
int printf(const char *__restrict, ...);
int putchar(int);
int puts(const char*);

END_CDECLS

#endif
