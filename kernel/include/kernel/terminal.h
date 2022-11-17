#ifndef DAVOS_KERNEL_TERMINAL_H_INCLUDED
#define DAVOS_KERNEL_TERMINAL_H_INCLUDED

#include <stddef.h>

#include <compiler/macros.h>

BEGIN_CDECLS

void terminal_init(void);
void terminal_write(const char *str, size_t size);

END_CDECLS

#endif
