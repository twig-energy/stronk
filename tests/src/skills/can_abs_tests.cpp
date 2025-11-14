

#include <cstdlib>

#include "stronk/skills/can_abs.hpp"

#include <doctest/doctest.h>

#include "stronk/cmath.hpp"
#include "stronk/stronk.hpp"

namespace twig
{

struct an_abs_min_max_type : stronk<an_abs_min_max_type, int, can_order, can_equate>
{
    using stronk::stronk;
};

TEST_SUITE("can_abs")
{
    TEST_CASE("abs is applied correctly")
    {
        CHECK_EQ(twig::abs(an_abs_min_max_type {10}), an_abs_min_max_type {10});
        CHECK_EQ(twig::abs(an_abs_min_max_type {-10}), an_abs_min_max_type {10});
    }

    TEST_CASE("abs behaves similar to integer abs")
    {
        for (auto i = -16; i < 16; i++) {
            CHECK_EQ(an_abs_min_max_type {std::abs(i)}, twig::abs(an_abs_min_max_type {i}));
        }
    }

    struct an_underlying_type_with_custom_can_abs
    {
        int value;

        auto operator==(const an_underlying_type_with_custom_can_abs& other) const -> bool = default;
    };

    // NOLINTNEXTLINE(misc-use-internal-linkage) if in anonymous namespace ADL does not work
    constexpr auto abs(const an_underlying_type_with_custom_can_abs& v) -> an_underlying_type_with_custom_can_abs
    {
        return an_underlying_type_with_custom_can_abs {std::abs(v.value) + 1};
    }

    struct custom_abs_stronk : stronk<custom_abs_stronk, an_underlying_type_with_custom_can_abs, can_equate>
    {
        using stronk::stronk;
    };

    TEST_CASE("abs will forward to custom abs implementations")
    {
        for (auto i = -16; i < 16; i++) {
            CHECK_EQ(custom_abs_stronk {an_underlying_type_with_custom_can_abs {std::abs(i) + 1}},
                     twig::abs(custom_abs_stronk {an_underlying_type_with_custom_can_abs {i}}));
        }
    }
}

}  // namespace twig
