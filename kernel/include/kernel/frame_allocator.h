#ifndef DAVOS_KERNEL_FRAME_ALLOCATOR_H_INCLUDED
#define DAVOS_KERNEL_FRAME_ALLOCATOR_H_INCLUDED

#include <cstdint>

struct PhysicalFrame;

auto frame_allocator_init() -> void;

/**
 * @brief Get a new physical frame. Returns the physical address of the new frame.
 */
auto allocate_frame() -> void *;

/**
 * @brief Frees a previously allocated physical frame. Do not deallocate the same frame twice
 * (the function does not check for this).
 */
auto deallocate_frame(void *frame_to_deallocate) -> void;

// TODO: these functions don't belong here
/**
 * @brief Get a pointer pointing to the corresponding virtual address of a kernel physical address.
 */
auto kernel_physical_to_virtual(void *physical_address) -> void *;

/**
 * @brief Get the virtual address of the given physical kernel address.
 */
auto kernel_physical_to_virtual(uintptr_t physical_address) -> uintptr_t;

/**
 * @brief Reclaim bootloader-reclaimable memory allocated by Limine.
 * (This is safe to do once we've set up our own page tables, since Limine
 * uses bootloader-reclaimable memory for the Limine page tables.
 */
auto free_limine_bootloader_memory() -> void;

/**
 * @brief Print the base, limit, and mapping type of each of the initial Limine memory map
 */
auto print_memory_map() -> void;

auto available_frames() -> std::size_t;

/**
 * @brief Update the ref count for the given frame by change.
 */
auto update_frame_ref_count(uintptr_t frame, int change) -> void;

#endif
