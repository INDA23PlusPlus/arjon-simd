#include "radix_sort.h"

#include <arm_neon.h>
#include <array>
#include <ranges>
#include <chrono>
#include <iostream>

auto radix_sort(std::vector<uint32_t> &vec) -> void {
    auto output = std::vector<uint32_t>(vec.size());
    auto count = std::array<uint32_t, 256>{};

    for (std::uint_fast8_t shift = 0; shift <= 24; shift += 8) {
        // Reset count
        count.fill(0);

        // Count the number of times each value appears
        for (const auto &i: vec) {
            ++count[(i >> shift) & 0xFF];
        }

        // Compute prefix sum
        for (std::uint_fast16_t i = 1; i < 256; ++i) {
            count[i] += count[i - 1];
        }

        // Build output array
        for (const auto &x: std::ranges::reverse_view(vec)) {
            output[--count[(x >> shift) & 0xFF]] = x;
        }

        vec = output;
    }
}

auto radix_sort_neon(std::vector<uint32_t> &vec) -> void {
    auto output = std::vector<uint32_t>(vec.size());
    auto count = std::array<uint32_t, 256>{};

    auto iteration = [&vec, &output, &count]<std::uint_fast8_t S>() {
        // Reset count
        count.fill(0);

        // Count the number of times each value appears
        for (std::size_t i = 0; i < vec.size(); i += 4) {
            uint32x4_t data = vld1q_u32(&vec[i]);
            if constexpr (S != 0) {
                data = vshrq_n_u32(data, S);
            }
            uint32x4_t masked = vandq_u32(data, vdupq_n_u32(0xFF));
            uint32x2_t low = vget_low_u32(masked);
            uint32x2_t high = vget_high_u32(masked);
            count[vget_lane_u32(low, 0)]++;
            count[vget_lane_u32(low, 1)]++;
            count[vget_lane_u32(high, 0)]++;
            count[vget_lane_u32(high, 1)]++;
        }

        // Compute prefix sum
        for (std::uint_fast16_t i = 1; i < 256; ++i) {
            count[i] += count[i - 1];
        }

        // Build output array
        for (const auto &x: std::ranges::reverse_view(vec)) {
            output[--count[(x >> S) & 0xFF]] = x;
        }

        vec = output;
    };

    iteration.operator()<0>();
    iteration.operator()<8>();
    iteration.operator()<16>();
    iteration.operator()<24>();
}


auto is_sorted(const std::vector<uint32_t> &vec) -> bool {
    for (std::size_t i = 1; i < vec.size(); ++i) {
        if (vec[i - 1] > vec[i]) {
            return false;
        }
    }

    return true;
}

