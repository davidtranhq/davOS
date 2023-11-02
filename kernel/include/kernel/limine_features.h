/**
 * @file limine_features.h
 * @brief Globally accessible Limine structures loaded by the bootloader.
 *
 * See https://github.com/limine-bootloader/limine/blob/trunk/PROTOCOL.md
 * for documentation on supported Limine features.
 */

#include <kernel/limine.h>

namespace limine
{

extern struct limine_memmap_response *memory_map;
extern struct limine_framebuffer_response *framebuffers_info;
extern struct limine_kernel_address_response *kernel_address;
extern struct limine_hhdm_response *hhdm_address;

}
