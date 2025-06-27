#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <doctest/doctest.h>
#include <nanobench.h>

#include "./benchmark_helpers.hpp"

namespace
{
template<typename T>
void benchmark_default_onto_reserved_vector(ankerl::nanobench::Bench& bench, size_t size)
{
    auto vec = std::vector<T>(size);
    bench.batch(size).run(get_name<T>(),
                          [&]()
                          {
                              ankerl::nanobench::doNotOptimizeAway(vec.data());
                              for (auto i = 0ULL; i < size; i++) {
                                  vec[i] = T {};
                              }
                              ankerl::nanobench::doNotOptimizeAway(vec);
                          });
}

template<typename T>
void benchmark_rand_onto_reserved_vector(ankerl::nanobench::Bench& bench, size_t size)
{
    auto vec = std::vector<T>(size);
    bench.batch(size).run(get_name<T>(),
                          [&]()
                          {
                              ankerl::nanobench::doNotOptimizeAway(vec.data());
                              for (auto i = 0ULL; i < size; i++) {
                                  vec[i] = generate_randomish<T> {}();
                              }
                              ankerl::nanobench::doNotOptimizeAway(vec);
                          });
}

template<typename T>
void benchmark_copy_vector_of(ankerl::nanobench::Bench& bench, size_t size)
{
    auto vec = std::vector<T>(size);
    std::generate(vec.begin(), vec.end(), []() { return generate_randomish<T> {}(); });
    bench.batch(size).run(get_name<T>(),
                          [&]()
                          {
                              ankerl::nanobench::doNotOptimizeAway(vec.data());
                              auto copy_into = vec;
                              ankerl::nanobench::doNotOptimizeAway(copy_into.data());
                          });
}

}  // namespace

TEST_SUITE("construction benchmarks")
{
    TEST_CASE("Default Construction onto Reserved Vector")
    {
        auto size = 8192ULL;

        auto bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(50).relative(true);
        benchmark_default_onto_reserved_vector<int8_t>(bench, size);
        benchmark_default_onto_reserved_vector<int8_t_wrapping_type>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(50).relative(true);
        benchmark_default_onto_reserved_vector<int64_t>(bench, size);
        benchmark_default_onto_reserved_vector<int64_t_wrapping_type>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(50).relative(true);
        benchmark_default_onto_reserved_vector<std::string>(bench, size);
        benchmark_default_onto_reserved_vector<string_wrapping_type>(bench, size);
    }

    TEST_CASE("Random Construction onto Reserved Vector")
    {
        auto size = 8192ULL;

        auto bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_rand_onto_reserved_vector<int8_t>(bench, size);
        benchmark_rand_onto_reserved_vector<int8_t_wrapping_type>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_rand_onto_reserved_vector<int64_t>(bench, size);
        benchmark_rand_onto_reserved_vector<int64_t_wrapping_type>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_rand_onto_reserved_vector<std::string>(bench, size);
        benchmark_rand_onto_reserved_vector<string_wrapping_type>(bench, size);
    }

    TEST_CASE("Copy Vector Benchmarks")
    {
        auto size = 8192ULL;

        auto bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_copy_vector_of<int8_t>(bench, size);
        benchmark_copy_vector_of<int8_t_wrapping_type>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_copy_vector_of<int64_t>(bench, size);
        benchmark_copy_vector_of<int64_t_wrapping_type>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_copy_vector_of<std::string>(bench, size);
        benchmark_copy_vector_of<string_wrapping_type>(bench, size);
    }
}
