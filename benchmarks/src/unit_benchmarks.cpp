#include <chrono>
#include <cstdint>
#include <string>

#include <benchmark/benchmark.h>
#include <fmt/core.h>

#include "./benchmark_helpers.h"

template<typename T>
static void benchmark_add_units(benchmark::State& state)
{
    auto vec_a = std::vector<T>(static_cast<size_t>(state.range(0)));
    auto vec_b = std::vector<T>(static_cast<size_t>(state.range(0)));
    std::generate(vec_a.begin(), vec_a.end(), []() { return generate_randomish<T> {}(); });
    std::generate(vec_b.begin(), vec_b.end(), []() { return generate_randomish<T> {}(); });

    for (auto _ : state) {
        for (auto i = 0ULL; i < static_cast<size_t>(state.range(0)); i++) {
            T res;
            benchmark::DoNotOptimize(res);
            res = vec_a[i] + vec_b[i];
            benchmark::ClobberMemory();
        }
    }
}

template<typename T, size_t WidthV>
static void benchmark_add_units_simd(benchmark::State& state)
{
    auto vec_a = std::vector<T>(static_cast<size_t>(state.range(0)));
    auto vec_b = std::vector<T>(static_cast<size_t>(state.range(0)));
    std::generate(vec_a.begin(), vec_a.end(), []() { return generate_randomish<T> {}(); });
    std::generate(vec_b.begin(), vec_b.end(), []() { return generate_randomish<T> {}(); });

    for (auto _ : state) {
        for (auto i = 0ULL; i < static_cast<size_t>(state.range(0)) - WidthV; i++) {
            auto array_c = std::array<T, WidthV> {};
            benchmark::DoNotOptimize(array_c.data());
            // We expect the inner loop to be vectorized to SIMD instructions
            for (size_t j = 0; j < WidthV; j++) {
                array_c[j] = vec_a[i + j] + vec_b[i + j];
            }
            benchmark::ClobberMemory();
        }
    }
}

// // benchmark_add_units
BENCHMARK_TEMPLATE(benchmark_add_units, int8_t)->Arg(8192);
BENCHMARK_TEMPLATE(benchmark_add_units, int8_t_wrapping_type)->Arg(8192);

BENCHMARK_TEMPLATE(benchmark_add_units, int64_t)->Arg(8192);
BENCHMARK_TEMPLATE(benchmark_add_units, int64_t_wrapping_type)->Arg(8192);

BENCHMARK_TEMPLATE(benchmark_add_units, double)->Arg(8192);
BENCHMARK_TEMPLATE(benchmark_add_units, double_wrapping_type)->Arg(8192);

// benchmark_add_units_simd
BENCHMARK_TEMPLATE(benchmark_add_units_simd, int8_t, 32)->Arg(8192 / 32);
BENCHMARK_TEMPLATE(benchmark_add_units_simd, int8_t_wrapping_type, 32)->Arg(8192 / 32);

BENCHMARK_TEMPLATE(benchmark_add_units_simd, int64_t, 32)->Arg(8192 / 32);
BENCHMARK_TEMPLATE(benchmark_add_units_simd, int64_t_wrapping_type, 32)->Arg(8192 / 32);

BENCHMARK_TEMPLATE(benchmark_add_units_simd, double, 32)->Arg(8192 / 32);
BENCHMARK_TEMPLATE(benchmark_add_units_simd, double_wrapping_type, 32)->Arg(8192 / 32);

template<typename T>
static void benchmark_subtract_units(benchmark::State& state)
{
    auto vec_a = std::vector<T>(static_cast<size_t>(state.range(0)));
    auto vec_b = std::vector<T>(static_cast<size_t>(state.range(0)));
    std::generate(vec_a.begin(), vec_a.end(), []() { return generate_randomish<T> {}(); });
    std::generate(vec_b.begin(), vec_b.end(), []() { return generate_randomish<T> {}(); });

    for (auto _ : state) {
        for (auto i = 0ULL; i < static_cast<size_t>(state.range(0)); i++) {
            T res;
            benchmark::DoNotOptimize(res);
            res = vec_a[i] - vec_b[i];
            benchmark::ClobberMemory();
        }
    }
}

