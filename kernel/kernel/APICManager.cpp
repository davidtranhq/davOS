#include <kernel/APICManager.hpp>
#include <kernel/constants.h>
#include <kernel/frame_allocator.h>
#include <kernel/macros.h>
#include <kernel/paging.h>
#include <kernel/RootSDT.h>
#include <kernel/RootSystemDescriptionPointer.h>

void APICManager::initialize()
{
    DEBUG("Initializing APIC manager...\n");

    acpi::rsdp->validate();

    DEBUG("RSDP is valid.\n");

    const auto physicalRsdt = acpi::rsdp->rootSDTPhysicalAddress();
    const auto acpiRevision = acpi::rsdp->acpiRevision;

    DEBUG("RSDT acpi version: %d\n", acpiRevision);
    DEBUG("RSDT physical address: %p\n", physicalRsdt);

    // TODO: make this into a helper, paging_add_kernel_mapping
    
    const auto rsdt = reinterpret_cast<RootSDT *>(kernel_physical_to_virtual(physicalRsdt));
    paging_add_mapping(reinterpret_cast<uintptr_t>(rsdt), physicalRsdt, kernelConstants::pageSize, PageFlags::None);

    DEBUG("Locating MADT...\n");

    auto madt = rsdt->findSDTWithSignature<MADT>("APIC", acpiRevision);
    if (!madt)
        kernel_panic("MADT not found\n");

    DEBUG("Found MADT at %p\n", madt);

    auto apicStructures = reinterpret_cast<uint8_t*>(madt->apicStructures);
    auto end = reinterpret_cast<uint8_t*>(madt) + madt->header.length;
    while (apicStructures < end)
    {
        auto type = *apicStructures;
        auto length = *(apicStructures + 1);
        switch (type)
        {
        case 0:
        {
            auto localApicInfo = reinterpret_cast<LocalAPICInfo*>(apicStructures);
            m_localApicInfos[m_numLocalApics++] = *localApicInfo;
            break;
        }
        case 1:
        {
            auto ioApicInfo = reinterpret_cast<IOAPICInfo*>(apicStructures);
            m_ioApicInfos[m_numIoApics++] = *ioApicInfo;
            break;
        }
        default:
            break;
        }
        apicStructures += length;
    }
}
