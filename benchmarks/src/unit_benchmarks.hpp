#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
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

    bench.run(
        [&]()
        {
            for (auto i = 0ULL; i < size; i++) {
                T res;
                ankerl::nanobench::doNotOptimizeAway(res);
                res = vec_a[i] + vec_b[i];
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

    bench.run(
        [&]()
        {
            for (auto i = 0ULL; i < size - WidthV; i++) {
                auto array_c = std::array<T, WidthV> {};
                ankerl::nanobench::doNotOptimizeAway(array_c.data());
                // We expect the inner loop to be vectorized to SIMD instructions
                for (size_t j = 0; j < WidthV; j++) {
                    array_c[j] = vec_a[i + j] + vec_b[i + j];
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
    std::generate(vec_a.begin(), vec_a.end(), []() { return generate_randomish<T> {}(); });
    std::generate(vec_b.begin(), vec_b.end(), []() { return generate_randomish<T> {}(); });

    bench.run(
        [&]()
        {
            for (auto i = 0ULL; i < size; i++) {
                T res;
                ankerl::nanobench::doNotOptimizeAway(res);
                res = vec_a[i] - vec_b[i];
                ankerl::nanobench::doNotOptimizeAway(res);
            }
        });
}

template<typename T, size_t WidthV>
void benchmark_subtract_units_simd(ankerl::nanobench::Bench& bench, size_t size)
{
    auto vec_a = std::vector<T>(size);
    auto vec_b = std::vector<T>(size);
    std::generate(vec_a.begin(), vec_a.end(), []() { return generate_randomish<T> {}(); });
    std::generate(vec_b.begin(), vec_b.end(), []() { return generate_randomish<T> {}(); });

    bench.run(
        [&]()
        {
            for (auto i = 0ULL; i < size - WidthV; i++) {
                auto array_c = std::array<T, WidthV> {};
                ankerl::nanobench::doNotOptimizeAway(array_c.data());
                // We expect the inner loop to be vectorized to SIMD instructions
                for (size_t j = 0; j < WidthV; j++) {
                    array_c[j] = vec_a[i + j] - vec_b[i + j];
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
    std::generate(vec_a.begin(), vec_a.end(), []() { return generate_randomish<T> {}(); });
    std::generate(vec_b.begin(), vec_b.end(), []() { return generate_randomish<O> {}(); });
    using res_t = decltype(T {} * O {});

    bench.run(
        [&]()
        {
            for (auto i = 0ULL; i < size; i++) {
                res_t res;
                ankerl::nanobench::doNotOptimizeAway(res);
                res = vec_a[i] * vec_b[i];
                ankerl::nanobench::doNotOptimizeAway(res);
            }
        });
}

template<typename T, typename O, size_t WidthV>
void benchmark_multiply_units_simd(ankerl::nanobench::Bench& bench, size_t size)
{
    auto vec_a = std::vector<T>(size);
    auto vec_b = std::vector<O>(size);
    std::generate(vec_a.begin(), vec_a.end(), []() { return generate_randomish<T> {}(); });
    std::generate(vec_b.begin(), vec_b.end(), []() { return generate_randomish<O> {}(); });
    using res_t = decltype(T {} * O {});

    bench.run(
        [&]()
        {
            for (auto i = 0ULL; i < size - WidthV; i++) {
                auto array_c = std::array<res_t, WidthV> {};
                ankerl::nanobench::doNotOptimizeAway(array_c.data());
                // We expect the inner loop to be vectorized to SIMD instructions
                for (size_t j = 0; j < WidthV; j++) {
                    array_c[j] = vec_a[i + j] * vec_b[i + j];
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
    std::generate(vec_a.begin(), vec_a.end(), []() { return generate_randomish<T> {}(); });
    std::generate(vec_b.begin(), vec_b.end(), []() { return generate_none_zero_randomish<O>(); });
    using res_t = decltype(T {} / O {});

    bench.run(
        [&]()
        {
            for (auto i = 0ULL; i < size; i++) {
                res_t res;
                ankerl::nanobench::doNotOptimizeAway(res);
                res = vec_a[i] / vec_b[i];
                ankerl::nanobench::doNotOptimizeAway(res);
            }
        });
}

template<typename T, typename O, size_t WidthV>
void benchmark_divide_units_simd(ankerl::nanobench::Bench& bench, size_t size)
{
    auto vec_a = std::vector<T>(size);
    auto vec_b = std::vector<O>(size);
    std::generate(vec_a.begin(), vec_a.end(), []() { return generate_randomish<T> {}(); });
    std::generate(vec_b.begin(), vec_b.end(), []() { return generate_none_zero_randomish<O>(); });
    using res_t = decltype(T {} / O {});

    bench.run(
        [&]()
        {
            for (auto i = 0ULL; i < size - WidthV; i++) {
                auto array_c = std::array<res_t, WidthV> {};
                ankerl::nanobench::doNotOptimizeAway(array_c.data());
                // We expect the inner loop to be vectorized to SIMD instructions
                for (size_t j = 0; j < WidthV; j++) {
                    array_c[j] = vec_a[i + j] / vec_b[i + j];
                }
                ankerl::nanobench::doNotOptimizeAway(array_c);
            }
        });
}

inline void run_add_units_benchmarks()
{
    benchmark_add_units<int8_t>(ankerl::nanobench::Bench().title("add_units_int8_t"), 8192);
    benchmark_add_units<int8_t_wrapping_type>(ankerl::nanobench::Bench().title("add_units_int8_t_wrapping_type"), 8192);

    benchmark_add_units<int64_t>(ankerl::nanobench::Bench().title("add_units_int64_t"), 8192);
    benchmark_add_units<int64_t_wrapping_type>(ankerl::nanobench::Bench().title("add_units_int64_t_wrapping_type"),
                                               8192);

    benchmark_add_units<double>(ankerl::nanobench::Bench().title("add_units_double"), 8192);
    benchmark_add_units<double_wrapping_type>(ankerl::nanobench::Bench().title("add_units_double_wrapping_type"), 8192);
}

inline void run_add_units_simd_benchmarks()
{
    benchmark_add_units_simd<int8_t, 32>(ankerl::nanobench::Bench().title("add_units_simd_int8_t"), 8192 / 32);
    benchmark_add_units_simd<int8_t_wrapping_type, 32>(
        ankerl::nanobench::Bench().title("add_units_simd_int8_t_wrapping_type"), 8192 / 32);

    benchmark_add_units_simd<int64_t, 32>(ankerl::nanobench::Bench().title("add_units_simd_int64_t"), 8192 / 32);
    benchmark_add_units_simd<int64_t_wrapping_type, 32>(
        ankerl::nanobench::Bench().title("add_units_simd_int64_t_wrapping_type"), 8192 / 32);

    benchmark_add_units_simd<double, 32>(ankerl::nanobench::Bench().title("add_units_simd_double"), 8192 / 32);
    benchmark_add_units_simd<double_wrapping_type, 32>(
        ankerl::nanobench::Bench().title("add_units_simd_double_wrapping_type"), 8192 / 32);
}

inline void run_subtract_units_benchmarks()
{
    benchmark_subtract_units<int8_t>(ankerl::nanobench::Bench().title("subtract_units_int8_t"), 8192);
    benchmark_subtract_units<int8_t_wrapping_type>(
        ankerl::nanobench::Bench().title("subtract_units_int8_t_wrapping_type"), 8192);

    benchmark_subtract_units<int64_t>(ankerl::nanobench::Bench().title("subtract_units_int64_t"), 8192);
    benchmark_subtract_units<int64_t_wrapping_type>(
        ankerl::nanobench::Bench().title("subtract_units_int64_t_wrapping_type"), 8192);

    benchmark_subtract_units<double>(ankerl::nanobench::Bench().title("subtract_units_double"), 8192);
    benchmark_subtract_units<double_wrapping_type>(
        ankerl::nanobench::Bench().title("subtract_units_double_wrapping_type"), 8192);
}

inline void run_subtract_units_simd_benchmarks()
{
    benchmark_subtract_units_simd<int8_t, 32>(ankerl::nanobench::Bench().title("subtract_units_simd_int8_t"),
                                              8192 / 32);
    benchmark_subtract_units_simd<int8_t_wrapping_type, 32>(
        ankerl::nanobench::Bench().title("subtract_units_simd_int8_t_wrapping_type"), 8192 / 32);

    benchmark_subtract_units_simd<int64_t, 32>(ankerl::nanobench::Bench().title("subtract_units_simd_int64_t"),
                                               8192 / 32);
    benchmark_subtract_units_simd<int64_t_wrapping_type, 32>(
        ankerl::nanobench::Bench().title("subtract_units_simd_int64_t_wrapping_type"), 8192 / 32);

    benchmark_subtract_units_simd<double, 32>(ankerl::nanobench::Bench().title("subtract_units_simd_double"),
                                              8192 / 32);
    benchmark_subtract_units_simd<double_wrapping_type, 32>(
        ankerl::nanobench::Bench().title("subtract_units_simd_double_wrapping_type"), 8192 / 32);
}

inline void run_multiply_units_benchmarks()
{
    benchmark_multiply_units<int8_t, int8_t>(ankerl::nanobench::Bench().title("multiply_units_int8_t"), 8192);
    benchmark_multiply_units<int8_t_wrapping_type, int8_t_wrapping_type>(
        ankerl::nanobench::Bench().title("multiply_units_int8_t_wrapping_type"), 8192);

    benchmark_multiply_units<int64_t, int64_t>(ankerl::nanobench::Bench().title("multiply_units_int64_t"), 8192);
    benchmark_multiply_units<int64_t_wrapping_type, int64_t_wrapping_type>(
        ankerl::nanobench::Bench().title("multiply_units_int64_t_wrapping_type"), 8192);

    benchmark_multiply_units<double, double>(ankerl::nanobench::Bench().title("multiply_units_double"), 8192);
    benchmark_multiply_units<double_wrapping_type, double_wrapping_type>(
        ankerl::nanobench::Bench().title("multiply_units_double_wrapping_type"), 8192);

    benchmark_multiply_units<int64_t, double>(ankerl::nanobench::Bench().title("multiply_units_int64_t_double"), 8192);
    benchmark_multiply_units<int64_t_wrapping_type, double_wrapping_type>(
        ankerl::nanobench::Bench().title("multiply_units_int64_t_wrapping_type_double_wrapping_type"), 8192);

    benchmark_multiply_units<double, int64_t>(ankerl::nanobench::Bench().title("multiply_units_double_int64_t"), 8192);
    benchmark_multiply_units<double_wrapping_type, int64_t_wrapping_type>(
        ankerl::nanobench::Bench().title("multiply_units_double_wrapping_type_int64_t_wrapping_type"), 8192);
}

inline void run_multiply_units_simd_benchmarks()
{
    benchmark_multiply_units_simd<int8_t, int8_t, 32>(ankerl::nanobench::Bench().title("multiply_units_simd_int8_t"),
                                                      8192 / 32);
    benchmark_multiply_units_simd<int8_t_wrapping_type, int8_t_wrapping_type, 32>(
        ankerl::nanobench::Bench().title("multiply_units_simd_int8_t_wrapping_type"), 8192 / 32);

    benchmark_multiply_units_simd<int64_t, int64_t, 32>(ankerl::nanobench::Bench().title("multiply_units_simd_int64_t"),
                                                        8192 / 32);
    benchmark_multiply_units_simd<int64_t_wrapping_type, int64_t_wrapping_type, 32>(
        ankerl::nanobench::Bench().title("multiply_units_simd_int64_t_wrapping_type"), 8192 / 32);

    benchmark_multiply_units_simd<double, double, 32>(ankerl::nanobench::Bench().title("multiply_units_simd_double"),
                                                      8192 / 32);
    benchmark_multiply_units_simd<double_wrapping_type, double_wrapping_type, 32>(
        ankerl::nanobench::Bench().title("multiply_units_simd_double_wrapping_type"), 8192 / 32);

    benchmark_multiply_units_simd<int64_t, double, 32>(
        ankerl::nanobench::Bench().title("multiply_units_simd_int64_t_double"), 8192 / 32);
    benchmark_multiply_units_simd<int64_t_wrapping_type, double_wrapping_type, 32>(
        ankerl::nanobench::Bench().title("multiply_units_simd_int64_t_wrapping_type_double_wrapping_type"), 8192 / 32);

    benchmark_multiply_units_simd<double, int64_t, 32>(
        ankerl::nanobench::Bench().title("multiply_units_simd_double_int64_t"), 8192 / 32);
    benchmark_multiply_units_simd<double_wrapping_type, int64_t_wrapping_type, 32>(
        ankerl::nanobench::Bench().title("multiply_units_simd_double_wrapping_type_int64_t_wrapping_type"), 8192 / 32);
}

inline void run_divide_units_benchmarks()
{
    benchmark_divide_units<int8_t, int8_t>(ankerl::nanobench::Bench().title("divide_units_int8_t"), 8192);
    benchmark_divide_units<int8_t_wrapping_type, int8_t_wrapping_type>(
        ankerl::nanobench::Bench().title("divide_units_int8_t_wrapping_type"), 8192);

    benchmark_divide_units<int64_t, int64_t>(ankerl::nanobench::Bench().title("divide_units_int64_t"), 8192);
    benchmark_divide_units<int64_t_wrapping_type, int64_t_wrapping_type>(
        ankerl::nanobench::Bench().title("divide_units_int64_t_wrapping_type"), 8192);

    benchmark_divide_units<double, double>(ankerl::nanobench::Bench().title("divide_units_double"), 8192);
    benchmark_divide_units<double_wrapping_type, double_wrapping_type>(
        ankerl::nanobench::Bench().title("divide_units_double_wrapping_type"), 8192);

    benchmark_divide_units<int64_t, double>(ankerl::nanobench::Bench().title("divide_units_int64_t_double"), 8192);
    benchmark_divide_units<int64_t_wrapping_type, double_wrapping_type>(
        ankerl::nanobench::Bench().title("divide_units_int64_t_wrapping_type_double_wrapping_type"), 8192);

    benchmark_divide_units<double, int64_t>(ankerl::nanobench::Bench().title("divide_units_double_int64_t"), 8192);
    benchmark_divide_units<double_wrapping_type, int64_t_wrapping_type>(
        ankerl::nanobench::Bench().title("divide_units_double_wrapping_type_int64_t_wrapping_type"), 8192);
}

inline void run_divide_units_simd_benchmarks()
{
    benchmark_divide_units_simd<int8_t, int8_t, 32>(ankerl::nanobench::Bench().title("divide_units_simd_int8_t"),
                                                    8192 / 32);
    benchmark_divide_units_simd<int8_t_wrapping_type, int8_t_wrapping_type, 32>(
        ankerl::nanobench::Bench().title("divide_units_simd_int8_t_wrapping_type"), 8192 / 32);

    benchmark_divide_units_simd<int64_t, int64_t, 32>(ankerl::nanobench::Bench().title("divide_units_simd_int64_t"),
                                                      8192 / 32);
    benchmark_divide_units_simd<int64_t_wrapping_type, int64_t_wrapping_type, 32>(
        ankerl::nanobench::Bench().title("divide_units_simd_int64_t_wrapping_type"), 8192 / 32);

    benchmark_divide_units_simd<double, double, 32>(ankerl::nanobench::Bench().title("divide_units_simd_double"),
                                                    8192 / 32);
    benchmark_divide_units_simd<double_wrapping_type, double_wrapping_type, 32>(
        ankerl::nanobench::Bench().title("divide_units_simd_double_wrapping_type"), 8192 / 32);

    benchmark_divide_units_simd<int64_t, double, 32>(
        ankerl::nanobench::Bench().title("divide_units_simd_int64_t_double"), 8192 / 32);
    benchmark_divide_units_simd<int64_t_wrapping_type, double_wrapping_type, 32>(
        ankerl::nanobench::Bench().title("divide_units_simd_int64_t_wrapping_type_double_wrapping_type"), 8192 / 32);

    benchmark_divide_units_simd<double, int64_t, 32>(
        ankerl::nanobench::Bench().title("divide_units_simd_double_int64_t"), 8192 / 32);
    benchmark_divide_units_simd<double_wrapping_type, int64_t_wrapping_type, 32>(
        ankerl::nanobench::Bench().title("divide_units_simd_double_wrapping_type_int64_t_wrapping_type"), 8192 / 32);
}
