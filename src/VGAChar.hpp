/**
 * A single VGA character following the VGA text mode specification
 * https://en.wikipedia.org/wiki/VGA_text_mode.
 */

#ifndef HEADER_GUARD_VGA_CHAR_HPP
#define HEADER_GUARD_VGA_CHAR_HPP

#include <cstdint>

#include "VGAColor.hpp"

class VGAChar
{
public:
    VGAChar(
        uint8_t code_point,
        VGAColor fg_color,
        VGAColor bg_color,
        bool blink = false
    );

    /**
     * bit pattern:
     * 0-7: ASCII code point
     * 8-11: foreground color
     * 12-14: background color
     * 15: blink flag (1 for present, 0 for hidden)
     */
    uint16_t to_uint16_t();

    bool is_newline();

private:
    uint8_t code_point;
    VGAColor fg_color;
    VGAColor bg_color;
    bool blink;
};

#endif
