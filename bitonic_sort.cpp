//
// Created by Arvid Jonasson on 2024-01-21.
//

#include "bitonic_sort.h"

#include <arm_neon.h>
#include <stdexcept>
#include <thread>
#include "thread_pool.h"
#include "arm_neon.h"

static auto bitonic_sort_merge(ThreadPool &pool, std::uint32_t *data, const std::size_t low, const std::size_t count, const bool ascending) -> void {
    if(count > 1) {
        const std::size_t mid = count / 2;

        for(std::size_t i = low; i < low + mid; ++i) {
            if(ascending == (data[i] > data[i + mid])) {
                std::swap(data[i], data[i + mid]);
            }
        }

        auto left = pool.enqueue(bitonic_sort_merge, std::ref(pool), data, low, mid, ascending);

        if (!left.has_value()) {
            bitonic_sort_merge(pool, data, low, mid, ascending);
        }

        bitonic_sort_merge(pool, data, low + mid, mid, ascending);

        if(left.has_value()) {
            left->get();
        }
    }
}

static auto bitonic_sort_recursive(ThreadPool &pool, std::uint32_t *data, const std::size_t low, const std::size_t count, const bool ascending) -> void {
    if (count > 1) {
        const std::size_t mid = count / 2;

        auto left = pool.enqueue(bitonic_sort_recursive, std::ref(pool), data, low, mid, true);

        if(!left.has_value()) {
            bitonic_sort_recursive(pool, data, low, mid, true);
        }

        bitonic_sort_recursive(pool, data, low + mid, mid, false);

        if(left.has_value()) {
            left->get();
        }

        bitonic_sort_merge(pool, data, low, count, ascending);
    }
}

auto bitonic_sort(std::uint32_t *data, const std::size_t n) -> void {
    if (!is_power_of_two(n)) {
        throw std::invalid_argument("n must be a power of two");
    }

    ThreadPool pool(3);
    bitonic_sort_recursive(pool, data, 0, n, true);
}

static auto bitonic_sort_merge_simd(ThreadPool &pool, std::uint32_t *data, const std::size_t low, const std::size_t count, const bool ascending) -> void {
    if(count > 1) {
        const std::size_t mid = count / 2;

        std::size_t i = low;
        for(; i + 4 <= low + mid; i += 4) {

            // Load four consecutive uint32_t values from the memory location pointed to by &data[i] into a 128-bit NEON register a
            uint32x4_t a = vld1q_u32(&data[i]);

            // Load four consecutive uint32_t values from the memory location pointed to by &data[i + mid] into a 128-bit NEON register b
            uint32x4_t b = vld1q_u32(&data[i + mid]);

            // Compare the four pairs of uint32_t values in a and b and put the smaller of each pair in the corresponding element of min
            uint32x4_t min = vminq_u32(a, b);

            // Compare the four pairs of uint32_t values in a and b and put the larger of each pair in the corresponding element of max
            uint32x4_t max = vmaxq_u32(a, b);

            // If ascending is true, store the four uint32_t values in min to the memory location pointed to by &data[i]
            // and store the four uint32_t values in max to the memory location pointed to by &data[i + mid]
            // If ascending is false, store the four uint32_t values in max to the memory location pointed to by &data[i]
            // and store the four uint32_t values in min to the memory location pointed to by &data[i + mid]
            if(ascending) {
                vst1q_u32(&data[i], min);
                vst1q_u32(&data[i + mid], max);
            } else {
                vst1q_u32(&data[i], max);
                vst1q_u32(&data[i + mid], min);
            }
        }

        // Handle remaining elements
        for(; i < low + mid; ++i) {
            if(ascending == (data[i] > data[i + mid])) {
                std::swap(data[i], data[i + mid]);
            }
        }

        auto left = pool.enqueue(bitonic_sort_merge_simd, std::ref(pool), data, low, mid, ascending);

        if (!left.has_value()) {
            bitonic_sort_merge_simd(pool, data, low, mid, ascending);
        }

        bitonic_sort_merge_simd(pool, data, low + mid, mid, ascending);

        if(left.has_value()) {
            left->get();
        }
    }
}

static auto bitonic_sort_recursive_simd(ThreadPool &pool, std::uint32_t *data, const std::size_t low, const std::size_t count, const bool ascending) -> void {
    if (count > 1) {
        const std::size_t mid = count / 2;

        auto left = pool.enqueue(bitonic_sort_recursive_simd, std::ref(pool), data, low, mid, true);

        if(!left.has_value()) {
            bitonic_sort_recursive_simd(pool, data, low, mid, true);
        }

        bitonic_sort_recursive_simd(pool, data, low + mid, mid, false);

        if(left.has_value()) {
            left->get();
        }

        bitonic_sort_merge_simd(pool, data, low, count, ascending);
    }
}

auto bitonic_sort_simd(std::uint32_t *data, const std::size_t n) -> void {
    if (!is_power_of_two(n)) {
        throw std::invalid_argument("n must be a power of two");
    }

    ThreadPool pool(3);
    bitonic_sort_recursive_simd(pool, data, 0, n, true);
}