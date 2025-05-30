
#include "stronk/skills/can_divide.hpp"

#include <gtest/gtest.h>

#include "stronk/stronk.hpp"

namespace twig
{
struct a_dividing_type : stronk<a_dividing_type, int, can_divide, can_equate>
{
    using stronk::stronk;
};

TEST(can_divide, can_divide_numbers_correctly)
{
    auto divideable1 = a_dividing_type {8};
    auto divideable2 = a_dividing_type {2};
    EXPECT_EQ(divideable1 / divideable2, a_dividing_type {4});
    divideable1 /= divideable2;

    EXPECT_EQ(divideable1 / divideable2, a_dividing_type {2});
}

TEST(can_divide, dividing_behaves_similar_to_integers)
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            if (j == 0) {
                continue;
            }
            EXPECT_EQ(a_dividing_type {i / j}, a_dividing_type {i} / a_dividing_type {j});
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

TEST(can_divide_with, can_divide_with_integers_and_other_types)
{
    auto val = a_can_divide_with_int_and_double_type {200};
    val /= 10;
    val = val / 10;
    EXPECT_EQ(val, a_can_divide_with_int_and_double_type {2}) << val.unwrap<a_can_divide_with_int_and_double_type>();
    val /= 0.1;
    val = val / 0.1;
    EXPECT_EQ(val, a_can_divide_with_int_and_double_type {200});
}

}  // namespace twig
