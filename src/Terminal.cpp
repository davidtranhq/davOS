#include <cstdint>
#include <cstddef>

#include "Terminal.hpp"
#include "VGAChar.hpp"

Terminal::Terminal()
{
    clear();
}

void Terminal::clear()
{
    for (std::size_t y = 0; y < height; ++y)
    {
        for (std::size_t x = 0; x < width; ++x)
        {
            const std::size_t index = y * width + x;
            auto space_char = VGAChar(' ', fg_color, bg_color);
            buffer[index] = space_char.to_uint16_t();
        }
    }
}

void Terminal::put_char(VGAChar vga_char)
{
    if (!vga_char.is_newline())
        put_char_at(vga_char, cursor_row, cursor_column);
    advance_cursor();
}

void Terminal::put_char_at(VGAChar vga_char, std::size_t row, std::size_t column)
{
    const std::size_t index {cursor_row * width + cursor_column};
    buffer[index] = vga_char.to_uint16_t();
}

void Terminal::write_string(const char *str)
{
    write(str, Terminal::strlen(str));
}

void Terminal::advance_cursor()
{
    cursor_column += 1;
    if (cursor_column == width)
    {
        cursor_column = 0;
        cursor_row += 1;
        if (cursor_row == height)
        {
            cursor_row = 0;
        }
    }
}

void Terminal::write(const char *str, std::size_t length)
{
    for (std::size_t i = 0; i < length; ++i)
    {
        auto vga_char = VGAChar(str[i], fg_color, bg_color);
        put_char(vga_char);
    }
}

std::size_t Terminal::strlen(const char *str)
{
    std::size_t len = 0;
    while (str[len])
        len += 1;
    return len;
}