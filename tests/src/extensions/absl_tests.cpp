#include <absl/hash/hash.h>
#include <gtest/gtest.h>
#include <stronk/extensions/absl.h>
#include <stronk/stronk.h>

namespace twig
{

struct a_can_absl_hash_type : stronk<a_can_absl_hash_type, int, can_absl_hash>
{
    using stronk::stronk;
};

TEST(can_absl_hash, can_absl_hash_adds_the_absl_hash_function)  // NOLINT
{
    for (auto i = -10; i < 10; i++) {
        EXPECT_EQ(absl::HashOf(a_can_absl_hash_type(i)), absl::HashOf(i));
    }
}

}  // namespace twig
