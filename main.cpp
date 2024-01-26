#include <__algorithm/ranges_sort.h>
#include <iostream>

#include "radix_sort.h"

#include <iostream>
#include <chrono>
#include <cmath>
#include <thread>

#include "bitonic_sort.h"


int main() {

    std::cout << (int)x << std::endl;

    std::vector<std::uint32_t> vec;
    const auto vec_size = static_cast<std::size_t>(std::pow<std::size_t, std::size_t>(2, 20));
    for(std::size_t i = 0; i < vec_size; ++i) {
        auto x = rand();
        vec.push_back(*reinterpret_cast<std::uint32_t*>(&x));
    }

    std::cout << std::thread::hardware_concurrency() << " threads." << std::endl;

    auto vec2 = vec;
    std::uint32_t* vec3 = nullptr;
    if(posix_memalign((void**)&vec3, 16, vec_size * sizeof(std::uint32_t)) != 0) {
        throw std::bad_alloc();
    }

    for(std::size_t i = 0; i < vec_size; ++i) {
        vec3[i] = vec[i];
    }


    // Time the sorting
    auto start = std::chrono::high_resolution_clock::now();
    std::ranges::sort(vec);
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms for std::sort." << std::endl;


    start = std::chrono::high_resolution_clock::now();
    bitonic_sort(vec2.data(), vec2.size());
    end = std::chrono::high_resolution_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms for bitonic sort." << std::endl;

    start = std::chrono::high_resolution_clock::now();
    bitonic_sort_simd(vec3, vec_size);
    end = std::chrono::high_resolution_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms for bitonic sort simd." << std::endl;

    if(vec != vec2) {
        std::cout << "Bitonic not equal" << std::endl;
    }

    for(std::size_t i = 0; i < vec_size; ++i) {
        if(vec[i] != vec3[i]) {
            std::cout << "Bitonic simd not equal" << std::endl;
            break;
        }
    }

    return 0;
}
