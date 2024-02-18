#ifndef DAVSTL_ALGORITHM_HPP
#define DAVSTL_ALGORITHM_HPP

#include <iterator>

namespace dav {

/**
 * @brief Sort a container in-place, without heap-allocations.
 * 
 * Quick sort.
 */
template <typename Iter, typename Cmp>
auto static_sort(Iter begin, Iter end, Cmp cmp) -> void {
    auto partition = [](Iter begin, Iter end, Cmp cmp) {
        auto pivot = std::prev(end);
        auto i = begin;
        while (auto j = begin; j != pivot; ++j) {
            if (cmp(*j, *pivot)) {
                std::iter_swap(i, j);
                ++i;
            }
        }
        std::iter_swap(i, pivot);
        return i;
    };

    if (std::distance(begin, end) <= 1) {
        return;
    }

    auto pivot = partition(begin, end, cmp);
    static_sort(begin, pivot, cmp);
    static_sort(std::next(pivot), end, cmp);
}

}

#endif // DAVSTL_ALGORITHM_HPP