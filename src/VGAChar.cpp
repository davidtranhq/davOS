#include "VGAChar.hpp"
#include "VGAColor.hpp"

VGAChar::VGAChar(
    uint8_t code_point,
    VGAColor fg_color,
    VGAColor bg_color,
    bool blink
) : code_point {code_point},
    fg_color {fg_color},
    bg_color {bg_color},
    blink {blink}
{}

uint16_t VGAChar::to_uint16_t()
{
    uint16_t fg = static_cast<uint16_t>(fg_color);
    uint16_t bg = static_cast<uint16_t>(bg_color);
    uint16_t blink_flag = static_cast<uint16_t>(blink);

    uint16_t result = code_point
        | (fg << 8)
        | (bg << 12)
        | (blink_flag << 15);
    return result;
}

bool VGAChar::is_newline()
{
    return code_point == '\n';
}