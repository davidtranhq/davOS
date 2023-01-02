#ifndef DAVOS_KERNEL_TERMINAL_H_INCLUDED
#define DAVOS_KERNEL_TERMINAL_H_INCLUDED

#include <stddef.h>

void terminal_init(void);
void terminal_write(const char *str, size_t size);

#endif
