#pragma once

#include <cstdint>
#include <concepts>
#include <kpp/array.hpp>
#include <kernel/ACPISDTHeader.h>
#include <kernel/frame_allocator.h>
#include <kernel/kernel.h>
#include <kpp/cstring.hpp>

template<typename T>
concept SystemDescriptorTable = requires(T t) {
    { t.header } -> std::convertible_to<ACPISDTHeader>;
};

struct RootSDT {
    ACPISDTHeader header;
    void* pointersToOtherSDTs;

    /**
     * @brief The number of system descriptor tables that are pointed to by this table.
     */
    std::size_t numPointersToOtherSDTs(int acpiRevisionNumber) const
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
        const auto numTables = numPointersToOtherSDTs(acpiRevisionNumber);
        for (std::size_t i = 0; i < numTables; ++i) {
            if (bytesPerPointer(acpiRevisionNumber) == 4) {
                auto sdt = reinterpret_cast<SDT*>(reinterpret_cast<const uint32_t*>(&pointersToOtherSDTs)[i]);
                if (!kpp::strncmp(sdt->header.signature, signature, 4))
                    return sdt;
            } else {
                auto sdt = reinterpret_cast<SDT*>(reinterpret_cast<const uint64_t*>(&pointersToOtherSDTs)[i]);
                if (!kpp::strncmp(sdt->header.signature, signature, 4))
                    return sdt;
            }
        }
        return nullptr;
    }

private:
    inline std::size_t bytesPerPointer(int acpiRevisionNumber) const
    {
        return acpiRevisionNumber == 0 ? sizeof(uint32_t) : sizeof(uint64_t);
    }
};
