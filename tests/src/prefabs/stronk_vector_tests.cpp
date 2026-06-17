

#include "stronk/prefabs/stronk_vector.hpp"

#include <doctest/doctest.h>

#include "stronk/stronk.hpp"

namespace twig
{

struct a_vector_type : stronk_vector<a_vector_type, int>
{
    using stronk::stronk;
};

TEST_SUITE("stronk_vector")
{
    TEST_CASE("can_be_used_as_vector")
    {
        auto stronk_vector = a_vector_type {1, 2, 4};
        auto another = a_vector_type {1, 2, 3};
        CHECK_EQ(stronk_vector.size(), 3);
        CHECK_EQ(stronk_vector, stronk_vector);
        CHECK_NE(stronk_vector, another);

        CHECK_EQ(stronk_vector[0], 1);
        CHECK_EQ(stronk_vector.at(1), 2);
        CHECK_EQ(stronk_vector.at(2), 4);

        auto expected = 1;
        for (const auto& i : stronk_vector) {
            CHECK_EQ(i, expected);
            expected *= 2;
        }
    }

    TEST_CASE("can be converted to span and back")
    {
        auto stronk_vector = a_vector_type {1, 2, 4};
        auto mutating_func = [](a_vector_type::mutable_view_t view) -> void
        {
            CHECK_EQ(view.size(), 3);
            CHECK_EQ(view[0], 1);
            CHECK_EQ(view[1], 2);
            CHECK_EQ(view[2], 4);

            view[0] = 5;
            view[1] = 6;
            view[2] = 7;
        };
        mutating_func(stronk_vector);
        CHECK_EQ(stronk_vector[0], 5);
        CHECK_EQ(stronk_vector[1], 6);
        CHECK_EQ(stronk_vector[2], 7);

        const auto const_copy = stronk_vector;
        auto func = [](a_vector_type::view_t view) -> void
        {
            CHECK_EQ(view.size(), 3);
            CHECK_EQ(view[0], 5);
            CHECK_EQ(view[1], 6);
            CHECK_EQ(view[2], 7);
        };
        func(const_copy);
    }
}

}  // namespace twig
