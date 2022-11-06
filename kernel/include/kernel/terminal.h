#ifndef DAVOS_KERNEL_TERMINAL_H_INCLUDED
#define DAVOS_KERNEL_TERMINAL_H_INCLUDED

#include <compiler/macros.h>
#include <stddef.h>

BEGIN_CDECLS

void terminal_init(void);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);

END_CDECLS

#endif
