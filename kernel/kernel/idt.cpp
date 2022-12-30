#include <string.h>

#include <kernel/idt.h>
#include <kernel/IDT.h>
#include <kernel/IDTDescriptor.h>
#include <kernel/kernel.h>
#include <kernel/SegmentSelector.h>

namespace
{
    IDT idt;
    IDTDescriptor idt_descriptor(IDT::size, idt.address());
}

__attribute__((interrupt))
void isr_divide_by_zero(void *interrupt_frame)
{
    kernel_panic("divide by zero\n");
}

void idt_init()
{
    idt.load_gate_descriptor(
        0,
        isr_divide_by_zero,
        SegmentSelector(PrivilegeLevel::kernel, DescriptorTable::global, 5),
        0,
        IDT::GateType::interrupt,
        PrivilegeLevel::kernel
    );

    // load the address of the IDT Descriptor into the IDTR (IDT register)
    __asm__("lidt %0" :: "m"(*idt_descriptor.address()));

}