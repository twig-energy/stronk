
#include <cstddef>
#include <numeric>
#include <vector>

#include "stronk/skills/can_iterate.hpp"

#include <doctest/doctest.h>

#include "stronk/stronk.hpp"

namespace twig
{

struct a_can_iterate_vector_type : stronk<a_can_iterate_vector_type, std::vector<int>, can_iterate>
{
    using stronk::stronk;
};

TEST_SUITE("can_const_iterate")
{
    TEST_CASE("can_const_iterate_works_for_vectors")
    {
        for (size_t i = 0; i < 16; i++) {
            const auto vector = [&i]()
            {
                auto tmp = std::vector<int>(i);
                std::iota(tmp.begin(), tmp.end(), -8);
                return a_can_iterate_vector_type(tmp);
            }();
            auto curr = -8;
            for (const auto& val : vector) {
                CHECK_EQ(val, curr);
                curr++;
            }
        }
    }
}

TEST_SUITE("can_iterate")
{
    TEST_CASE("can_iterate_works_for_vectors")
    {
        for (size_t i = 0; i < 16; i++) {
            auto vector = [&i]()
            {
                auto tmp = std::vector<int>(i);
                std::iota(tmp.begin(), tmp.end(), -8);
                return a_can_iterate_vector_type(tmp);
            }();
            auto curr = -8;
            for (auto& val : vector) {
                val++;
                CHECK_EQ(val, curr + 1);
                curr++;
            }
        }
    }
}

}  // namespace twig
