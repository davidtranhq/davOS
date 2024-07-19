#include <kernel/FreeListAllocator.h>
#include <kernel/macros.h>
#include <cstdint>
#include <new>

// template <typename T>
// FreeListAllocator<T>::FreeListAllocator(T *base, size_t size)
//     : head(new(base) FreeBlockHeader {size})
// {}

template <typename T>
auto FreeListAllocator<T>::allocate_impl(size_t request) -> T * {
    // First fit
    auto curr = head;
    while (curr && !curr->can_fit(request)) {
        curr = curr->next;
    }
    if (!curr) {
        kernel_panic("ran out of virtual memory!\n");
    }
    auto allocated = static_cast<void *>(nullptr);
    if (curr->perfect_fit(request)) {
        allocated = curr->reserve_entire(request);
        if (curr == head) {
            head = curr->next;
        }
    } else {
        allocated = curr->reserve_end(request);
    }

#ifdef DEBUG_BUILD_FREE_LIST_ALLOCATOR
    DEBUG("allocate: %x\n", request);
    head->print_free_list();
#endif

    return reinterpret_cast<T *>
        (reinterpret_cast<UsedBlockHeader *>(allocated) + 1);
}

template <typename T>
auto FreeListAllocator<T>::deallocate_impl(T *base, size_t size) -> void {
    auto curr = head;
    auto prev = static_cast<FreeBlockHeader *>(nullptr);
    auto used_block = reinterpret_cast<FreeListAllocator<T>::UsedBlockHeader *>(base) - 1;
    if (used_block->magic != UsedBlockHeader::magic_constant) {
        kernel_panic("invalid memory free\n");
    }
    while (curr && reinterpret_cast<uintptr_t>(curr) < reinterpret_cast<uintptr_t>(used_block)) {
        prev = curr;
        curr = curr->next;
    }
    auto freed = new(used_block) FreeBlockHeader
        {used_block->size + sizeof(UsedBlockHeader) - sizeof(FreeBlockHeader), curr, prev};
    
    
    if (!prev) {
        head = freed;
    } else {
        prev->next = freed;
    }
    if (curr) {
        curr->prev = freed;
    }

    freed->coalesce_next();
    freed->coalesce_prev();

#ifdef DEBUG_BUILD
    DEBUG("deallocate: %p, %x\n", base, size);
    head->print_free_list();
#endif
}

template <typename T>
auto FreeListAllocator<T>::add_memory_impl(T *base, size_t size) -> void {
    DEBUG("head: %x\n", head);
    DEBUG("adding memory: %p\n", base);
    auto allocated = new(base) UsedBlockHeader {size};
    allocated += 1;
    deallocate_impl(reinterpret_cast<T *>(allocated), 0);
}

template <typename T>
auto FreeListAllocator<T>::get_size_impl(T *base) -> size_t {
    auto header = reinterpret_cast<UsedBlockHeader *>(base) - 1;
    return header->size;
}

template <typename T>
struct FreeListAllocator<T>::FreeBlockHeader {
    size_t size {};
    FreeBlockHeader* next = nullptr;
    FreeBlockHeader* prev = nullptr;

    auto can_fit(size_t request) -> bool const {
        return request + sizeof(UsedBlockHeader) <= size;
    }

    auto perfect_fit(size_t request) -> bool const {
        return request + sizeof(UsedBlockHeader) == size;
    }

    auto real_size() -> size_t const {
        return size + sizeof(FreeBlockHeader);
    }

    auto unlink() -> void {
        if (prev) prev->next = next;
        if (next) next->prev = prev;
    }

    auto reserve_end(size_t request) -> UsedBlockHeader * {
        auto allocation_size = request + sizeof(UsedBlockHeader);
        auto free_block_end = reinterpret_cast<uintptr_t>(this) + sizeof(FreeBlockHeader) + size;
        auto allocation_address = reinterpret_cast<void *>(free_block_end - allocation_size);
        auto allocated = new(allocation_address) UsedBlockHeader{request};
        size -= allocation_size;
        return allocated;
    }

    auto reserve_entire(size_t request) -> UsedBlockHeader * {
        unlink();
        auto allocated = new(this) UsedBlockHeader{request};
        return allocated;
    }

    auto coalesce_next() -> void {
        if (next && reinterpret_cast<uintptr_t>(next)
                    == reinterpret_cast<uintptr_t>(this) + real_size()) {
            size += next->real_size();
            next = next->next;
        }
    }

    auto coalesce_prev() -> void {
        if (prev && reinterpret_cast<uintptr_t>(this) 
                    == reinterpret_cast<uintptr_t>(prev) + prev->real_size()) {
            prev->size += real_size();
            prev->next = next;
        }
    }

#ifdef DEBUG_BUILD
    auto print_free_list() -> void {
        DEBUG("\n====FREE LIST======\n");
        auto curr = this;
        while (curr) {
            DEBUG("%p: {size: %x, next: %p, prev: %p}\n", curr, curr->size, curr->next, curr->prev);
            curr = curr->next;
        }
    }
#endif
};

template <typename T>
struct FreeListAllocator<T>::UsedBlockHeader {
    static constexpr uint64_t magic_constant = 0x11223344aabbccdd;

    size_t size {};
    uint64_t magic {magic_constant};
};
