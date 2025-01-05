#include <kernel/APICManager.hpp>
#include <kernel/constants.h>
#include <kernel/frame_allocator.h>
#include <kernel/limine_features.h>
#include <kernel/macros.h>
#include <kernel/paging.h>
#include <kernel/processor.hpp>
#include <kernel/RootSDT.h>
#include <kernel/RootSystemDescriptionPointer.h>

void APICManager::initialize()
{
    // TODO: Need to fix frame_allocator so that we can reserve the frames containing the APIC tables.

    // Use the RSDP to find the RSDT to find the MADT
    rsdp = reinterpret_cast<RootSystemDescriptionPointer*>(limine::rsdp_address->address);
    if (!rsdp)
        kernel_panic("Could not find RSDP. Maybe ACPI is not supported?\n");
    const auto acpiRevision = rsdp->acpiRevision;
    const auto physicalRsdt = rsdp->rootSDTPhysicalAddress();
    // TODO: make this into a helper, paging_add_kernel_mapping
    const auto rsdt = reinterpret_cast<RootSDT *>(kernel_physical_to_virtual(physicalRsdt));
    // TODO why don't we need to map this?
    /*paging_add_mapping(reinterpret_cast<uintptr_t>(rsdt), physicalRsdt, kernelConstants::pageSize, PageFlags::None);*/
    const auto physicalMadt = rsdt->findSDTWithSignature<MADT>("APIC", acpiRevision);
    if (!physicalMadt)
        kernel_panic("MADT not found\n");
    const auto madt = reinterpret_cast<MADT *>(kernel_physical_to_virtual(physicalMadt));
    /*paging_add_mapping(reinterpret_cast<uintptr_t>(madt), reinterpret_cast<uintptr_t>(physicalMadt), kernelConstants::pageSize, PageFlags::None);*/

    // Parse the MADT, a table of contiguous entries of different types, each storing information about an APIC
    auto apicStructures = reinterpret_cast<uint8_t*>(&madt->apicStructures);
    auto end = reinterpret_cast<uint8_t*>(madt) + madt->header.length;
    while (apicStructures < end)
    {
        auto type = *apicStructures;
        auto length = *(apicStructures + 1);
        apicStructures += 2;
        switch (type) {
        case 0: {
            [[ maybe_unused ]]
            auto localApicInfo = reinterpret_cast<LocalAPICInfo*>(apicStructures);
            // TODO: parse and save these
            break;
        }
        case 1: {
            auto ioApicInfo = reinterpret_cast<IOAPICInfo*>(apicStructures);

            // The registers of the IO APIC are memory-mapped, so we need to map the physical address to a virtual address.
            // We also need to make sure that the cache policy for the page is "strong uncacheable" (i.e. cache disabled)
            // for writing to the registers to work correctly (see Intel SDM Vol. 3A 10.4.1)
            paging_add_mapping(reinterpret_cast<uintptr_t>(kernel_physical_to_virtual(ioApicInfo->ioApicAddress)), ioApicInfo->ioApicAddress, kernelConstants::pageSize, PageFlags::Write | PageFlags::CacheDisable);
            if (m_numIoApics >= m_ioApics.size())
                kernel_panic("Too many IO APICs\n");
            m_ioApics[m_numIoApics++] = IOAPIC(kernel_physical_to_virtual(ioApicInfo->ioApicAddress), ioApicInfo->globalSystemInterruptBase, ioApicInfo->ioApicId);
            break;
        }
        case 2: {
            // TODO: do we need this?
            [[ maybe_unused ]]
            auto ioApicInterruptSourceOverride = reinterpret_cast<IOAPICInterruptSourceOverride*>(apicStructures);
        }
        default:
            break;
        }
        apicStructures += length - 2;
    }
}

APICManager::IOAPIC::IOAPIC(uint32_t address, uint32_t globalSystemInterruptBase, uint8_t id)
    : m_address {address},
      m_globalSystemInterruptBase {globalSystemInterruptBase},
      m_id {id}
{
      m_numRedirectionEntries = static_cast<uint8_t>((readRegister(1) >> 16) & 0xff);
}

void APICManager::IOAPIC::writeRegister(uint8_t registerNum, uint32_t value) const
{
    *reinterpret_cast<volatile uint32_t*>(m_address) = registerNum;
    *reinterpret_cast<volatile uint32_t*>(m_address + 0x10) = value;
}

uint32_t APICManager::IOAPIC::readRegister(uint8_t registerNum) const
{
    *reinterpret_cast<volatile uint32_t*>(m_address) = registerNum;
    return *reinterpret_cast<volatile uint32_t*>(m_address + 0x10);
}

void APICManager::redirectIrq(uint8_t irq, uint8_t vector)
{
    for (const auto& ioApic : m_ioApics) {
        if (!ioApic.handlesIRQ(irq))
            continue;
        auto low = ioApic.readRedirectionEntryLow(irq);
        auto high = ioApic.readRedirectionEntryHigh(irq);
        low &= 0;
        low |= vector;
        high &= 0;
        ioApic.writeRedirectionEntryLow(irq, low);
        ioApic.writeRedirectionEntryHigh(irq, high);
        DEBUG("Redirected IRQ %d to vector %d. New low: %x, new high: %x\n", irq, vector, ioApic.readRedirectionEntryLow(irq), ioApic.readRedirectionEntryHigh(irq));
        return;
    }
}

void APICManager::sendInterprocessorInterrupt(uint8_t destination, uint8_t vector)
{
    // Bits 0-7 of the low register (0x300) specify the vector of the interrupt
    // Bits 14 should be set, since if it is cleared it signifies an INIT level de-assert
    // (i.e. the end-of-interrupt signal for an interprocessor-initialization interrupt)
    auto low = 0x4000 | vector;
    
    // Bits 24-27 of the high register (0x310) specify the APIC ID of the destination processor
    auto high = destination << 24;

    // The interrupt is issued when register 0x300 is written to, so we write to 0x310 first
    const uintptr_t localAPICAddress = kernel_physical_to_virtual(processor::localAPICBaseAddress());
    *reinterpret_cast<volatile uint32_t*>(localAPICAddress + 0x310) = high;
    *reinterpret_cast<volatile uint32_t*>(localAPICAddress + 0x300) = low;
}
