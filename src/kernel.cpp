#ifdef __linux__
#error "cross-compiler not in-use"
#endif

#ifndef __i386__
#error "incorrect cross-compilation target: should target ix86-elf"
#endif

#include "Terminal.hpp"
#include "VGAChar.hpp"
#include "VGAColor.hpp"

void hello_test(Terminal &terminal)
{
    terminal.write_string("Hello, kernel!\n");
}

void newline_test(Terminal &terminal)
{
    terminal.write_string("Line 1\nLine 2\n");
}

void scroll_test(Terminal &terminal)
{
    // no memory allocation yet, need to do this by hand...
    terminal.write_string("0\n");
    terminal.write_string("1\n");
    terminal.write_string("2\n");
    terminal.write_string("3\n");
    terminal.write_string("4\n");
    terminal.write_string("5\n");
    terminal.write_string("6\n");
    terminal.write_string("7\n");
    terminal.write_string("8\n");
    terminal.write_string("9\n");
    terminal.write_string("10\n");
    terminal.write_string("11\n");
    terminal.write_string("12\n");
    terminal.write_string("13\n");
    terminal.write_string("14\n");
    terminal.write_string("15\n");
    terminal.write_string("16\n");
    terminal.write_string("17\n");
    terminal.write_string("18\n");
    terminal.write_string("19\n");
    terminal.write_string("20\n");
    terminal.write_string("21\n");
    terminal.write_string("22\n");
    terminal.write_string("23\n");
    terminal.write_string("24\n");
    terminal.write_string("25\n");
    terminal.write_string("26\n");
    terminal.write_string("27\n");
    terminal.write_string("28\n");
    terminal.write_string("29\n");
    terminal.write_string("30\n");
}

void color_test(Terminal &terminal)
{
    VGAChar chars[16] = {
        VGAChar('a', VGAColor::black, VGAColor::blue),
        VGAChar('b', VGAColor::blue, VGAColor::green),
        VGAChar('c', VGAColor::green, VGAColor::cyan),
        VGAChar('d', VGAColor::cyan, VGAColor::red),
        VGAChar('e', VGAColor::red, VGAColor::magenta),
        VGAChar('f', VGAColor::magenta, VGAColor::brown),
        VGAChar('g', VGAColor::brown, VGAColor::light_grey),
        VGAChar('h', VGAColor::light_grey, VGAColor::dark_grey),
        VGAChar('i', VGAColor::dark_grey, VGAColor::light_blue),
        VGAChar('j', VGAColor::light_blue, VGAColor::light_green),
        VGAChar('k', VGAColor::light_green, VGAColor::light_cyan),
        VGAChar('l', VGAColor::light_cyan, VGAColor::light_red),
        VGAChar('m', VGAColor::light_red, VGAColor::light_magenta),
        VGAChar('n', VGAColor::light_magenta, VGAColor::light_brown),
        VGAChar('o', VGAColor::light_brown, VGAColor::white),
        VGAChar('p', VGAColor::white, VGAColor::black),
    };

    for (int i = 0; i < 16; ++i)
    {
        terminal.put_char(chars[i]);
    }
}

extern "C"
{

void kernel_main()
{
    Terminal terminal;
    scroll_test(terminal);
    hello_test(terminal);
    newline_test(terminal);
    color_test(terminal);
}

}