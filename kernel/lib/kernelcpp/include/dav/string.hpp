#ifndef KERNELCPP_STRING_HPP_INCLUDED
#define KERNELCPP_STRING_HPP_INCLUDED

#include <cstddef>
#include <string.h>

namespace dav {

/**
 * @brief A dynamically-allocated, null-terminated string.
 */
class string {
public:
    string(const char *str) {
        
    }

private:
    char *data;
    size_t size;
};

}

#endif