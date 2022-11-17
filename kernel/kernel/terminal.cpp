#include <kernel/limine.h>
#include <kernel/terminal.h>

namespace
{

// Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent.
volatile struct limine_terminal_request terminal_request =
{
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0
};

struct limine_terminal *terminal = NULL;

} // anonymous namespace


void terminal_init()
{
    if (terminal_request.response == NULL
        || terminal_request.response->terminal_count < 1)
    {
        // hang if the terminal failed to initialize
        for (;;)
            __asm__("hlt");
    }

    terminal = terminal_request.response->terminals[0];
}

void terminal_write(const char *str, size_t size)
{
    terminal_request.response->write(terminal, str, size);
}