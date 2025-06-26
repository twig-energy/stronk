
#include "stronk/skills/can_divide.hpp"

#include <doctest/doctest.h>

#include "stronk/stronk.hpp"

namespace twig
{
struct a_dividing_type : stronk<a_dividing_type, int, can_divide, can_equate>
{
    using stronk::stronk;
};

TEST_SUITE("can_divide")
{
    TEST_CASE("can_divide_numbers_correctly")
    {
        auto divideable1 = a_dividing_type {8};
        auto divideable2 = a_dividing_type {2};
        CHECK_EQ(divideable1 / divideable2, a_dividing_type {4});
        divideable1 /= divideable2;

        CHECK_EQ(divideable1 / divideable2, a_dividing_type {2});
    }

    TEST_CASE("dividing_behaves_similar_to_integers")
    {
        for (auto i = -16; i < 16; i++) {
            for (auto j = -16; j < 16; j++) {
                if (j == 0) {
                    continue;
                }
                CHECK_EQ(a_dividing_type {i / j}, a_dividing_type {i} / a_dividing_type {j});
            }
        }
    }
}

struct a_can_divide_with_int_and_double_type
    : stronk<a_can_divide_with_int_and_double_type,
             double,
             can_divide_with<int>::skill,
             can_divide_with<double>::skill,
             can_equate_underlying_type_specific>
{
    using stronk::stronk;
};

TEST_SUITE("can_divide_with")
{
    TEST_CASE("can_divide_with_integers_and_other_types")
    {
        auto val = a_can_divide_with_int_and_double_type {200};
        val /= 10;
        val = val / 10;
        CHECK_EQ(val, a_can_divide_with_int_and_double_type {2});
        val /= 0.1;
        val = val / 0.1;
        CHECK_EQ(val, a_can_divide_with_int_and_double_type {200});
    }
}
}  // namespace twig
