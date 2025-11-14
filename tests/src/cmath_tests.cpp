

#include <cmath>
#include <cstdlib>

#include "stronk/cmath.hpp"

#include <doctest/doctest.h>

#include "stronk/stronk.hpp"

namespace another
{

struct an_underlying_type_from_another_ns
{
    double value;

    auto operator==(const an_underlying_type_from_another_ns& other) const -> bool
    {
        return std::abs(value - other.value) < 1e-9;
    }
};

// NOLINTNEXTLINE(misc-use-internal-linkage) if in anonymous namespace ADL does not work
auto abs(const an_underlying_type_from_another_ns& v) -> an_underlying_type_from_another_ns
{
    return an_underlying_type_from_another_ns {std::abs(v.value) - 1};
}

// NOLINTNEXTLINE(misc-use-internal-linkage) if in anonymous namespace ADL does not work
auto log(const an_underlying_type_from_another_ns& v) -> an_underlying_type_from_another_ns
{
    return an_underlying_type_from_another_ns {std::log(v.value) - 1};
}

// NOLINTNEXTLINE(misc-use-internal-linkage) if in anonymous namespace ADL does not work
auto log2(const an_underlying_type_from_another_ns& v) -> an_underlying_type_from_another_ns
{
    return an_underlying_type_from_another_ns {std::log2(v.value) - 2};
}

// NOLINTNEXTLINE(misc-use-internal-linkage) if in anonymous namespace ADL does not work
auto log10(const an_underlying_type_from_another_ns& v) -> an_underlying_type_from_another_ns
{
    return an_underlying_type_from_another_ns {std::log10(v.value) - 3};
}

}  // namespace another

namespace twig
{

struct a_stronk_type : stronk<a_stronk_type, double, can_order, can_equate_with_is_close_abs_tol_only>
{
    using stronk::stronk;
};

struct stronk_with_custom_underlying_type_in_another_ns
    : stronk<stronk_with_custom_underlying_type_in_another_ns, another::an_underlying_type_from_another_ns, can_equate>
{
    using stronk::stronk;
};

TEST_SUITE("abs")
{
    TEST_CASE("abs is applied correctly")
    {
        CHECK_EQ(twig::abs(a_stronk_type {10}), a_stronk_type {10});
        CHECK_EQ(twig::abs(a_stronk_type {-10}), a_stronk_type {10});
    }

    TEST_CASE("abs behaves similar to integer abs")
    {
        for (auto i = -16; i < 16; i++) {
            CHECK_EQ(a_stronk_type {std::abs(i)}, twig::abs(a_stronk_type {i}));
        }
    }

    TEST_CASE("abs will find custom abs implementations in other namespaces via ADL")
    {
        for (auto i = -16; i < 16; i++) {
            CHECK_EQ(stronk_with_custom_underlying_type_in_another_ns {another::an_underlying_type_from_another_ns {
                         std::abs(i) - 1.0}},
                     twig::abs(stronk_with_custom_underlying_type_in_another_ns {
                         another::an_underlying_type_from_another_ns {static_cast<double>(i)}}));
        }
    }
}

TEST_SUITE("log")
{
    TEST_CASE("log works as expected for stronk types")
    {
        for (auto i = 1; i <= 1000; i *= 10) {
            CHECK_EQ(a_stronk_type {std::log(static_cast<double>(i))},
                     twig::log(a_stronk_type {static_cast<double>(i)}));
        }
    }

    TEST_CASE("log uses underlying type's log via ADL if available")
    {
        using T = stronk_with_custom_underlying_type_in_another_ns;
        using U = another::an_underlying_type_from_another_ns;
        for (auto i = 1; i <= 1000; i++) {
            CHECK_EQ(T {U {std::log(static_cast<double>(i)) - 1.0}}, twig::log(T {U {static_cast<double>(i)}}));
        }
    }
}

TEST_SUITE("log2")
{
    TEST_CASE("log2 works as expected for stronk types")
    {
        for (auto i = 1; i <= 1000; i *= 10) {
            CHECK_EQ(a_stronk_type {std::log2(static_cast<double>(i))},
                     twig::log2(a_stronk_type {static_cast<double>(i)}));
        }
    }

    TEST_CASE("log2 uses underlying type's log via ADL if available")
    {
        using T = stronk_with_custom_underlying_type_in_another_ns;
        using U = another::an_underlying_type_from_another_ns;
        for (auto i = 1; i <= 1000; i++) {
            CHECK_EQ(T {U {std::log2(static_cast<double>(i)) - 2.0}}, twig::log2(T {U {static_cast<double>(i)}}));
        }
    }
}

TEST_SUITE("log10")
{
    TEST_CASE("log10 works as expected for stronk types")
    {
        for (auto i = 1; i <= 1000; i *= 10) {
            CHECK_EQ(a_stronk_type {std::log10(static_cast<double>(i))},
                     twig::log10(a_stronk_type {static_cast<double>(i)}));
        }
    }

    TEST_CASE("log10 uses underlying type's log via ADL if available")
    {
        using T = stronk_with_custom_underlying_type_in_another_ns;
        using U = another::an_underlying_type_from_another_ns;
        for (auto i = 1; i <= 1000; i++) {
            CHECK_EQ(T {U {std::log10(static_cast<double>(i)) - 3.0}}, twig::log10(T {U {static_cast<double>(i)}}));
        }
    }
}

}  // namespace twig
