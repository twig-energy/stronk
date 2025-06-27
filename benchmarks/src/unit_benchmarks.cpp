#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

#include <doctest/doctest.h>
#include <fmt/format.h>
#include <nanobench.h>

#include "./benchmark_helpers.hpp"

namespace
{

struct add
{
    constexpr static auto name = "+";

    template<typename T, typename O>
    constexpr auto operator()(T a, O b) const noexcept -> auto
    {
        return a + b;
    }
};

struct subtract
{
    constexpr static auto name = "-";

    template<typename T, typename O>
    constexpr auto operator()(T a, O b) const noexcept -> auto
    {
        return a - b;
    }
};

struct multiply
{
    constexpr static auto name = "*";

    template<typename T, typename O>
    constexpr auto operator()(T a, O b) const noexcept -> auto
    {
        return a * b;
    }
};

struct divide
{
    constexpr static auto name = "/";

    template<typename T, typename O>
    constexpr auto operator()(T a, O b) const -> auto
    {
        return a / b;
    }
};

template<typename T, typename O, typename Op>
void benchmark_units_operation(ankerl::nanobench::Bench& bench, size_t size, const Op& op, O o_min_val = O {})
{
    auto vec_a = std::vector<T>(size);
    auto vec_b = std::vector<O>(size);
    std::ranges::generate(vec_a, []() { return generate_randomish<T> {}(); });
    std::ranges::generate(vec_b, [&o_min_val]() { return generate_randomish<O> {}() + o_min_val; });

    auto tmp = decltype(op(T {}, O {})) {};
    bench.batch(size).run(fmt::format("{} {} {}", get_name<T>(), Op::name, get_name<O>()),
                          [&vec_a, &vec_b, &op, &tmp]()
                          {
                              for (auto i = 0ULL; i < vec_a.size(); i++) {
                                  tmp = op(vec_a[i], vec_b[i]);  // NOLINT
                                  ankerl::nanobench::doNotOptimizeAway(tmp);
                              }
                          });
}

template<typename T, typename O, size_t WidthV, typename Op>
void benchmark_units_simd_operation(ankerl::nanobench::Bench& bench, size_t size, const Op& op, O o_min_val = O {})
{
    auto vec_a = std::vector<T>(size);
    auto vec_b = std::vector<O>(size);
    std::ranges::generate(vec_a, []() { return generate_randomish<T> {}(); });
    std::ranges::generate(vec_b, [&o_min_val]() { return generate_randomish<O> {}() + o_min_val; });

    using ResT = decltype(op(T {}, O {}));
    auto array_c = std::array<ResT, WidthV> {};
    bench.batch(size * WidthV - WidthV)
        .run(fmt::format("{} {} {}", get_name<T>(), Op::name, get_name<O>()),
             [&vec_a, &vec_b, &op, &array_c]()
             {
                 for (auto i = 0ULL; i < vec_a.size() - WidthV; i++) {
                     // We expect the inner loop to be vectorized to SIMD instructions
                     for (size_t j = 0; j < WidthV; j++) {
                         array_c[j] = op(vec_a[i + j], vec_b[i + j]);  // NOLINT
                     }
                     for (size_t j = 0; j < WidthV; j++) {
                         ankerl::nanobench::doNotOptimizeAway(array_c[j]);
                     }
                 }
             });
}

template<typename T>
void benchmark_add_units(ankerl::nanobench::Bench& bench, size_t size)
{
    benchmark_units_operation<T, T>(bench, size, add {});
}

template<typename T, size_t WidthV>
void benchmark_add_units_simd(ankerl::nanobench::Bench& bench, size_t size)
{
    benchmark_units_simd_operation<T, T, WidthV>(bench, size, add {});
}

template<typename T>
void benchmark_subtract_units(ankerl::nanobench::Bench& bench, size_t size)
{
    benchmark_units_operation<T, T>(bench, size, subtract {});
}

template<typename T, size_t WidthV>
void benchmark_subtract_units_simd(ankerl::nanobench::Bench& bench, size_t size)
{
    benchmark_units_simd_operation<T, T, WidthV>(bench, size, subtract {});
}

template<typename T, typename O>
void benchmark_multiply_units(ankerl::nanobench::Bench& bench, size_t size)
{
    benchmark_units_operation<T, O>(bench, size, multiply {});  // NOLINT
}

template<typename T, typename O, size_t WidthV>
void benchmark_multiply_units_simd(ankerl::nanobench::Bench& bench, size_t size)
{
    benchmark_units_simd_operation<T, O, WidthV>(bench, size, multiply {});  // NOLINT
}

template<typename T, typename O>
void benchmark_divide_units(ankerl::nanobench::Bench& bench, size_t size)
{
    benchmark_units_operation<T, O>(bench, size, divide {}, O {1});  // NOLINT
}

template<typename T, typename O, size_t WidthV>
void benchmark_divide_units_simd(ankerl::nanobench::Bench& bench, size_t size)
{
    benchmark_units_simd_operation<T, O, WidthV>(bench, size, divide {}, O {1});  // NOLINT
}

}  // namespace

