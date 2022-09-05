#include <cstdint>
#include <string>

#include <benchmark/benchmark.h>

#include "./benchmark_helpers.h"

template<typename T>
static void benchmark_add_units(benchmark::State& state)
{
    for (auto _ : state) {
        for (auto i = 0; i < state.range(0); i++) {
            benchmark::DoNotOptimize(generate_randomish<T> {}() + generate_randomish<T> {}());
        }
    }
}

template<typename T>
static void benchmark_add_units_simd(benchmark::State& state)
{
    for (auto _ : state) {
        auto array_a = std::array<T, 32> {};
        auto array_b = std::array<T, 32> {};
        std::generate(array_a.begin(), array_a.end(), []() { return generate_randomish<T> {}(); });
        std::generate(array_b.begin(), array_b.end(), []() { return generate_randomish<T> {}(); });

        auto array_c = std::array<T, 32> {};
        for (size_t i = 0ULL; i < 32ULL; i++) {
            array_c[i] = array_a[i] + array_b[i];
        }
        benchmark::DoNotOptimize(array_c);
    }
}

// benchmark_add_units
BENCHMARK_TEMPLATE(benchmark_add_units, int8_t)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_add_units, int8_t_wrapping_type)->Range(32ULL, 8ULL << 10ULL);

BENCHMARK_TEMPLATE(benchmark_add_units, int64_t)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_add_units, int64_t_wrapping_type)->Range(32ULL, 8ULL << 10ULL);

BENCHMARK_TEMPLATE(benchmark_add_units, double)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_add_units, double_wrapping_type)->Range(32ULL, 8ULL << 10ULL);

// benchmark_add_units_simd
BENCHMARK_TEMPLATE(benchmark_add_units_simd, int8_t);
BENCHMARK_TEMPLATE(benchmark_add_units_simd, int8_t_wrapping_type);

BENCHMARK_TEMPLATE(benchmark_add_units_simd, int64_t);
BENCHMARK_TEMPLATE(benchmark_add_units_simd, int64_t_wrapping_type);

BENCHMARK_TEMPLATE(benchmark_add_units_simd, double);
BENCHMARK_TEMPLATE(benchmark_add_units_simd, double_wrapping_type);

template<typename T>
static void benchmark_subtract_units(benchmark::State& state)
{
    for (auto _ : state) {
        for (auto i = 0; i < state.range(0); i++) {
            benchmark::DoNotOptimize(generate_randomish<T> {}() - generate_randomish<T> {}());
        }
    }
}

BENCHMARK_TEMPLATE(benchmark_subtract_units, int8_t)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_subtract_units, int8_t_wrapping_type)->Range(32ULL, 8ULL << 10ULL);

BENCHMARK_TEMPLATE(benchmark_subtract_units, int64_t)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_subtract_units, int64_t_wrapping_type)->Range(32ULL, 8ULL << 10ULL);

BENCHMARK_TEMPLATE(benchmark_subtract_units, double)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_subtract_units, double_wrapping_type)->Range(32ULL, 8ULL << 10ULL);

template<typename T, typename O>
static void benchmark_multiply_units(benchmark::State& state)
{
    for (auto _ : state) {
        for (auto i = 0; i < state.range(0); i++) {
            benchmark::DoNotOptimize(generate_randomish<T> {}() * generate_randomish<O> {}());  // NOLINT
        }
    }
}

BENCHMARK_TEMPLATE(benchmark_multiply_units, int8_t, int8_t)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_multiply_units, int8_t_wrapping_type, int8_t_wrapping_type)->Range(32ULL, 8ULL << 10ULL);

BENCHMARK_TEMPLATE(benchmark_multiply_units, int64_t, int64_t)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_multiply_units, int64_t_wrapping_type, int64_t_wrapping_type)->Range(32ULL, 8ULL << 10ULL);

BENCHMARK_TEMPLATE(benchmark_multiply_units, double, double)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_multiply_units, double_wrapping_type, double_wrapping_type)->Range(32ULL, 8ULL << 10ULL);

BENCHMARK_TEMPLATE(benchmark_multiply_units, int64_t, double)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_multiply_units, int64_t_wrapping_type, double_wrapping_type)->Range(32ULL, 8ULL << 10ULL);

BENCHMARK_TEMPLATE(benchmark_multiply_units, double, int64_t)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_multiply_units, double_wrapping_type, int64_t_wrapping_type)->Range(32ULL, 8ULL << 10ULL);

template<typename T, typename O>
static void benchmark_divide_units(benchmark::State& state)
{
    for (auto _ : state) {
        for (auto i = 0; i < state.range(0); i++) {
            auto by = generate_randomish<O> {}();
            if (by <= static_cast<O>(0.001) || by >= static_cast<O>(-0.001)) {
                continue;
            }
            benchmark::DoNotOptimize(generate_randomish<T> {}() / by);  // NOLINT
        }
    }
}

BENCHMARK_TEMPLATE(benchmark_divide_units, int8_t, int8_t)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_divide_units, int8_t_wrapping_type, int8_t_wrapping_type)->Range(32ULL, 8ULL << 10ULL);

BENCHMARK_TEMPLATE(benchmark_divide_units, int64_t, int64_t)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_divide_units, int64_t_wrapping_type, int64_t_wrapping_type)->Range(32ULL, 8ULL << 10ULL);

BENCHMARK_TEMPLATE(benchmark_divide_units, double, double)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_divide_units, double_wrapping_type, double_wrapping_type)->Range(32ULL, 8ULL << 10ULL);

BENCHMARK_TEMPLATE(benchmark_divide_units, int64_t, double)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_divide_units, int64_t_wrapping_type, double_wrapping_type)->Range(32ULL, 8ULL << 10ULL);

BENCHMARK_TEMPLATE(benchmark_divide_units, double, int64_t)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_divide_units, double_wrapping_type, int64_t_wrapping_type)->Range(32ULL, 8ULL << 10ULL);
