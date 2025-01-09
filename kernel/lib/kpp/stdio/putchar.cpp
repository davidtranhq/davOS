#include <kpp/cstdio.hpp>

#include <kernel/Terminal.hpp>

int kpp::putchar(int ic)
{
    char c = (char)ic;
    KernelTerminal::instance->write(c);
    return ic;
}
