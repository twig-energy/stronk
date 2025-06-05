

#include <vector>

#include "stronk/prefabs/stronk_vector.hpp"

#include <gtest/gtest.h>

#include "stronk/stronk.hpp"

namespace twig
{

struct a_vector_type : stronk_vector<a_vector_type, int>
{
    using stronk::stronk;
};

TEST(stronk_vector, can_be_used_as_vector)
{
    auto stronk_vector = a_vector_type {std::vector {1, 2, 4}};
    auto another = a_vector_type {std::vector {1, 2, 3}};
    EXPECT_EQ(stronk_vector.size(), 3);
    EXPECT_EQ(stronk_vector, stronk_vector);
    EXPECT_NE(stronk_vector, another);

    EXPECT_EQ(stronk_vector[0], 1);
    EXPECT_EQ(stronk_vector.at(1), 2);
    EXPECT_EQ(stronk_vector.at(2), 4);

    auto expected = 1;
    for (auto& i : stronk_vector) {
        EXPECT_EQ(i, expected);
        expected *= 2;
    }
}

}  // namespace twig
