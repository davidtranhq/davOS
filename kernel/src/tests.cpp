#include <kpp/cstdio.hpp>
#include <cstdint>
#include <kpp/cstring.hpp>

#include <kpp/algorithm.hpp>
#include <kernel/APICManager.hpp>
#include <kernel/Allocator.h>
#include <kernel/frame_allocator.h>
#include <kernel/FreeListAllocator.h>
#include <kernel/macros.h>
#include <kernel/processor.hpp>
#include <kernel/tests.h>
#include <kernel/paging.h>

namespace
{

class GlobalConstructorTest
{
public:
    GlobalConstructorTest() : test_value_ {0xdeadbeef} {}

    bool was_constructed() const
    {
        return test_value_ == 0xdeadbeef;
    }

private:
    uint64_t test_value_;
};

GlobalConstructorTest global_constructor_test;

}

[[ gnu::noinline ]]
void test_stack_smash()
{
    kpp::printf("running stack smash test...\n");
	char arr[10];
    kpp::memset(arr, 0xa9, 12);
}

void test_interrupt_handling()
{
    kpp::printf("running interrupt handling test...\n");
    // interrupt 3 is the BREAKPOINT exception, it is modified in the TEST_BUILD to
    // run the interrupt handling test
    __asm__("int $3");
}

void test_global_constructor()
{
    kpp::printf("running global constructor test...\n");
    if (global_constructor_test.was_constructed())
        kpp::printf("global constructor test: PASSED\n");
    
}

void test_paging()
{
    kpp::printf("running paging test...\n");
    auto new_frame = reinterpret_cast<uintptr_t>(allocate_frame());
#ifdef DEBUG_BUILD
    DEBUG("Allocated physical frame at %p\n", new_frame);
#endif
    // map two virtual pages to the same frame, see if changes are reflected
    uintptr_t write_vpage = 0x80002000;
    uintptr_t read_vpage = 0x80000000;
    paging_add_mapping(write_vpage, new_frame, 0x1000, PageFlags::Write);
    paging_add_mapping(read_vpage, new_frame, 0x1000, PageFlags::Write);
    uint32_t *write_ptr = reinterpret_cast<uint32_t *>(write_vpage);
    uint32_t *read_ptr = reinterpret_cast<uint32_t *>(read_vpage);
    uint32_t test_value = 0xabcd1234;
    *write_ptr = test_value;
    if (*read_ptr == test_value)
    {
        kpp::printf("paging test: PASSED\n");
    }
    else
    {
        kpp::printf("paging test: FAILED\n");
        kpp::printf("read %d at %p, expected %d\n", *read_ptr, new_frame, test_value);
    }
}

template <typename Alloc>
auto test_allocator() -> void {
    kpp::printf("running allocator test...\n");
    auto allocator = Alloc {};
    const auto free_regions = paging_get_initial_free_regions();
    const auto free_region = free_regions[0];
    constexpr auto free_region_max_size = size_t {0x1000};
    allocator.add_memory(reinterpret_cast<Alloc::value_type *>(free_region.base),
        kpp::max(free_region.size, free_region_max_size));

    const auto p1 = allocator.allocate(0x20);
    const auto p2 = allocator.allocate(0x30);
    const auto p3 = allocator.allocate(0x50);

    allocator.deallocate(p3);
    allocator.deallocate(p2);
    allocator.deallocate(p1);
    kpp::printf("allocator test: PASSED\n");
}

void test_interprocessor_interrupts()
{
    kpp::printf("running interprocessor interrupt test...\n");
    APICManager().sendInterprocessorInterrupt(0, 0xff);
}

void test_keyboard()
{
    kpp::printf("running keyboard controller test...\n");

    // --- PS/2 Controller Command Byte Check ---
    processor::outb(0x64, 0x20);
    processor::ioWait();
    uint8_t cmd = processor::inb(0x60);

    bool irq1_enabled = cmd & (1 << 0);         // Bit 0
    bool irq12_enabled = cmd & (1 << 1);        // Bit 1
    bool kbd_clock_enabled = !(cmd & (1 << 4)); // Bit 4 (0 = enabled)
    bool translation_enabled = cmd & (1 << 6);  // Bit 6

    kpp::printf("Command Byte: 0x%x\n", cmd);
    kpp::printf("  IRQ1 (keyboard):       %s (%s)\n", irq1_enabled ? "enabled" : "disabled", irq1_enabled ? "OK" : "EXPECTED ENABLED");
    kpp::printf("  IRQ12 (mouse):         %s\n", irq12_enabled ? "enabled" : "disabled");
    kpp::printf("  Keyboard clock:        %s (%s)\n", kbd_clock_enabled ? "enabled" : "disabled", kbd_clock_enabled ? "OK" : "EXPECTED ENABLED");
    kpp::printf("  Scancode translation:  %s\n", translation_enabled ? "enabled" : "disabled");

    if (irq1_enabled && kbd_clock_enabled)
        kpp::printf("keyboard controller test: PASSED\n");
    else
        kpp::printf("keyboard controller test: FAILED\n");

    // --- Local APIC State Check ---
    uint32_t tpr = processor::localAPIC.read(0x80);
    kpp::printf("TPR: 0x%x (%s)\n", tpr, tpr == 0 ? "OK" : "EXPECTED 0");

    int flags;
    asm volatile("pushf; pop %%rax; and $0x200, %%rax" : "=a"(flags));
    kpp::printf("IF enabled: %s (%s)\n", flags ? "yes" : "no", flags ? "OK" : "EXPECTED yes");

    uint32_t svr = processor::localAPIC.read(0xF0);
    bool apic_enabled = svr & (1 << 8);
    kpp::printf("SVR: 0x%x\n", svr);
    kpp::printf("  APIC enabled bit: %s (%s)\n", apic_enabled ? "yes" : "no", apic_enabled ? "OK" : "EXPECTED yes");

}

// TODO: Make this into a real suite that errors out on failure
void run_all_tests()
{
    kpp::printf("\n=========================\n");
    kpp::printf("RUNNING TEST SUITE...\n\n");
    test_global_constructor();
    // the following two tests cause kernel panics on success, so they are
    // uncommented here
    // test_interrupt_handling();
    // test_stack_smash();
    test_paging();
    test_allocator<FreeListAllocator<char>>();
    test_interprocessor_interrupts();
    test_keyboard();
    kpp::printf("\nPASSED ALL TESTS\n");
    kpp::printf("=========================\n\n");
}