template<typename T, size_t WidthV>
static void benchmark_subtract_units_simd(benchmark::State& state)
{
    auto vec_a = std::vector<T>(static_cast<size_t>(state.range(0)));
    auto vec_b = std::vector<T>(static_cast<size_t>(state.range(0)));
    std::generate(vec_a.begin(), vec_a.end(), []() { return generate_randomish<T> {}(); });
    std::generate(vec_b.begin(), vec_b.end(), []() { return generate_randomish<T> {}(); });

    for (auto _ : state) {
        for (auto i = 0ULL; i < static_cast<size_t>(state.range(0)) - WidthV; i++) {
            auto array_c = std::array<T, WidthV> {};
            benchmark::DoNotOptimize(array_c.data());
            // We expect the inner loop to be vectorized to SIMD instructions
            for (size_t j = 0; j < WidthV; j++) {
                array_c[j] = vec_a[i + j] - vec_b[i + j];
            }
            benchmark::ClobberMemory();
        }
    }
}

BENCHMARK_TEMPLATE(benchmark_subtract_units, int8_t)->Arg(8192);
BENCHMARK_TEMPLATE(benchmark_subtract_units, int8_t_wrapping_type)->Arg(8192);

BENCHMARK_TEMPLATE(benchmark_subtract_units, int64_t)->Arg(8192);
BENCHMARK_TEMPLATE(benchmark_subtract_units, int64_t_wrapping_type)->Arg(8192);

BENCHMARK_TEMPLATE(benchmark_subtract_units, double)->Arg(8192);
BENCHMARK_TEMPLATE(benchmark_subtract_units, double_wrapping_type)->Arg(8192);

// simd
BENCHMARK_TEMPLATE(benchmark_subtract_units_simd, int8_t, 32)->Arg(8192 / 32);
BENCHMARK_TEMPLATE(benchmark_subtract_units_simd, int8_t_wrapping_type, 32)->Arg(8192 / 32);

BENCHMARK_TEMPLATE(benchmark_subtract_units_simd, int64_t, 32)->Arg(8192 / 32);
BENCHMARK_TEMPLATE(benchmark_subtract_units_simd, int64_t_wrapping_type, 32)->Arg(8192 / 32);

BENCHMARK_TEMPLATE(benchmark_subtract_units_simd, double, 32)->Arg(8192 / 32);
BENCHMARK_TEMPLATE(benchmark_subtract_units_simd, double_wrapping_type, 32)->Arg(8192 / 32);

template<typename T, typename O>
static void benchmark_multiply_units(benchmark::State& state)
{
    auto vec_a = std::vector<T>(static_cast<size_t>(state.range(0)));
    auto vec_b = std::vector<O>(static_cast<size_t>(state.range(0)));
    std::generate(vec_a.begin(), vec_a.end(), []() { return generate_randomish<T> {}(); });
    std::generate(vec_b.begin(), vec_b.end(), []() { return generate_randomish<O> {}(); });
    using res_t = decltype(T {} * O {});

    for (auto _ : state) {
        for (auto i = 0ULL; i < static_cast<size_t>(state.range(0)); i++) {
            res_t res;
            benchmark::DoNotOptimize(res);
            res = vec_a[i] * vec_b[i];
            benchmark::ClobberMemory();
        }
    }
}

