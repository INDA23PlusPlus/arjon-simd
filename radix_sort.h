//
// Created by Arvid Jonasson on 2024-01-21.
//

#ifndef RADIX_SORT_H
#define RADIX_SORT_H

#include <cstdint>
#include <vector>
#include <iostream>


auto radix_sort(std::vector<uint32_t> &vec) -> void;

auto is_sorted(const std::vector<uint32_t> &vec) -> bool;

auto test_sort(auto func) -> bool {
    auto vec = std::vector<uint32_t>{};

    for (std::size_t i = 0; i < 100'000'000 * 4; ++i) {
        vec.push_back(rand());
    }

    func(vec);

    return is_sorted(vec);
}

auto radix_sort_neon(std::vector<uint32_t> &vec) -> void;

auto speed_test(auto func) -> void {
    auto vec = std::vector<uint32_t>{};

    for (std::size_t i = 0; i < 100'000'000 * 4; ++i) {
        vec.push_back(rand());
    }

    const auto start = std::chrono::high_resolution_clock::now();
    func(vec);
    const auto end = std::chrono::high_resolution_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms." << std::endl;
}

#endif //RADIX_SORT_H
