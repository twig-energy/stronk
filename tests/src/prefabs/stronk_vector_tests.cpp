

#include <ranges>

#include "stronk/prefabs/stronk_vector.hpp"

#include <gtest/gtest.h>

#include "stronk/stronk.hpp"

namespace twig
{

struct a_vector_type : stronk_vector<a_vector_type, int>
{
    using stronk::stronk;
};

static_assert(a_vector_type {1, 2, 4}.size() == 3);

TEST(a_vector_type, can_be_used_as_vector)
{
    auto stronk_vector = a_vector_type {{1, 2, 4}};
    auto another = a_vector_type {1, 2, 4};
    EXPECT_EQ(stronk_vector.size(), 3);
    EXPECT_EQ(stronk_vector, stronk_vector);
    EXPECT_NE(stronk_vector, another);

    EXPECT_EQ(stronk_vector[0], 1);
    EXPECT_EQ(stronk_vector.at(1), 2);
    EXPECT_EQ(stronk_vector.at(2), 4);

    auto transformed =
        stronk_vector | std::ranges::views::transform([](int i) { return i * 2; }) | std::ranges::to<a_vector_type>();
    auto doubled = a_vector_type {2, 4, 8};
    EXPECT_EQ(transformed, doubled);
}

}  // namespace twig
