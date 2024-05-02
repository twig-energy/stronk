#include <cstdint>

#include <gtest/gtest.h>
#include <stronk/can_increment.h>
#include <stronk/stronk.h>

namespace twig
{

struct an_int_increment_test_type : stronk<an_int_increment_test_type, int, can_increment, can_equate>
{
    using stronk::stronk;
};

TEST(can_increment, can_increment_stronk_integer)
{
    auto a = an_int_increment_test_type {-1};

    EXPECT_EQ(++a, an_int_increment_test_type {0});
    EXPECT_EQ(a++, an_int_increment_test_type {0});
    EXPECT_EQ(a, an_int_increment_test_type {1});
}

struct a_uint64_t_increment_test_type : stronk<a_uint64_t_increment_test_type, uint64_t, can_increment, can_equate>
{
    using stronk::stronk;
};

TEST(can_increment, can_increment_stronk_uint64_t)
{
    auto a = a_uint64_t_increment_test_type {0};

    EXPECT_EQ(++a, a_uint64_t_increment_test_type {1});
    EXPECT_EQ(a++, a_uint64_t_increment_test_type {1});
    EXPECT_EQ(a, a_uint64_t_increment_test_type {2});
}

}  // namespace twig
