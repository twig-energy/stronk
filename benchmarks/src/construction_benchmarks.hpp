#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include <nanobench.h>

#include "./benchmark_helpers.hpp"

template<typename T>
void benchmark_default_onto_reserved_vector(ankerl::nanobench::Bench& bench, size_t size)
{
    auto vec = std::vector<T>(size);
    bench.run(
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
    for (auto size : {32ULL, 64ULL, 128ULL, 256ULL, 512ULL, 1024ULL, 2048ULL, 4096ULL, 8192ULL}) {
        ankerl::nanobench::Bench().title("default_onto_reserved_vector_int8_t").relative(true);
        benchmark_default_onto_reserved_vector<int8_t>(
            ankerl::nanobench::Bench().title("default_onto_reserved_vector_int8_t"), size);
        benchmark_default_onto_reserved_vector<int8_t_wrapping_type>(
            ankerl::nanobench::Bench().title("default_onto_reserved_vector_int8_t_wrapping_type"), size);

        benchmark_default_onto_reserved_vector<int64_t>(
            ankerl::nanobench::Bench().title("default_onto_reserved_vector_int64_t"), size);
        benchmark_default_onto_reserved_vector<int64_t_wrapping_type>(
            ankerl::nanobench::Bench().title("default_onto_reserved_vector_int64_t_wrapping_type"), size);

        benchmark_default_onto_reserved_vector<std::string>(
            ankerl::nanobench::Bench().title("default_onto_reserved_vector_string"), size);
        benchmark_default_onto_reserved_vector<string_wrapping_type>(
            ankerl::nanobench::Bench().title("default_onto_reserved_vector_string_wrapping_type"), size);
    }
}

template<typename T>
void benchmark_rand_onto_reserved_vector(ankerl::nanobench::Bench& bench, size_t size)
{
    auto vec = std::vector<T>(size);
    bench.run(
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
    for (auto size : {32ULL, 64ULL, 128ULL, 256ULL, 512ULL, 1024ULL, 2048ULL, 4096ULL, 8192ULL}) {
        benchmark_rand_onto_reserved_vector<int8_t>(
            ankerl::nanobench::Bench().title("rand_onto_reserved_vector_int8_t"), size);
        benchmark_rand_onto_reserved_vector<int8_t_wrapping_type>(
            ankerl::nanobench::Bench().title("rand_onto_reserved_vector_int8_t_wrapping_type"), size);

        benchmark_rand_onto_reserved_vector<int64_t>(
            ankerl::nanobench::Bench().title("rand_onto_reserved_vector_int64_t"), size);
        benchmark_rand_onto_reserved_vector<int64_t_wrapping_type>(
            ankerl::nanobench::Bench().title("rand_onto_reserved_vector_int64_t_wrapping_type"), size);

        benchmark_rand_onto_reserved_vector<std::string>(
            ankerl::nanobench::Bench().title("rand_onto_reserved_vector_string"), size);
        benchmark_rand_onto_reserved_vector<string_wrapping_type>(
            ankerl::nanobench::Bench().title("rand_onto_reserved_vector_string_wrapping_type"), size);
    }
}

template<typename T>
void benchmark_copy_vector_of(ankerl::nanobench::Bench& bench, size_t size)
{
    auto vec = std::vector<T>(size);
    std::generate(vec.begin(), vec.end(), []() { return generate_randomish<T> {}(); });
    bench.run(
        [&]()
        {
            ankerl::nanobench::doNotOptimizeAway(vec.data());
            auto copy_into = vec;
            ankerl::nanobench::doNotOptimizeAway(copy_into.data());
        });
}

inline void run_copy_vector_of_benchmarks()
{
    for (auto size : {32ULL, 64ULL, 128ULL, 256ULL, 512ULL, 1024ULL, 2048ULL, 4096ULL, 8192ULL}) {
        benchmark_copy_vector_of<int8_t>(ankerl::nanobench::Bench().title("copy_vector_of_int8_t"), size);
        benchmark_copy_vector_of<int8_t_wrapping_type>(
            ankerl::nanobench::Bench().title("copy_vector_of_int8_t_wrapping_type"), size);

        benchmark_copy_vector_of<int64_t>(ankerl::nanobench::Bench().title("copy_vector_of_int64_t"), size);
        benchmark_copy_vector_of<int64_t_wrapping_type>(
            ankerl::nanobench::Bench().title("copy_vector_of_int64_t_wrapping_type"), size);

        benchmark_copy_vector_of<std::string>(ankerl::nanobench::Bench().title("copy_vector_of_string"), size);
        benchmark_copy_vector_of<string_wrapping_type>(
            ankerl::nanobench::Bench().title("copy_vector_of_string_wrapping_type"), size);
    }
}