template<typename T, typename O, size_t WidthV>
static void benchmark_multiply_units_simd(benchmark::State& state)
{
    auto vec_a = std::vector<T>(static_cast<size_t>(state.range(0)));
    auto vec_b = std::vector<O>(static_cast<size_t>(state.range(0)));
    std::generate(vec_a.begin(), vec_a.end(), []() { return generate_randomish<T> {}(); });
    std::generate(vec_b.begin(), vec_b.end(), []() { return generate_randomish<O> {}(); });
    using res_t = decltype(T {} * O {});

    for (auto _ : state) {
        for (auto i = 0ULL; i < static_cast<size_t>(state.range(0)) - WidthV; i++) {
            auto array_c = std::array<res_t, WidthV> {};
            benchmark::DoNotOptimize(array_c.data());
            // We expect the inner loop to be vectorized to SIMD instructions
            for (size_t j = 0; j < WidthV; j++) {
                array_c[j] = vec_a[i + j] * vec_b[i + j];
            }
            benchmark::ClobberMemory();
        }
    }
}

BENCHMARK_TEMPLATE(benchmark_multiply_units, int8_t, int8_t)->Arg(8192);
BENCHMARK_TEMPLATE(benchmark_multiply_units, int8_t_wrapping_type, int8_t_wrapping_type)->Arg(8192);

BENCHMARK_TEMPLATE(benchmark_multiply_units, int64_t, int64_t)->Arg(8192);
BENCHMARK_TEMPLATE(benchmark_multiply_units, int64_t_wrapping_type, int64_t_wrapping_type)->Arg(8192);

BENCHMARK_TEMPLATE(benchmark_multiply_units, double, double)->Arg(8192);
BENCHMARK_TEMPLATE(benchmark_multiply_units, double_wrapping_type, double_wrapping_type)->Arg(8192);

BENCHMARK_TEMPLATE(benchmark_multiply_units, int64_t, double)->Arg(8192);
BENCHMARK_TEMPLATE(benchmark_multiply_units, int64_t_wrapping_type, double_wrapping_type)->Arg(8192);

BENCHMARK_TEMPLATE(benchmark_multiply_units, double, int64_t)->Arg(8192);
BENCHMARK_TEMPLATE(benchmark_multiply_units, double_wrapping_type, int64_t_wrapping_type)->Arg(8192);

BENCHMARK_TEMPLATE(benchmark_multiply_units_simd, int8_t, int8_t, 32)->Arg(8192 / 32);
BENCHMARK_TEMPLATE(benchmark_multiply_units_simd, int8_t_wrapping_type, int8_t_wrapping_type, 32)->Arg(8192 / 32);

BENCHMARK_TEMPLATE(benchmark_multiply_units_simd, int64_t, int64_t, 32)->Arg(8192 / 32);
BENCHMARK_TEMPLATE(benchmark_multiply_units_simd, int64_t_wrapping_type, int64_t_wrapping_type, 32)->Arg(8192 / 32);

BENCHMARK_TEMPLATE(benchmark_multiply_units_simd, double, double, 32)->Arg(8192 / 32);
BENCHMARK_TEMPLATE(benchmark_multiply_units_simd, double_wrapping_type, double_wrapping_type, 32)->Arg(8192 / 32);

BENCHMARK_TEMPLATE(benchmark_multiply_units_simd, int64_t, double, 32)->Arg(8192 / 32);
BENCHMARK_TEMPLATE(benchmark_multiply_units_simd, int64_t_wrapping_type, double_wrapping_type, 32)->Arg(8192 / 32);

BENCHMARK_TEMPLATE(benchmark_multiply_units_simd, double, int64_t, 32)->Arg(8192 / 32);
BENCHMARK_TEMPLATE(benchmark_multiply_units_simd, double_wrapping_type, int64_t_wrapping_type, 32)->Arg(8192 / 32);

template<typename T, typename O>
static void benchmark_divide_units(benchmark::State& state)
{
    auto vec_a = std::vector<T>(static_cast<size_t>(state.range(0)));
    auto vec_b = std::vector<O>(static_cast<size_t>(state.range(0)));
    std::generate(vec_a.begin(), vec_a.end(), []() { return generate_randomish<T> {}(); });
    std::generate(vec_b.begin(), vec_b.end(), []() { return generate_none_zero_randomish<O>(); });
    using res_t = decltype(T {} / O {});

    for (auto _ : state) {
        for (auto i = 0ULL; i < static_cast<size_t>(state.range(0)); i++) {
            res_t res;
            benchmark::DoNotOptimize(res);
            res = vec_a[i] / vec_b[i];
            benchmark::ClobberMemory();
        }
    }
}

