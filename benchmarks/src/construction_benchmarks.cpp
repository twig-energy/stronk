#include <cstdint>
#include <string>

#include <benchmark/benchmark.h>

#include "./benchmark_helpers.h"

template<typename T>
static void benchmark_default_onto_vector(benchmark::State& state)
{
    for (auto _ : state) {
        auto vec = std::vector<T>();
        for (auto i = 0; i < state.range(0); i++) {
            vec.emplace_back(T());
        }
        benchmark::DoNotOptimize(vec);
    }
}

BENCHMARK_TEMPLATE(benchmark_default_onto_vector, int8_t)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_default_onto_vector, int8_t_wrapping_type)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_default_onto_vector, decltype(int64_t_wrapping_type {} * int64_t_wrapping_type {}))
    ->Range(32ULL, 8ULL << 10ULL);

BENCHMARK_TEMPLATE(benchmark_default_onto_vector, int64_t)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_default_onto_vector, int64_t_wrapping_type)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_default_onto_vector, decltype(int64_t_wrapping_type {} * int64_t_wrapping_type {}))
    ->Range(32ULL, 8ULL << 10ULL);

BENCHMARK_TEMPLATE(benchmark_default_onto_vector, std::string)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_default_onto_vector, string_wrapping_type)->Range(32ULL, 8ULL << 10ULL);

template<typename T>
static void benchmark_rand_onto_vector(benchmark::State& state)
{
    for (auto _ : state) {
        auto vec = std::vector<T>();
        for (auto i = 0; i < state.range(0); i++) {
            vec.emplace_back(generate_randomish<T> {}());
        }
        benchmark::DoNotOptimize(vec);
    }
}

BENCHMARK_TEMPLATE(benchmark_rand_onto_vector, int8_t)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_rand_onto_vector, int8_t_wrapping_type)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_rand_onto_vector, decltype(int8_t_wrapping_type {} * int8_t_wrapping_type {}))
    ->Range(32ULL, 8ULL << 10ULL);

BENCHMARK_TEMPLATE(benchmark_rand_onto_vector, int64_t)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_rand_onto_vector, int64_t_wrapping_type)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_rand_onto_vector, decltype(int64_t_wrapping_type {} * int64_t_wrapping_type {}))
    ->Range(32ULL, 8ULL << 10ULL);

BENCHMARK_TEMPLATE(benchmark_rand_onto_vector, std::string)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_rand_onto_vector, string_wrapping_type)->Range(32ULL, 8ULL << 10ULL);
