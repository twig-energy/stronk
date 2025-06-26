#include <concepts>

#include "stronk/utilities/ratio.hpp"

#include <doctest/doctest.h>

namespace twig
{

// supports very large numbers
#if defined(__SIZEOF_INT128__)
static_assert(std::same_as<ratio_multiply<ratio<1'000'000'000'000>, ratio<1'000'000'000'000>>,
                           ratio<stronk_details::u_biggest_int_t {1'000'000'000'000}
                                 * stronk_details::u_biggest_int_t {1'000'000'000'000}>>);
#endif

static_assert(std::same_as<ratio_multiply<ratio<1, 2>, ratio<3, 4>>, ratio<3, 8>>);
static_assert(std::same_as<ratio_multiply<ratio<1, 2>, ratio<3>>, ratio<3, 2>>);
static_assert(std::same_as<ratio_multiply<ratio<1, 2>, ratio<1, 3>>, ratio<1, 6>>);
static_assert(std::same_as<ratio_multiply<ratio<1, 2>, ratio<1, 1>>, ratio<1, 2>>);

static_assert(std::same_as<ratio_divide<ratio<1, 2>, ratio<3, 4>>, ratio<2, 3>>);
static_assert(std::same_as<ratio_divide<ratio<1, 2>, ratio<3>>, ratio<1, 6>>);
static_assert(std::same_as<ratio_divide<ratio<1, 2>, ratio<1, 3>>, ratio<3, 2>>);
static_assert(std::same_as<ratio_divide<ratio<1, 2>, ratio<1, 1>>, ratio<1, 2>>);

TEST_SUITE("gcd")
{
    TEST_CASE("few_examples")
    {
        CHECK_EQ(stronk_details::gcd(0, 0), 0);
        CHECK_EQ(stronk_details::gcd(1, 0), 1);
        CHECK_EQ(stronk_details::gcd(0, 1), 1);
        CHECK_EQ(stronk_details::gcd(2, 4), 2);
        CHECK_EQ(stronk_details::gcd(4, 2), 2);
        CHECK_EQ(stronk_details::gcd(12, 15), 3);
    }
}

}  // namespace twig
