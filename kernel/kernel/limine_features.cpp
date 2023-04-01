#include <kernel/limine_features.h>

namespace limine
{

volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

struct limine_memmap_response *memory_map = memmap_request.response;

volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 1
};

struct limine_framebuffer_response *framebuffers_info = framebuffer_request.response;

};