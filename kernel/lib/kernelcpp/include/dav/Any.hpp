#ifndef DAVOS_KERNELCPP_ANY_HPP
#define DAVOS_KERNELCPP_ANY_HPP

#include <typeinfo>


namespace dav {
class BadCast {};

class Any {
public:
private:
    std::type_info m_typeInfo;
    void* m_data;
    void* (*m_clone)(int);
};
} // namespace dav

#endif
