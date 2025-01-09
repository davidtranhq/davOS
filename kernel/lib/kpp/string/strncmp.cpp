#include <kpp/cstring.hpp>

int kpp::strncmp(const char* a, const char* b, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        if (a[i] != b[i])
            return a[i] - b[i];
    }
    return 0;
}
