#include <string.h>

#include <kernel/GateDescriptor.h>
#include <kernel/idt.h>
#include <kernel/kernel.h>
#include <kernel/SegmentSelector.h>

__attribute__((interrupt))
void isr_divide_by_zero(void *interrupt_frame)
{
    kernel_panic("divide by zero\n");
}

void idt_init()
{
    GateDescriptor de_descriptor(
        reinterpret_cast<uint64_t>(&isr_divide_by_zero),
        SegmentSelector(
            PrivilegeLevel::kernel,
            DescriptorTable::global,
            0
        ),
        0,
        GateDescriptor::GateType::interrupt,
        PrivilegeLevel::kernel
    );

    void *idt_base = reinterpret_cast<void *>(0x1000);
    memcpy(idt_base, de_descriptor.to_bytes(), GateDescriptor::NUM_BYTES);

    // WRONG: this should be a linear address
    __asm__("lidt 0x1000");
}