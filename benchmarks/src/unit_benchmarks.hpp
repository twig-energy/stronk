#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

#include <nanobench.h>

#include "./benchmark_helpers.hpp"

template<typename T>
void benchmark_add_units(ankerl::nanobench::Bench& bench, size_t size)
{
    auto vec_a = std::vector<T>(size);
    auto vec_b = std::vector<T>(size);
    std::ranges::generate(vec_a, []() { return generate_randomish<T> {}(); });
    std::ranges::generate(vec_b, []() { return generate_randomish<T> {}(); });

    bench.complexityN(size).run(get_name<T>(),
                                [&]()
                                {
                                    for (auto i = 0ULL; i < size; i++) {
                                        T res;
                                        ankerl::nanobench::doNotOptimizeAway(res);
                                        res = vec_a[i] + vec_b[i];  // NOLINT
                                        ankerl::nanobench::doNotOptimizeAway(res);
                                    }
                                });
}

template<typename T, size_t WidthV>
void benchmark_add_units_simd(ankerl::nanobench::Bench& bench, size_t size)
{
    auto vec_a = std::vector<T>(size);
    auto vec_b = std::vector<T>(size);
    std::generate(vec_a.begin(), vec_a.end(), []() { return generate_randomish<T> {}(); });
    std::generate(vec_b.begin(), vec_b.end(), []() { return generate_randomish<T> {}(); });

    bench.complexityN(size).run(get_name<T>() + "_simd",
                                [&]()
                                {
                                    for (auto i = 0ULL; i < size - WidthV; i++) {
                                        auto array_c = std::array<T, WidthV> {};
                                        ankerl::nanobench::doNotOptimizeAway(array_c.data());
                                        // We expect the inner loop to be vectorized to SIMD instructions
                                        for (size_t j = 0; j < WidthV; j++) {
                                            array_c[j] = vec_a[i + j] + vec_b[i + j];  // NOLINT
                                        }
                                        ankerl::nanobench::doNotOptimizeAway(array_c);
                                    }
                                });
}

template<typename T>
void benchmark_subtract_units(ankerl::nanobench::Bench& bench, size_t size)
{
    auto vec_a = std::vector<T>(size);
    auto vec_b = std::vector<T>(size);
    std::ranges::generate(vec_a, []() { return generate_randomish<T> {}(); });
    std::ranges::generate(vec_b, []() { return generate_randomish<T> {}(); });

    bench.complexityN(size).run(get_name<T>(),
                                [&]()
                                {
                                    for (auto i = 0ULL; i < size; i++) {
                                        T res;
                                        ankerl::nanobench::doNotOptimizeAway(res);
                                        res = vec_a[i] - vec_b[i];  // NOLINT
                                        ankerl::nanobench::doNotOptimizeAway(res);
                                    }
                                });
}

template<typename T, size_t WidthV>
void benchmark_subtract_units_simd(ankerl::nanobench::Bench& bench, size_t size)
{
    auto vec_a = std::vector<T>(size);
    auto vec_b = std::vector<T>(size);
    std::ranges::generate(vec_a, []() { return generate_randomish<T> {}(); });
    std::ranges::generate(vec_b, []() { return generate_randomish<T> {}(); });

    bench.complexityN(size).run(get_name<T>() + "_simd",
                                [&]()
                                {
                                    for (auto i = 0ULL; i < size - WidthV; i++) {
                                        auto array_c = std::array<T, WidthV> {};
                                        ankerl::nanobench::doNotOptimizeAway(array_c.data());
                                        // We expect the inner loop to be vectorized to SIMD instructions
                                        for (size_t j = 0; j < WidthV; j++) {
                                            array_c[j] = vec_a[i + j] - vec_b[i + j];  // NOLINT
                                        }
                                        ankerl::nanobench::doNotOptimizeAway(array_c);
                                    }
                                });
}

