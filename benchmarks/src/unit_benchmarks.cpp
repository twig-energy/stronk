#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <vector>

#include <doctest/doctest.h>
#include <fmt/format.h>
#include <nanobench.h>

#include "./benchmark_helpers.hpp"

namespace
{

template<typename T, typename O>
constexpr void benchmark_units_operation(ankerl::nanobench::Bench& bench,
                                         size_t size,
                                         const auto& op,
                                         O o_min_val = O {})
{
    auto vec_a = std::vector<T>(size);
    auto vec_b = std::vector<O>(size);
    std::ranges::generate(vec_a, []() { return generate_randomish<T> {}(); });
    std::ranges::generate(vec_b, [&o_min_val]() { return generate_randomish<O> {}() + o_min_val; });

    bench.complexityN(size).batch(size).run(fmt::format("{}_{}", get_name<T>(), get_name<O>()),
                                            [&]()
                                            {
                                                using ResT = decltype(op(T {}, O {}));
                                                for (auto i = 0ULL; i < size; i++) {
                                                    ResT res;
                                                    ankerl::nanobench::doNotOptimizeAway(res);
                                                    res = op(vec_a[i], vec_b[i]);  // NOLINT
                                                    ankerl::nanobench::doNotOptimizeAway(res);
                                                }
                                            });
}

template<typename T, typename O, size_t WidthV>
constexpr void benchmark_units_simd_operation(ankerl::nanobench::Bench& bench,
                                              size_t size,
                                              const auto& op,
                                              O o_min_val = O {})
{
    auto vec_a = std::vector<T>(size);
    auto vec_b = std::vector<O>(size);
    std::ranges::generate(vec_a, []() { return generate_randomish<T> {}(); });
    std::ranges::generate(vec_b, [&o_min_val]() { return generate_randomish<O> {}() + o_min_val; });

    bench.complexityN(size).batch(size).run(fmt::format("{}_{}", get_name<T>(), get_name<O>()),
                                            [&]()
                                            {
                                                using ResT = decltype(op(T {}, O {}));
                                                for (auto i = 0ULL; i < size - WidthV; i++) {
                                                    auto array_c = std::array<ResT, WidthV> {};
                                                    ankerl::nanobench::doNotOptimizeAway(array_c.data());
                                                    // We expect the inner loop to be vectorized to SIMD instructions
                                                    for (size_t j = 0; j < WidthV; j++) {
                                                        array_c[j] = op(vec_a[i + j], vec_b[i + j]);  // NOLINT
                                                    }
                                                    ankerl::nanobench::doNotOptimizeAway(array_c);
                                                }
                                            });
}

template<typename T>
constexpr void benchmark_add_units(ankerl::nanobench::Bench& bench, size_t size)
{
    benchmark_units_operation<T, T>(bench, size, std::plus<> {});
}

template<typename T, size_t WidthV>
constexpr void benchmark_add_units_simd(ankerl::nanobench::Bench& bench, size_t size)
{
    benchmark_units_simd_operation<T, T, WidthV>(bench, size, std::plus<> {});
}

template<typename T>
constexpr void benchmark_subtract_units(ankerl::nanobench::Bench& bench, size_t size)
{
    benchmark_units_operation<T, T>(bench, size, std::minus<> {});
}

template<typename T, size_t WidthV>
constexpr void benchmark_subtract_units_simd(ankerl::nanobench::Bench& bench, size_t size)
{
    benchmark_units_simd_operation<T, T, WidthV>(bench, size, std::minus<> {});
}

template<typename T, typename O>
constexpr void benchmark_multiply_units(ankerl::nanobench::Bench& bench, size_t size)
{
    benchmark_units_operation<T, O>(bench, size, [](auto x, auto y) { return x * y; });  // NOLINT
}

template<typename T, typename O, size_t WidthV>
constexpr void benchmark_multiply_units_simd(ankerl::nanobench::Bench& bench, size_t size)
{
    benchmark_units_simd_operation<T, O, WidthV>(bench, size, [](auto x, auto y) { return x * y; });  // NOLINT
}

template<typename T, typename O>
constexpr void benchmark_divide_units(ankerl::nanobench::Bench& bench, size_t size)
{
    benchmark_units_operation<T, O>(bench, size, [](auto x, auto y) { return x / y; }, O {1});  // NOLINT
}

template<typename T, typename O, size_t WidthV>
constexpr void benchmark_divide_units_simd(ankerl::nanobench::Bench& bench, size_t size)
{
    benchmark_units_simd_operation<T, O, WidthV>(bench, size, [](auto x, auto y) { return x / y; }, O {1});  // NOLINT
}

}  // namespace

TEST_SUITE("Unit Operations Benchmarks")
{
    TEST_CASE("Add Units")
    {
        auto bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).performanceCounters(true);

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

    TEST_CASE("Add Units SIMD")
    {
        auto bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).performanceCounters(true);

        auto sizes = std::array {64ULL, 128ULL, 256ULL, 512ULL, 1024ULL, 2048ULL, 4096ULL, 8192ULL};
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

    TEST_CASE("Subtract Units")
    {
        auto bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).performanceCounters(true);

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

    TEST_CASE("Subtract Units SIMD")
    {
        auto bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).performanceCounters(true);

        auto sizes = std::array {64ULL, 128ULL, 256ULL, 512ULL, 1024ULL, 2048ULL, 4096ULL, 8192ULL};
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

    TEST_CASE("multiply_units_benchmarks")
    {
        auto bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).performanceCounters(true);

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

        std::cout << bench.complexityBigO() << '\n';
    }

    TEST_CASE("Multiply Units SIMD")
    {
        auto bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).performanceCounters(true);

        auto sizes = std::array {64ULL, 128ULL, 256ULL, 512ULL, 1024ULL, 2048ULL, 4096ULL, 8192ULL};
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

        std::cout << bench.complexityBigO() << '\n';
    }

    TEST_CASE("Divide Units")
    {
        auto bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).performanceCounters(true);

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

        std::cout << bench.complexityBigO() << '\n';
    }

    TEST_CASE("Divide Units SIMD")
    {
        auto bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).performanceCounters(true);

        auto sizes = std::array {64ULL, 128ULL, 256ULL, 512ULL, 1024ULL, 2048ULL, 4096ULL, 8192ULL};
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

        std::cout << bench.complexityBigO() << '\n';
    }
}
