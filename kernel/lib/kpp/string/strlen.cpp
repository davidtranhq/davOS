#include <kpp/cstring.hpp>

size_t kpp::strlen(const char* str)
{
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}
