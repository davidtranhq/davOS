#include <kernel/GateDescriptor.h>
#include <kernel/idt.h>
#include <kernel/SegmentSelector.h>

void idt_init()
{
    constexpr auto selector = SegmentSelector(PrivilegeLevel::user, DescriptorTable::local, 0xabc0);
    
    constexpr auto descriptor = GateDescriptor(
        0xabcd1234abcd1234,
        selector,
        5,
        GateDescriptor::GateType::trap,
        PrivilegeLevel::user
    ).to_bytes();

    // TEST: check that SegmentSelector and GateDescriptor are indeed compile-time constructs
    [[ maybe_unused ]]
    constexpr auto sum = descriptor[0] + descriptor[1] + descriptor[2] + descriptor[3];
}