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

BENCHMARK_TEMPLATE(benchmark_add_units, int8_t)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_add_units, int8_t_wrapping_type)->Range(32ULL, 8ULL << 10ULL);

BENCHMARK_TEMPLATE(benchmark_add_units, int64_t)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_add_units, int64_t_wrapping_type)->Range(32ULL, 8ULL << 10ULL);

BENCHMARK_TEMPLATE(benchmark_add_units, double)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_add_units, double_wrapping_type)->Range(32ULL, 8ULL << 10ULL);

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
