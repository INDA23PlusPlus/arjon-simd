cmake_minimum_required(VERSION 3.27)
project(simd)

set(CMAKE_CXX_STANDARD 23)

add_compile_options(-Ofast -Wall -Wextra -Wpedantic -Werror -march=native -mtune=native)

add_executable(simd main.cpp
        radix_sort.cpp
        radix_sort.h
        bitonic_sort.cpp
        bitonic_sort.h
        thread_pool.cpp
        thread_pool.h)
