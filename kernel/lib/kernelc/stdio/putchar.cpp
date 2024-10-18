#include <stdio.h>

#include <kernel/Terminal.hpp>

int putchar(int ic)
{
    char c = (char)ic;
    KernelTerminal::instance->write(c);
    return ic;
}
