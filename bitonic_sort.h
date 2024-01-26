//
// Created by Arvid Jonasson on 2024-01-21.
//

#ifndef BITONIC_SORT_H
#define BITONIC_SORT_H

#include <cstdint>
#include <cstddef>

inline auto is_power_of_two(const std::size_t n) -> bool {
    return (n & (n - 1)) == 0;
}

void bitonic_sort(std::uint32_t *data, std::size_t n);
void bitonic_sort_simd(std::uint32_t *data, std::size_t n);

#endif //BITONIC_SORT_H