template<typename T, typename O>
void benchmark_multiply_units(ankerl::nanobench::Bench& bench, size_t size)
{
    auto vec_a = std::vector<T>(size);
    auto vec_b = std::vector<O>(size);
    std::ranges::generate(vec_a, []() { return generate_randomish<T> {}(); });
    std::ranges::generate(vec_b, []() { return generate_randomish<O> {}(); });
    using res_t = decltype(T {} * O {});

    bench.complexityN(size).run(get_name<T>() + "_" + get_name<O>(),
                                [&]()
                                {
                                    for (auto i = 0ULL; i < size; i++) {
                                        res_t res;
                                        ankerl::nanobench::doNotOptimizeAway(res);
                                        res = vec_a[i] * vec_b[i];  // NOLINT
                                        ankerl::nanobench::doNotOptimizeAway(res);
                                    }
                                });
}

template<typename T, typename O, size_t WidthV>
void benchmark_multiply_units_simd(ankerl::nanobench::Bench& bench, size_t size)
{
    auto vec_a = std::vector<T>(size);
    auto vec_b = std::vector<O>(size);
    std::ranges::generate(vec_a, []() { return generate_randomish<T> {}(); });
    std::ranges::generate(vec_b, []() { return generate_randomish<O> {}(); });
    using res_t = decltype(T {} * O {});

    bench.complexityN(size).run(get_name<T>() + "_" + get_name<O>() + "_simd",
                                [&]()
                                {
                                    for (auto i = 0ULL; i < size - WidthV; i++) {
                                        auto array_c = std::array<res_t, WidthV> {};
                                        ankerl::nanobench::doNotOptimizeAway(array_c.data());
                                        // We expect the inner loop to be vectorized to SIMD instructions
                                        for (size_t j = 0; j < WidthV; j++) {
                                            array_c[j] = vec_a[i + j] * vec_b[i + j];  // NOLINT
                                        }
                                        ankerl::nanobench::doNotOptimizeAway(array_c);
                                    }
                                });
}

template<typename T, typename O>
void benchmark_divide_units(ankerl::nanobench::Bench& bench, size_t size)
{
    auto vec_a = std::vector<T>(size);
    auto vec_b = std::vector<O>(size);
    std::ranges::generate(vec_a, []() { return generate_randomish<T> {}(); });
    std::ranges::generate(vec_b, []() { return generate_none_zero_randomish<O>(); });
    using res_t = decltype(T {} / O {});

    bench.complexityN(size).run(get_name<T>() + "_" + get_name<O>(),
                                [&]()
                                {
                                    for (auto i = 0ULL; i < size; i++) {
                                        res_t res;
                                        ankerl::nanobench::doNotOptimizeAway(res);
                                        res = vec_a[i] / vec_b[i];  // NOLINT
                                        ankerl::nanobench::doNotOptimizeAway(res);
                                    }
                                });
}

template<typename T, typename O, size_t WidthV>
void benchmark_divide_units_simd(ankerl::nanobench::Bench& bench, size_t size)
{
    auto vec_a = std::vector<T>(size);
    auto vec_b = std::vector<O>(size);
    std::ranges::generate(vec_a, []() { return generate_randomish<T> {}(); });
    std::ranges::generate(vec_b, []() { return generate_none_zero_randomish<O>(); });
    using res_t = decltype(T {} / O {});

    bench.complexityN(size).run(get_name<T>() + "_" + get_name<O>() + "_simd",
                                [&]()
                                {
                                    for (auto i = 0ULL; i < size - WidthV; i++) {
                                        auto array_c = std::array<res_t, WidthV> {};
                                        ankerl::nanobench::doNotOptimizeAway(array_c.data());
                                        // We expect the inner loop to be vectorized to SIMD instructions
                                        for (size_t j = 0; j < WidthV; j++) {
                                            array_c[j] = vec_a[i + j] / vec_b[i + j];  // NOLINT
                                        }
                                        ankerl::nanobench::doNotOptimizeAway(array_c);
                                    }
                                });
}

inline void run_add_units_benchmarks()
{
    auto bench = ankerl::nanobench::Bench {}
                     .title("Add Units")
                     .unit("operations")
                     .warmup(100)
                     .relative(true)
                     .performanceCounters(true);

    auto sizes = std::array {32ULL, 64ULL, 128ULL, 256ULL, 512ULL, 1024ULL, 2048ULL, 4096ULL, 8192ULL};
    for (auto size : sizes) {
        benchmark_add_units<int8_t>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_add_units<int8_t_wrapping_type>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_add_units<int64_t>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_add_units<int64_t_wrapping_type>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_add_units<double>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_add_units<double_wrapping_type>(bench, size);
    }

    std::cout << bench.complexityBigO() << '\n';
}

