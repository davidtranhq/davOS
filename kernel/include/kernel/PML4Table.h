#ifndef DAVOS_KERNEL_PML4TABLE_H_INCLUDED
#define DAVOS_KERNEL_PML4TABLE_H_INCLUDED

#include <frg/array.hpp>

class PML4Table
{
public:
    enum class Flags : uint64_t
    {
        Write = 1 << 1, // allow writes
        User = 1 << 2, // allow user-mode access
        WriteThrough = 1 << 3, // use write-through caching policy
        CacheDisable = 1 << 4,
        ExecuteDisable = 1 << 63, // disable instruction fetches
    };

    void add_mapping(uint64_t base, uint64_t length, Flags flags);

private:
    PML4TableEntry entries_[512];
};

#endif