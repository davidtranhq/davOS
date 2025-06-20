#include <cstdint>
#include <kernel/constants.h>
#include <kernel/frame_allocator.h>
#include <kernel/kernel.h>
#include <kernel/LocalAPIC.hpp>
#include <kernel/paging.h>
#include <kernel/processor.hpp>

namespace {
    constexpr uint32_t LAPIC_SPURIOUS_REGISTER = 0xF0;
    constexpr uint32_t LAPIC_EOI_REGISTER = 0xB0;
    constexpr uint32_t LAPIC_TASK_PRIORITY = 0x80;
}

void LocalAPIC::enableAPIC()
{
    const auto physicalAPICAddress = processor::localAPICBaseAddress();
    const auto virtualAPICAddress = kernel_physical_to_virtual(physicalAPICAddress);

    kernel_assert(processor::hasPAT(), "Processor does not support PAT");

    // Map the Local APIC into the kernel's virtual address space with appropriate flags
    paging_add_mapping(virtualAPICAddress, physicalAPICAddress, kernelConstants::pageSize,
                       PageFlags::Write | PageFlags::CacheDisable);

    baseAddress = reinterpret_cast<volatile uint32_t*>(virtualAPICAddress);

    // Program the APIC base MSR and enable the hardware APIC
    processor::hardwareEnableLocalAPICAndSetBaseAddress(physicalAPICAddress);

    // In addition to disabling the PIC, we also remap the PIC1 and PIC2 interrupt requests 
    // to start at 0x20 and 0x28 respectively instead of 0x8 and 0x70. This is done to avoid
    // conflicts with the local APIC. Even though the PIC is disabled, it
    // could still give out spurious interrupts, which would conflict with system exceptions.
    processor::disablePIC();
    processor::remapPIC(0x20, 0x28);

    // Enable interrupts globally
    asm volatile("sti");

    // The spurious interrupt vector is the vector that the local APIC will use to signal interrupts
    // that are not associated with a specific interrupt vector. This is typically set to 0xFF.
    // The 16th bit of the spurious interrupt vector is the APIC software enable bit.
    // This bit must be set to 1 to enable the local APIC.
    write(LAPIC_SPURIOUS_REGISTER, 0x1FF);

    // Set the task priority to 0 to allow all interrupts to be delivered.
    write(LAPIC_TASK_PRIORITY, 0);
}

void LocalAPIC::sendEndOfInterrupt()
{
    // Send an End of Interrupt (EOI) signal to the local APIC.
    // This tells the local APIC that the current interrupt has been handled.
    write(LAPIC_EOI_REGISTER, 0);
}

void LocalAPIC::write(uint32_t regOffset, uint32_t value)
{
    *(volatile uint32_t*)((uintptr_t)baseAddress + regOffset) = value;
}

uint32_t LocalAPIC::read(uint32_t regOffset)
{
    return *(volatile uint32_t*)((uintptr_t)baseAddress + regOffset);
}
