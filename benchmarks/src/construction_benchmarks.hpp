#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <nanobench.h>

#include "./benchmark_helpers.hpp"

template<typename T>
void benchmark_default_onto_reserved_vector(ankerl::nanobench::Bench& bench, size_t size)
{
    auto vec = std::vector<T>(size);
    bench.complexityN(size).run(get_name<T>(),
                                [&]()
                                {
                                    ankerl::nanobench::doNotOptimizeAway(vec.data());
                                    for (auto i = 0ULL; i < size; i++) {
                                        vec[i] = T {};
                                    }
                                    ankerl::nanobench::doNotOptimizeAway(vec);
                                });
}

inline void run_default_onto_reserved_vector_benchmarks()
{
    ankerl::nanobench::Bench bench;
    bench.title("Default Construction onto Reserved Vector").unit("elements").warmup(100).relative(true);
    bench.performanceCounters(true);

    auto sizes = std::array {32ULL, 64ULL, 128ULL, 256ULL, 512ULL, 1024ULL, 2048ULL, 4096ULL, 8192ULL};
    for (auto size : sizes) {
        benchmark_default_onto_reserved_vector<int8_t>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_default_onto_reserved_vector<int8_t_wrapping_type>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_default_onto_reserved_vector<int64_t>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_default_onto_reserved_vector<int64_t_wrapping_type>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_default_onto_reserved_vector<std::string>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_default_onto_reserved_vector<string_wrapping_type>(bench, size);
    }

    std::cout << bench.complexityBigO() << '\n';
}

template<typename T>
void benchmark_rand_onto_reserved_vector(ankerl::nanobench::Bench& bench, size_t size)
{
    auto vec = std::vector<T>(size);
    bench.complexityN(size).run(get_name<T>(),
                                [&]()
                                {
                                    ankerl::nanobench::doNotOptimizeAway(vec.data());
                                    for (auto i = 0ULL; i < size; i++) {
                                        vec[i] = generate_randomish<T> {}();
                                    }
                                    ankerl::nanobench::doNotOptimizeAway(vec);
                                });
}

inline void run_rand_onto_reserved_vector_benchmarks()
{
    ankerl::nanobench::Bench bench;
    bench.title("Random Construction onto Reserved Vector").unit("elements").warmup(100).relative(true);
    bench.performanceCounters(true);

    auto sizes = std::array {32ULL, 64ULL, 128ULL, 256ULL, 512ULL, 1024ULL, 2048ULL, 4096ULL, 8192ULL};
    for (auto size : sizes) {
        benchmark_rand_onto_reserved_vector<int8_t>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_rand_onto_reserved_vector<int8_t_wrapping_type>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_rand_onto_reserved_vector<int64_t>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_rand_onto_reserved_vector<int64_t_wrapping_type>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_rand_onto_reserved_vector<std::string>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_rand_onto_reserved_vector<string_wrapping_type>(bench, size);
    }
    std::cout << bench.complexityBigO() << '\n';
}

template<typename T>
void benchmark_copy_vector_of(ankerl::nanobench::Bench& bench, size_t size)
{
    auto vec = std::vector<T>(size);
    std::generate(vec.begin(), vec.end(), []() { return generate_randomish<T> {}(); });
    bench.complexityN(size).run(get_name<T>(),
                                [&]()
                                {
                                    ankerl::nanobench::doNotOptimizeAway(vec.data());
                                    auto copy_into = vec;
                                    ankerl::nanobench::doNotOptimizeAway(copy_into.data());
                                });
}

inline void run_copy_vector_of_benchmarks()
{
    ankerl::nanobench::Bench bench;
    bench.title("Copy Vector Benchmarks").unit("elements").warmup(100).relative(true);
    bench.performanceCounters(true);

    auto sizes = std::array {32ULL, 64ULL, 128ULL, 256ULL, 512ULL, 1024ULL, 2048ULL, 4096ULL, 8192ULL};
    for (auto size : sizes) {
        benchmark_copy_vector_of<int8_t>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_copy_vector_of<int8_t_wrapping_type>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_copy_vector_of<int64_t>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_copy_vector_of<int64_t_wrapping_type>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_copy_vector_of<std::string>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_copy_vector_of<string_wrapping_type>(bench, size);
    }
    std::cout << bench.complexityBigO() << '\n';
}
