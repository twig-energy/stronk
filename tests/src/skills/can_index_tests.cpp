
#include <cstddef>
#include <numeric>
#include <vector>

#include "stronk/skills/can_index.hpp"

#include <doctest/doctest.h>

#include "stronk/stronk.hpp"

namespace twig
{

struct a_can_index_vector_type : stronk<a_can_index_vector_type, std::vector<int>, can_index>
{
    using stronk::stronk;
};

TEST_SUITE("can_const_index")
{
    TEST_CASE("can_const_index_works_for_vectors")
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
                CHECK_EQ(vector.at(j), curr);
                CHECK_EQ(vector[j], curr);
                curr++;
            }
        }
    }
}

TEST_SUITE("can_index")
{
    TEST_CASE("can_index_works_for_vectors")
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
                CHECK_EQ(vector.at(j), curr + 2);
                CHECK_EQ(vector[j], curr + 2);
                curr++;
            }
        }
    }
}

}  // namespace twig
