#include <concepts>

#include "stronk/utilities/ratio.hpp"

#include <gtest/gtest.h>

namespace twig
{

// supports very large numbers
static_assert(std::same_as<ratio_multiply<ratio<1'000'000'000'000>, ratio<1'000'000'000'000>>,
                           ratio<stronk_details::u_biggest_int_t {1'000'000'000'000}
                                 * stronk_details::u_biggest_int_t {1'000'000'000'000}>>);

TEST(gcd, few_examples)
{
    EXPECT_EQ(stronk_details::gcd(0, 0), 0);
    EXPECT_EQ(stronk_details::gcd(1, 0), 1);
    EXPECT_EQ(stronk_details::gcd(0, 1), 1);
    EXPECT_EQ(stronk_details::gcd(2, 4), 2);
    EXPECT_EQ(stronk_details::gcd(4, 2), 2);
    EXPECT_EQ(stronk_details::gcd(12, 15), 3);
}

}  // namespace twig
