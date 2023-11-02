#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <kernel/frame_allocator.h>
#include <kernel/tests.h>
#include <kernel/vmm.h>

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
    printf("running stack smash test...\n");
	char arr[10];
	memset(arr, 0xa9, 12);
}

void test_interrupt_handling()
{
    printf("running interrupt handling test...\n");
    // interrupt 3 is the BREAKPOINT exception, it is modified in the TEST_BUILD to
    // run the interrupt handling test
    __asm__("int $3");
}

void test_global_constructor()
{
    printf("running global constructor test...\n");
    if (global_constructor_test.was_constructed())
        printf("global constructor test: PASSED\n");
    
}

void test_paging()
{
    printf("running paging test...\n");
    auto new_frame = reinterpret_cast<uintptr_t>(allocate_frame());
#ifdef DEBUG_BUILD
    DEBUG("Allocated physical frame at %p\n", new_frame);
#endif
    // map two virtual pages to the same frame, see if changes are reflected
    uintptr_t write_vpage = 0x80002000;
    uintptr_t read_vpage = 0x80000000;
    vmm_add_mapping(write_vpage, new_frame, 0x1000, PageFlags::Write);
    vmm_add_mapping(read_vpage, new_frame, 0x1000, PageFlags::Write);
    uint32_t *write_ptr = reinterpret_cast<uint32_t *>(write_vpage);
    uint32_t *read_ptr = reinterpret_cast<uint32_t *>(read_vpage);
    uint32_t test_value = 0xabcd1234;
    *write_ptr = test_value;
    if (*read_ptr == test_value)
    {
        printf("paging test: PASSED\n");
    }
    else
    {
        printf("paging test: FAILED\n");
        printf("read %d at %p, expected %d\n", *read_ptr, new_frame, test_value);
    }
    
}

void run_all_tests()
{
    test_global_constructor();
    test_interrupt_handling();
    test_stack_smash();
    test_paging();
    printf("\nFINISHED ALL TESTS\n");
    printf("----------\n\n");
}