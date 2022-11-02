/**
 * A simple VGA text terminal used by the kernel.
 * Adapted from https://wiki.osdev.org/Bare_Bones.
 */

#ifndef HEADER_GUARD_TERMINAL_HPP
#define HEADER_GUARD_TERMINAL_HPP

#include <cstdint>
#include <cstddef>

#include "VGAChar.hpp"
#include "VGAColor.hpp"

class Terminal
{
public:
    Terminal();

    void clear();
    void put_char(VGAChar vga_char);
    void put_char_at(VGAChar vga_char, std::size_t row, std::size_t column);
    void write_string(const char *str);

private:
    void advance_cursor();

    void write(const char *str, std::size_t length);

    /**
     * The kernel doesn't have access to this part of the STL,
     * so we need to implement it ourselves 
     */
    static std::size_t strlen(const char *str);

private:
    // the VGA text mode buffer is fixed at 0xb8000
    uint16_t *buffer = reinterpret_cast<uint16_t *>(0xb8000);
    std::size_t width = 80;
    std::size_t height = 25;
    std::size_t cursor_row = 0;
    std::size_t cursor_column = 0;
    VGAColor fg_color = VGAColor::white;
    VGAColor bg_color = VGAColor::black;
};

#endif
