#include <kpp/cstdio.hpp>
#include <kpp/cstring.hpp>

#include <kernel/APICManager.hpp>
#include <kernel/frame_allocator.h>
#include <kernel/kernel.h>
#include <kernel/KeyboardBuffer.hpp>
#include <kernel/limine.h>
#include <kernel/macros.h>
#include <kernel/tests.h>
#include <kernel/Terminal.hpp>
#include <kernel/types.h> // LinkerAddress

extern LinkerAddress kernel_stack_start;

[[ noreturn ]]
void kernel_main()
{
    [[ maybe_unused ]]
    volatile int x = 0;
    kernel_init();
    DEBUG("Stack initialized to %x\n", &kernel_stack_start);
    run_all_tests();
    kpp::printf("Hello, world!\n");
    
    kpp::printf("\n");

    auto handle_command = [](const char* input, int length) {
        if (length == 0) {
            return;
        }
        if (kpp::strncmp(input, "help", 4) == 0) {
            kpp::printf("Available commands:\n");
            kpp::printf("  help - Show this help message\n");
            kpp::printf("  echo <message> - Echo the message back\n");
            kpp::printf("  exit - Exit the shell\n");
        } else if (kpp::strncmp(input, "echo ", 5) == 0) {
            kpp::printf("%s\n", input + 5);
        } else if (kpp::strncmp(input, "exit", 4) == 0) {
            kpp::printf("Exiting shell...\n");
            kernel_hang();
        } else if (kpp::strncmp(input, "clear", 5) == 0) {
            KernelTerminal::instance->clear();
        } else {
            kpp::printf("Unknown command: %s\n", input);
        }
    };

    char input[256];
    int pos = 0;
    using Key = KeyboardBuffer::Key;
    while (true) {
        pos = 0;
        kpp::printf("davOS> ");
        while (true) {
            Key key = kernel::keyboardBuffer.get();
            if (key == Key::enter) {
                kpp::putchar('\n');
                input[pos] = '\0';
                break;
            } else if (key == Key::backspace) {
                if (pos > 0) {
                    pos--;
                    kpp::printf("\b \b");
                }
            } else if (key == Key::LeftArrow) {
                if (pos > 0) {
                    pos--;
                }
            } else if (key == Key::RightArrow) {
                if (pos < (int)sizeof(input) - 1 && input[pos] != '\0') {
                    pos++;
                }
            } else if (key == Key::UpArrow || key == Key::DownArrow) {
                KernelTerminal::instance->scrollDown(key == Key::UpArrow ? -1 : 1);
            } else if (key != Key::None && pos < (int)sizeof(input) - 1) {
                input[pos++] = static_cast<char>(key);
                kpp::putchar(static_cast<char>(key));
            }
        }
        handle_command(input, pos);
    }

    kernel_hang();
}

extern "C"
[[ noreturn ]]
void kernel_pre_main()
{
    // Replace Limine's stack with our own so that we can reclaim bootloader-reclaimable memory.
    // It's fine that we throw away the old stack contents since there are no stack variables
    // in this function and we don't plan on returning to it (kernel_main() is noreturn)
    __asm__("mov %%rsp, %0" :: "r"(&kernel_stack_start));

    kernel_main();
}
