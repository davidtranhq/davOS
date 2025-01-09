#include <kpp/cstdlib.hpp>
#include <kpp/cstdio.hpp>

[[noreturn]]
void kpp::abort(void)
{
    kpp::printf("abort()\n");
    __builtin_trap();
    while (1) {
    }
    __builtin_unreachable();
}
