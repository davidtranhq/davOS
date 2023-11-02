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

volatile limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};

struct limine_kernel_address_response *kernel_address = kernel_address_request.response;

volatile limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

struct limine_hhdm_response *hhdm_address = hhdm_request.response;

};