template<typename T, typename O, size_t WidthV>
static void benchmark_divide_units_simd(benchmark::State& state)
{
    auto vec_a = std::vector<T>(static_cast<size_t>(state.range(0)));
    auto vec_b = std::vector<O>(static_cast<size_t>(state.range(0)));
    std::generate(vec_a.begin(), vec_a.end(), []() { return generate_randomish<T> {}(); });
    std::generate(vec_b.begin(), vec_b.end(), []() { return generate_none_zero_randomish<O>(); });
    using res_t = decltype(T {} / O {});

    for (auto _ : state) {
        for (auto i = 0ULL; i < static_cast<size_t>(state.range(0)) - WidthV; i++) {
            auto array_c = std::array<res_t, WidthV> {};
            benchmark::DoNotOptimize(array_c.data());
            // We expect the inner loop to be vectorized to SIMD instructions
            for (size_t j = 0; j < WidthV; j++) {
                array_c[j] = vec_a[i + j] / vec_b[i + j];
            }
            benchmark::ClobberMemory();
        }
    }
}

BENCHMARK_TEMPLATE(benchmark_divide_units, int8_t, int8_t)->Arg(8192);
BENCHMARK_TEMPLATE(benchmark_divide_units, int8_t_wrapping_type, int8_t_wrapping_type)->Arg(8192);

BENCHMARK_TEMPLATE(benchmark_divide_units, int64_t, int64_t)->Arg(8192);
BENCHMARK_TEMPLATE(benchmark_divide_units, int64_t_wrapping_type, int64_t_wrapping_type)->Arg(8192);

BENCHMARK_TEMPLATE(benchmark_divide_units, double, double)->Arg(8192);
BENCHMARK_TEMPLATE(benchmark_divide_units, double_wrapping_type, double_wrapping_type)->Arg(8192);

BENCHMARK_TEMPLATE(benchmark_divide_units, int64_t, double)->Arg(8192);
BENCHMARK_TEMPLATE(benchmark_divide_units, int64_t_wrapping_type, double_wrapping_type)->Arg(8192);

BENCHMARK_TEMPLATE(benchmark_divide_units, double, int64_t)->Arg(8192);
BENCHMARK_TEMPLATE(benchmark_divide_units, double_wrapping_type, int64_t_wrapping_type)->Arg(8192);

BENCHMARK_TEMPLATE(benchmark_divide_units_simd, int8_t, int8_t, 32)->Arg(8192 / 32);
BENCHMARK_TEMPLATE(benchmark_divide_units_simd, int8_t_wrapping_type, int8_t_wrapping_type, 32)->Arg(8192 / 32);

BENCHMARK_TEMPLATE(benchmark_divide_units_simd, int64_t, int64_t, 32)->Arg(8192 / 32);
BENCHMARK_TEMPLATE(benchmark_divide_units_simd, int64_t_wrapping_type, int64_t_wrapping_type, 32)->Arg(8192 / 32);

BENCHMARK_TEMPLATE(benchmark_divide_units_simd, double, double, 32)->Arg(8192 / 32);
BENCHMARK_TEMPLATE(benchmark_divide_units_simd, double_wrapping_type, double_wrapping_type, 32)->Arg(8192 / 32);

BENCHMARK_TEMPLATE(benchmark_divide_units_simd, int64_t, double, 32)->Arg(8192 / 32);
BENCHMARK_TEMPLATE(benchmark_divide_units_simd, int64_t_wrapping_type, double_wrapping_type, 32)->Arg(8192 / 32);

BENCHMARK_TEMPLATE(benchmark_divide_units_simd, double, int64_t, 32)->Arg(8192 / 32);
BENCHMARK_TEMPLATE(benchmark_divide_units_simd, double_wrapping_type, int64_t_wrapping_type, 32)->Arg(8192 / 32);
