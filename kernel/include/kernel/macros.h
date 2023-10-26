#ifndef DAVOS_KERNEL_MACROS_H_INCLUDED
#define DAVOS_KERNEL_MACROS_H_INCLUDED

#include <stdio.h>

#define DEBUG(fmt, ...) \
    printf("[DEBUG] %s:%d:" fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#define ERROR(fmt, ...) \
    printf("[ERROR] %s:%d:" fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#endif