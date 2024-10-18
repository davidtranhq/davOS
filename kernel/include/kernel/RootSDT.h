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
    uint32_t* pointersToOtherSDTs;

    /**
     * @brief The number of system descriptor tables that are pointed to by this table.
     */
    std::size_t pointersToOtherSDTsSize() const
    {
        return (header.length - sizeof(header)) / sizeof(uint32_t);
    }

    /**
     * @brief Find a system descriptor table with a given 4-byte signature.
     */
    template<SystemDescriptorTable SDT>
    SDT* findSDTWithSignature(const char* signature) const
    {
        const auto numTables = pointersToOtherSDTsSize();
        for (std::size_t i = 0; i < numTables; ++i) {
            const auto sdt = reinterpret_cast<SDT*>(pointersToOtherSDTs[i]);
            if (!strncmp(sdt->header.signature, signature, 4))
                return sdt;
        }
        return nullptr;
    }
};

struct ExtendedSDT {
    ACPISDTHeader header;
    uint64_t* pointersToOtherSDTs;

    /**
     * @brief The number of system descriptor tables that are pointed to by this table.
     */
    std::size_t pointersToOtherSDTsSize() const
    {
        return (header.length - sizeof(header)) / sizeof(uint64_t);
    }


    /**
     * @brief Find a system descriptor table with a given 4-byte signature.
     */
    template<SystemDescriptorTable SDT>
    SDT* findSDTWithSignature(const char* signature) const
    {
        const auto numTables = pointersToOtherSDTsSize();
        for (std::size_t i = 0; i < numTables; ++i) {
            const auto sdt = reinterpret_cast<SDT*>(pointersToOtherSDTs[i]);
            if (!strncmp(sdt->header.signature, signature, 4))
                return sdt;
        }
        return nullptr;
    }
};
