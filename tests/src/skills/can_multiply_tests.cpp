
#include "stronk/skills/can_multiply.hpp"

#include <gtest/gtest.h>

#include "stronk/stronk.hpp"

namespace twig
{

struct a_multiplying_type : stronk<a_multiplying_type, int, can_multiply, can_equate>
{
    using stronk::stronk;
};

TEST(can_multiply, can_multiply_numbers_correctly)
{
    auto val_1 = a_multiplying_type {5};
    auto val_2 = a_multiplying_type {2};
    EXPECT_EQ(val_1 * val_2, a_multiplying_type {10});
    val_1 *= val_2;

    EXPECT_EQ(val_1 * val_2, a_multiplying_type {20});
}

TEST(can_multiply, multiplying_behaves_similar_to_integers)
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            EXPECT_EQ(a_multiplying_type {i * j}, a_multiplying_type {i} * a_multiplying_type {j});
        }
    }
}

struct a_can_multiply_with_int_and_double_type
    : stronk<a_can_multiply_with_int_and_double_type,
             double,
             can_multiply_with<int>::skill,
             can_multiply_with<double>::skill,
             can_equate_underlying_type_specific>
{
    using stronk::stronk;
};

TEST(can_multiply_with, can_multiply_with_integers_and_other_types)
{
    auto val = a_can_multiply_with_int_and_double_type {2};
    val *= 10;
    val = 10 * val;
    val = val * 10;
    EXPECT_EQ(val, a_can_multiply_with_int_and_double_type {2000});
    val *= 0.1;
    val = 0.1 * val;
    val = val * 0.1;
    EXPECT_EQ(val, a_can_multiply_with_int_and_double_type {2});
}

}  // namespace twig
