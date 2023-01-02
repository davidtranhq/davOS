# Terminal

The terminal is rendered using the framebuffers exposed by Limine's framebuffer feature. We implement our own terminal instead of using Limine's built in terminal feature, since the terminal feature requires some additional runtime constraints (an additional entry in the page table that maintains the identity mapping) that we would rather do without.

## Rendering the Terminal

The Limine framebuffer structure contains a pointer to the framebuffer region in memory.

```c++
// Constants for `memory_model`
#define LIMINE_FRAMEBUFFER_RGB 1

struct limine_framebuffer
{
    void *address; // address to the framebuffer region in memory
    uint64_t width;
    uint64_t height;
    uint64_t pitch; // number of bytes per line
    uint16_t bpp; // bits per pixel
    uint8_t memory_model; // only LIMINE_FRAMEBUFFER_RGB
    uint8_t red_mask_size;
    uint8_t red_mask_shift;
    uint8_t green_mask_size;
    uint8_t green_mask_shift;
    uint8_t blue_mask_size;
    uint8_t blue_mask_shift;
    uint8_t unused[7];
    uint64_t edid_size;
    void *edid; // display metadata

    /* Revision 1 */
    uint64_t mode_count;
    struct limine_video_mode **modes;
};
```
The format of the pixels in memory is defined according to the `memory_model` in use. Currently, the only supported format is 32-bits per RGBA pixel, where each channel is 8-bits. So, writing `0xffffffff` to `address[10 * width + 7]` would draw a white pixel at coordinate `(7, 10)`.