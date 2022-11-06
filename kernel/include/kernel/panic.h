#ifndef DAVOS_KERNEL_PANIC_H_INCLUDED
#define DAVOS_KERNEL_PANIC_H_INCLUDED

#include <compiler/macros.h>

BEGIN_CDECLS

__attribute__((__noreturn__)) 
void panic(const char *fmt, ...);

END_CDECLS

#endif
