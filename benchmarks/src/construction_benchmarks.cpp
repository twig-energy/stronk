#include <cstdint>
#include <string>

#include <benchmark/benchmark.h>

#include "./benchmark_helpers.h"

template<typename T>
static void benchmark_default_onto_reserved_vector(benchmark::State& state)
{
    auto vec = std::vector<T>(static_cast<size_t>(state.range(0)));
    benchmark::DoNotOptimize(vec.data());
    for (auto _ : state) {
        for (auto i = 0ULL; i < static_cast<size_t>(state.range(0)); i++) {
            vec[i] = T {};
        }
        benchmark::ClobberMemory();
    }
}

BENCHMARK_TEMPLATE(benchmark_default_onto_reserved_vector, int8_t)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_default_onto_reserved_vector, int8_t_wrapping_type)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_default_onto_reserved_vector, decltype(int8_t_wrapping_type {} * int8_t_wrapping_type {}))
    ->Range(32ULL, 8ULL << 10ULL);

BENCHMARK_TEMPLATE(benchmark_default_onto_reserved_vector, int64_t)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_default_onto_reserved_vector, int64_t_wrapping_type)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_default_onto_reserved_vector,
                   decltype(int64_t_wrapping_type {} * int64_t_wrapping_type {}))
    ->Range(32ULL, 8ULL << 10ULL);

BENCHMARK_TEMPLATE(benchmark_default_onto_reserved_vector, std::string)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_default_onto_reserved_vector, string_wrapping_type)->Range(32ULL, 8ULL << 10ULL);

template<typename T>
static void benchmark_rand_onto_reserved_vector(benchmark::State& state)
{
    auto vec = std::vector<T>(static_cast<size_t>(state.range(0)));
    for (auto _ : state) {
        benchmark::DoNotOptimize(vec.data());
        for (auto i = 0ULL; i < static_cast<size_t>(state.range(0)); i++) {
            vec[i] = generate_randomish<T> {}();
        }
        benchmark::ClobberMemory();
    }
}

BENCHMARK_TEMPLATE(benchmark_rand_onto_reserved_vector, int8_t)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_rand_onto_reserved_vector, int8_t_wrapping_type)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_rand_onto_reserved_vector, decltype(int8_t_wrapping_type {} * int8_t_wrapping_type {}))
    ->Range(32ULL, 8ULL << 10ULL);

BENCHMARK_TEMPLATE(benchmark_rand_onto_reserved_vector, int64_t)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_rand_onto_reserved_vector, int64_t_wrapping_type)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_rand_onto_reserved_vector, decltype(int64_t_wrapping_type {} * int64_t_wrapping_type {}))
    ->Range(32ULL, 8ULL << 10ULL);

BENCHMARK_TEMPLATE(benchmark_rand_onto_reserved_vector, std::string)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_rand_onto_reserved_vector, string_wrapping_type)->Range(32ULL, 8ULL << 10ULL);

template<typename T>
static void benchmark_copy_vector_of(benchmark::State& state)
{
    auto vec = std::vector<T>(static_cast<size_t>(state.range(0)));
    std::generate(vec.begin(), vec.end(), []() { return generate_randomish<T> {}(); });
    benchmark::DoNotOptimize(vec.data());
    for (auto _ : state) {
        auto copy_into = std::vector<T>();
        benchmark::DoNotOptimize(copy_into.data());
        copy_into = vec;
        benchmark::ClobberMemory();
    }
}

BENCHMARK_TEMPLATE(benchmark_copy_vector_of, int8_t)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_copy_vector_of, int8_t_wrapping_type)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_copy_vector_of, decltype(int8_t_wrapping_type {} * int8_t_wrapping_type {}))
    ->Range(32ULL, 8ULL << 10ULL);

BENCHMARK_TEMPLATE(benchmark_copy_vector_of, int64_t)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_copy_vector_of, int64_t_wrapping_type)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_copy_vector_of, decltype(int64_t_wrapping_type {} * int64_t_wrapping_type {}))
    ->Range(32ULL, 8ULL << 10ULL);

BENCHMARK_TEMPLATE(benchmark_copy_vector_of, std::string)->Range(32ULL, 8ULL << 10ULL);
BENCHMARK_TEMPLATE(benchmark_copy_vector_of, string_wrapping_type)->Range(32ULL, 8ULL << 10ULL);