inline void run_add_units_simd_benchmarks()
{
    auto bench = ankerl::nanobench::Bench {}
                     .title("Add Units SIMD")
                     .unit("operations")
                     .warmup(100)
                     .relative(true)
                     .performanceCounters(true);

    auto sizes = std::array {32ULL, 64ULL, 128ULL, 256ULL, 512ULL, 1024ULL, 2048ULL, 4096ULL, 8192ULL};
    for (auto size : sizes) {
        benchmark_add_units_simd<int8_t, 32>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_add_units_simd<int8_t_wrapping_type, 32>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_add_units_simd<int64_t, 32>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_add_units_simd<int64_t_wrapping_type, 32>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_add_units_simd<double, 32>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_add_units_simd<double_wrapping_type, 32>(bench, size);
    }

    std::cout << bench.complexityBigO() << '\n';
}

inline void run_subtract_units_benchmarks()
{
    auto bench = ankerl::nanobench::Bench {}
                     .title("Subtract Units")
                     .unit("operations")
                     .warmup(100)
                     .relative(true)
                     .performanceCounters(true);

    auto sizes = std::array {32ULL, 64ULL, 128ULL, 256ULL, 512ULL, 1024ULL, 2048ULL, 4096ULL, 8192ULL};
    for (auto size : sizes) {
        benchmark_subtract_units<int8_t>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_subtract_units<int8_t_wrapping_type>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_subtract_units<int64_t>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_subtract_units<int64_t_wrapping_type>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_subtract_units<double>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_subtract_units<double_wrapping_type>(bench, size);
    }

    std::cout << bench.complexityBigO() << '\n';
}

inline void run_subtract_units_simd_benchmarks()
{
    auto bench = ankerl::nanobench::Bench {}
                     .title("Subtract Units SIMD")
                     .unit("operations")
                     .warmup(100)
                     .relative(true)
                     .performanceCounters(true);

    auto sizes = std::array {32ULL, 64ULL, 128ULL, 256ULL, 512ULL, 1024ULL, 2048ULL, 4096ULL, 8192ULL};
    for (auto size : sizes) {
        benchmark_subtract_units_simd<int8_t, 32>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_subtract_units_simd<int8_t_wrapping_type, 32>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_subtract_units_simd<int64_t, 32>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_subtract_units_simd<int64_t_wrapping_type, 32>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_subtract_units_simd<double, 32>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_subtract_units_simd<double_wrapping_type, 32>(bench, size);
    }

    std::cout << bench.complexityBigO() << '\n';
}

inline void run_multiply_units_benchmarks()
{
    auto bench = ankerl::nanobench::Bench {}
                     .title("Multiply Units")
                     .unit("operations")
                     .warmup(100)
                     .relative(true)
                     .performanceCounters(true);

    auto sizes = std::array {32ULL, 64ULL, 128ULL, 256ULL, 512ULL, 1024ULL, 2048ULL, 4096ULL, 8192ULL};
    for (auto size : sizes) {
        benchmark_multiply_units<int8_t, int8_t>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_multiply_units<int8_t_wrapping_type, int8_t_wrapping_type>(bench, size);
    }

    for (auto size : sizes) {
        benchmark_multiply_units<int64_t, int64_t>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_multiply_units<int64_t_wrapping_type, int64_t_wrapping_type>(bench, size);
    }

    for (auto size : sizes) {
        benchmark_multiply_units<double, double>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_multiply_units<double_wrapping_type, double_wrapping_type>(bench, size);
    }

    for (auto size : sizes) {
        benchmark_multiply_units<int64_t, double>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_multiply_units<int64_t_wrapping_type, double_wrapping_type>(bench, size);
    }

    for (auto size : sizes) {
        benchmark_multiply_units<double, int64_t>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_multiply_units<double_wrapping_type, int64_t_wrapping_type>(bench, size);
    }
}

