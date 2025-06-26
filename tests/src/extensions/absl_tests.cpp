#include "stronk/extensions/absl.hpp"

#include <absl/hash/hash.h>
#include <doctest/doctest.h>

#include "stronk/stronk.hpp"

namespace twig
{

struct a_can_absl_hash_type : stronk<a_can_absl_hash_type, int, can_absl_hash>
{
    using stronk::stronk;
};

TEST_SUITE("can_absl_hash")
{
    TEST_CASE("can_absl_hash_adds_the_absl_hash_function")  // NOLINT
    {
        for (auto i = -10; i < 10; i++) {
            CHECK_EQ(absl::HashOf(a_can_absl_hash_type(i)), absl::HashOf(i));
        }
    }
}

}  // namespace twig
