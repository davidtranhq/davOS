#ifndef DAVOS_KERNEL_FRAME_ALLOCATOR_H_INCLUDED
#define DAVOS_KERNEL_FRAME_ALLOCATOR_H_INCLUDED

#include <stdint.h>

/**
 * @brief Get a new physical frame. Returns the physical address of the new frame.
 */
uintptr_t allocate_frame();

/**
 * @brief Frees a previously allocated physical frame. Do not deallocate the same frame twice
 * (the function does not check for this).
 */
void deallocate_frame(uintptr_t frame_to_deallocate);

#endif