TEST_SUITE("Unit Operations Benchmarks")
{
    TEST_CASE("Add Units")
    {
        auto size = 8192ULL;

        auto bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_add_units<int8_t>(bench, size);
        benchmark_add_units<stronk_int8_t>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_add_units<int64_t>(bench, size);
        benchmark_add_units<stronk_int64_t>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_add_units<double>(bench, size);
        benchmark_add_units<stronk_double_t>(bench, size);
    }

    TEST_CASE("Add Units SIMD")
    {
        auto size = 8192ULL;

        auto bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_add_units_simd<int8_t, 32>(bench, size);
        benchmark_add_units_simd<stronk_int8_t, 32>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_add_units_simd<int64_t, 32>(bench, size);
        benchmark_add_units_simd<stronk_int64_t, 32>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_add_units_simd<double, 32>(bench, size);
        benchmark_add_units_simd<stronk_double_t, 32>(bench, size);
    }

    TEST_CASE("Subtract Units")
    {
        auto size = 8192ULL;

        auto bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_subtract_units<int8_t>(bench, size);
        benchmark_subtract_units<stronk_int8_t>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_subtract_units<int64_t>(bench, size);
        benchmark_subtract_units<stronk_int64_t>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_subtract_units<double>(bench, size);
        benchmark_subtract_units<stronk_double_t>(bench, size);
    }

    TEST_CASE("Subtract Units SIMD")
    {
        auto size = 8192ULL;

        auto bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_subtract_units_simd<int8_t, 32>(bench, size);
        benchmark_subtract_units_simd<stronk_int8_t, 32>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_subtract_units_simd<int64_t, 32>(bench, size);
        benchmark_subtract_units_simd<stronk_int64_t, 32>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_subtract_units_simd<double, 32>(bench, size);
        benchmark_subtract_units_simd<stronk_double_t, 32>(bench, size);
    }

    TEST_CASE("multiply_units_benchmarks")
    {
        auto size = 8192ULL;

        auto bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_multiply_units<int8_t, int8_t>(bench, size);
        benchmark_multiply_units<stronk_int8_t, stronk_int8_t>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_multiply_units<int64_t, int64_t>(bench, size);
        benchmark_multiply_units<stronk_int64_t, stronk_int64_t>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_multiply_units<double, double>(bench, size);
        benchmark_multiply_units<stronk_double_t, stronk_double_t>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_multiply_units<int64_t, double>(bench, size);
        benchmark_multiply_units<stronk_int64_t, stronk_double_t>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_multiply_units<double, int64_t>(bench, size);
        benchmark_multiply_units<stronk_double_t, stronk_int64_t>(bench, size);
    }

    TEST_CASE("Multiply Units SIMD")
    {
        auto size = 8192ULL;

        auto bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_multiply_units_simd<int8_t, int8_t, 32>(bench, size);
        benchmark_multiply_units_simd<stronk_int8_t, stronk_int8_t, 32>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_multiply_units_simd<int64_t, int64_t, 32>(bench, size);
        benchmark_multiply_units_simd<stronk_int64_t, stronk_int64_t, 32>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_multiply_units_simd<double, double, 32>(bench, size);
        benchmark_multiply_units_simd<stronk_double_t, stronk_double_t, 32>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_multiply_units_simd<int64_t, double, 32>(bench, size);
        benchmark_multiply_units_simd<stronk_int64_t, stronk_double_t, 32>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_multiply_units_simd<double, int64_t, 32>(bench, size);
        benchmark_multiply_units_simd<stronk_double_t, stronk_int64_t, 32>(bench, size);
    }

    TEST_CASE("Divide Units")
    {
        auto size = 8192ULL;

        auto bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_divide_units<int8_t, int8_t>(bench, size);
        benchmark_divide_units<stronk_int8_t, stronk_int8_t>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_divide_units<int64_t, int64_t>(bench, size);
        benchmark_divide_units<stronk_int64_t, stronk_int64_t>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_divide_units<double, double>(bench, size);
        benchmark_divide_units<stronk_double_t, stronk_double_t>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_divide_units<int64_t, double>(bench, size);
        benchmark_divide_units<stronk_int64_t, stronk_double_t>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_divide_units<double, int64_t>(bench, size);
        benchmark_divide_units<stronk_double_t, stronk_int64_t>(bench, size);
    }

    TEST_CASE("Divide Units SIMD")
    {
        auto size = 8192ULL;

        auto bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_divide_units_simd<int8_t, int8_t, 32>(bench, size);
        benchmark_divide_units_simd<stronk_int8_t, stronk_int8_t, 32>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_divide_units_simd<int64_t, int64_t, 32>(bench, size);
        benchmark_divide_units_simd<stronk_int64_t, stronk_int64_t, 32>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_divide_units_simd<double, double, 32>(bench, size);
        benchmark_divide_units_simd<stronk_double_t, stronk_double_t, 32>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_divide_units_simd<int64_t, double, 32>(bench, size);
        benchmark_divide_units_simd<stronk_int64_t, stronk_double_t, 32>(bench, size);

        bench = ankerl::nanobench::Bench {}.title(current_test_name()).warmup(100).relative(true);
        benchmark_divide_units_simd<double, int64_t, 32>(bench, size);
        benchmark_divide_units_simd<stronk_double_t, stronk_int64_t, 32>(bench, size);
    }
}
