#ifdef __linux__
#error "cross-compiler not in-use"
#endif

#ifndef __i386__
#error "incorrect cross-compilation target: should target ix86-elf"
#endif

#include "Terminal.hpp"

extern "C"
{

void kernel_main()
{
    Terminal terminal;
    terminal.write_string("Hello, kernel!\n");
}

}