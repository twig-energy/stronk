
#include <cstddef>
#include <numeric>
#include <vector>

#include "stronk/skills/can_index.hpp"

#include <gtest/gtest.h>

#include "stronk/stronk.hpp"

namespace twig
{

struct a_can_index_vector_type : stronk<a_can_index_vector_type, std::vector<int>, can_index>
{
    using stronk::stronk;
};

TEST(can_const_index, can_const_index_works_for_vectors)
{
    for (size_t i = 0; i < 16; i++) {
        const auto vector = [&i]()
        {
            auto tmp = std::vector<int>(i);
            std::iota(tmp.begin(), tmp.end(), -8);
            return a_can_index_vector_type(tmp);
        }();
        auto curr = -8;
        for (size_t j = 0; j < i; j++) {
            EXPECT_EQ(vector.at(j), curr);
            EXPECT_EQ(vector[j], curr);
            curr++;
        }
    }
}

TEST(can_index, can_index_works_for_vectors)
{
    for (size_t i = 0; i < 16; i++) {
        auto vector = [&i]()
        {
            auto tmp = std::vector<int>(i);
            std::iota(tmp.begin(), tmp.end(), -8);
            return a_can_index_vector_type(tmp);
        }();
        auto curr = -8;
        for (size_t j = 0; j < i; j++) {
            vector.at(j)++;
            vector[j]++;
            EXPECT_EQ(vector.at(j), curr + 2);
            EXPECT_EQ(vector[j], curr + 2);
            curr++;
        }
    }
}

}  // namespace twig
