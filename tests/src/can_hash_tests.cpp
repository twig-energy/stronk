#include <cstdint>
#include <functional>

#include "stronk/can_hash.hpp"

#include <gtest/gtest.h>

#include "stronk/stronk.hpp"

namespace twig
{

struct a_hashable_type : stronk<a_hashable_type, int64_t, can_hash>
{
    using stronk::stronk;
};

TEST(can_hash, can_hash_overloads_std_hash)  // NOLINT
{
    for (auto i = -10; i < 10; i++) {
        auto hash = std::hash<a_hashable_type> {}(a_hashable_type {i});
        auto expected_val = std::hash<int64_t> {}(i);
        EXPECT_EQ(hash, expected_val);
    }
}

}  // namespace twig
