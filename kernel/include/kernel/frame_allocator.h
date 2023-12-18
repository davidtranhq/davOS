#ifndef DAVOS_KERNEL_FRAME_ALLOCATOR_H_INCLUDED
#define DAVOS_KERNEL_FRAME_ALLOCATOR_H_INCLUDED

#include <stdint.h>

void frame_allocator_init();

/**
 * @brief Get a new physical frame. Returns the physical address of the new frame.
 */
void *allocate_frame();

/**
 * @brief Frees a previously allocated physical frame. Do not deallocate the same frame twice
 * (the function does not check for this).
 */
void deallocate_frame(void *frame_to_deallocate);

/**
 * @brief Get the number of available frames left. 
 */
uint64_t available_frames();

/**
 * @brief Get a pointer pointing to the corresponding virtual address of a physical address.
 */
void *kernel_physical_to_virtual(void *physical_address);

/**
 * @brief Get the virtual address of the given physical kernel address.
 */
uintptr_t kernel_physical_to_virtual(uintptr_t physical_address);

/**
 * @brief Reclaim bootloader-reclaimable memory allocated by Limine.
 * (This is safe to do once we've set up our own page tables, since Limine
 * uses bootloader-reclaimable memory for the Limine page tables.
 */
void free_limine_bootloader_memory();

#endif