inline void run_multiply_units_simd_benchmarks()
{
    auto bench = ankerl::nanobench::Bench {}
                     .title("Multiply Units SIMD")
                     .unit("operations")
                     .warmup(100)
                     .relative(true)
                     .performanceCounters(true);

    auto sizes = std::array {32ULL, 64ULL, 128ULL, 256ULL, 512ULL, 1024ULL, 2048ULL, 4096ULL, 8192ULL};
    for (auto size : sizes) {
        benchmark_multiply_units_simd<int8_t, int8_t, 32>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_multiply_units_simd<int8_t_wrapping_type, int8_t_wrapping_type, 32>(bench, size);
    }

    for (auto size : sizes) {
        benchmark_multiply_units_simd<int64_t, int64_t, 32>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_multiply_units_simd<int64_t_wrapping_type, int64_t_wrapping_type, 32>(bench, size);
    }

    for (auto size : sizes) {
        benchmark_multiply_units_simd<double, double, 32>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_multiply_units_simd<double_wrapping_type, double_wrapping_type, 32>(bench, size);
    }

    for (auto size : sizes) {
        benchmark_multiply_units_simd<int64_t, double, 32>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_multiply_units_simd<int64_t_wrapping_type, double_wrapping_type, 32>(bench, size);
    }

    for (auto size : sizes) {
        benchmark_multiply_units_simd<double, int64_t, 32>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_multiply_units_simd<double_wrapping_type, int64_t_wrapping_type, 32>(bench, size);
    }
}

inline void run_divide_units_benchmarks()
{
    auto bench = ankerl::nanobench::Bench {}
                     .title("Divide Units")
                     .unit("operations")
                     .warmup(100)
                     .relative(true)
                     .performanceCounters(true);

    auto sizes = std::array {32ULL, 64ULL, 128ULL, 256ULL, 512ULL, 1024ULL, 2048ULL, 4096ULL, 8192ULL};
    for (auto size : sizes) {
        benchmark_divide_units<int8_t, int8_t>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_divide_units<int8_t_wrapping_type, int8_t_wrapping_type>(bench, size);
    }

    for (auto size : sizes) {
        benchmark_divide_units<int64_t, int64_t>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_divide_units<int64_t_wrapping_type, int64_t_wrapping_type>(bench, size);
    }

    for (auto size : sizes) {
        benchmark_divide_units<double, double>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_divide_units<double_wrapping_type, double_wrapping_type>(bench, size);
    }

    for (auto size : sizes) {
        benchmark_divide_units<int64_t, double>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_divide_units<int64_t_wrapping_type, double_wrapping_type>(bench, size);
    }

    for (auto size : sizes) {
        benchmark_divide_units<double, int64_t>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_divide_units<double_wrapping_type, int64_t_wrapping_type>(bench, size);
    }
}

inline void run_divide_units_simd_benchmarks()
{
    auto bench = ankerl::nanobench::Bench {}
                     .title("Divide Units SIMD")
                     .unit("operations")
                     .warmup(100)
                     .relative(true)
                     .performanceCounters(true);

    auto sizes = std::array {32ULL, 64ULL, 128ULL, 256ULL, 512ULL, 1024ULL, 2048ULL, 4096ULL, 8192ULL};
    for (auto size : sizes) {
        benchmark_divide_units_simd<int8_t, int8_t, 32>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_divide_units_simd<int8_t_wrapping_type, int8_t_wrapping_type, 32>(bench, size);
    }

    for (auto size : sizes) {
        benchmark_divide_units_simd<int64_t, int64_t, 32>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_divide_units_simd<int64_t_wrapping_type, int64_t_wrapping_type, 32>(bench, size);
    }

    for (auto size : sizes) {
        benchmark_divide_units_simd<double, double, 32>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_divide_units_simd<double_wrapping_type, double_wrapping_type, 32>(bench, size);
    }

    for (auto size : sizes) {
        benchmark_divide_units_simd<int64_t, double, 32>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_divide_units_simd<int64_t_wrapping_type, double_wrapping_type, 32>(bench, size);
    }

    for (auto size : sizes) {
        benchmark_divide_units_simd<double, int64_t, 32>(bench, size);
    }
    for (auto size : sizes) {
        benchmark_divide_units_simd<double_wrapping_type, int64_t_wrapping_type, 32>(bench, size);
    }
}
