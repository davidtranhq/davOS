#ifndef KERNELCPP_FUNCTIONAL_HPP
#define KERNELCPP_FUNCTIONAL_HPP

namespace dav {

template <typename T = void>
struct less {
    constexpr auto operator()(const T &lhs, const T &rhs) const {
        return lhs < rhs;
    }
};

}

#endif