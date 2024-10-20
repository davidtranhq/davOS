#pragma once

#include <cstdint>
#include <dav/array.hpp>
#include <dav/concepts.hpp>
#include <kernel/ACPISDTHeader.h>

template<typename T>
concept SystemDescriptorTable = requires(T t) {
    { t.header } -> dav::Convertible<ACPISDTHeader>;
};

struct RootSDT {
    ACPISDTHeader header;
    void* pointersToOtherSDTs;

    /**
     * @brief The number of system descriptor tables that are pointed to by this table.
     */
    std::size_t pointersToOtherSDTsSize(int acpiRevisionNumber) const
    {
        return (header.length - sizeof(header)) / bytesPerPointer(acpiRevisionNumber);
    }

    /**
     * @brief Find a system descriptor table with a given 4-byte signature.
     *
     * The address returned is the physical address of the table.
     */
    template<SystemDescriptorTable SDT>
    SDT* findSDTWithSignature(const char* signature, int acpiRevisionNumber) const
    {
        const auto numTables = pointersToOtherSDTsSize(acpiRevisionNumber);
        for (std::size_t i = 0; i < numTables; ++i) {
            const auto sdt = reinterpret_cast<SDT*>(static_cast<char *>(pointersToOtherSDTs)[i * bytesPerPointer(acpiRevisionNumber)]);
            if (!strncmp(sdt->header.signature, signature, 4))
                return sdt;
        }
        return nullptr;
    }

private:
    inline std::size_t bytesPerPointer(int acpiRevisionNumber) const
    {
        return acpiRevisionNumber == 0 ? sizeof(uint32_t) : sizeof(uint64_t);
    }
};
