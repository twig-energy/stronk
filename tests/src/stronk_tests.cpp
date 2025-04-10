#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>
#include <stronk/prefabs.h>
#include <stronk/stronk.h>

namespace twig
{
struct an_int_test_type : stronk<an_int_test_type, int>
{
    using stronk::stronk;
};

struct a_float_test_type : stronk<a_float_test_type, float>
{
    using stronk::stronk;
};

TEST(unwrap, when_unwrapping_it_works_with_its_own_argument)
{
    auto a = an_int_test_type {42};
    EXPECT_EQ(a.unwrap<an_int_test_type>(), 42);

    auto b = a_float_test_type {42.1F};
    EXPECT_EQ(b.unwrap<a_float_test_type>(), 42.1F);
}

TEST(transform, transform_gets_called_on_the_underlying_type_and_returns_a_new_copy)
{
    auto val = an_int_test_type {2};
    val = val.transform([](auto v) { return v * 10; }).transform([](auto v) { return v + 22; });
    EXPECT_EQ(val, has_transform_skill_type {42});
}

template<an_int_test_type Val>
struct type_which_requires_stronk_none_type_template_param
{
    constexpr static an_int_test_type value = Val;
};

TEST(none_type_template_parameter, stronk_types_can_be_used_as_none_type_template_parameters)
{
    auto val = type_which_requires_stronk_none_type_template_param<an_int_test_type {25}> {};
    EXPECT_EQ(val.value.unwrap<an_int_test_type>(), 25);
}

struct mark_if_moved_type
{
    bool* indicator;

    explicit mark_if_moved_type(bool* indicator_)
        : indicator(indicator_)
    {
    }

    mark_if_moved_type(const mark_if_moved_type& o) = default;
    mark_if_moved_type(mark_if_moved_type&& s) noexcept
        : indicator(s.indicator)
    {
        (*this->indicator) = true;
        s.indicator = nullptr;
    }

    auto operator=(const mark_if_moved_type&) -> mark_if_moved_type& = delete;
    auto operator=(mark_if_moved_type&&) noexcept -> mark_if_moved_type& = delete;
    ~mark_if_moved_type() = default;
};
struct a_move_indicating_type : stronk<a_move_indicating_type, mark_if_moved_type>
{
    using stronk::stronk;
};

TEST(move, stronk_allows_to_move_and_move_out_with_unwrap)
{
    {  // wrapping type
        auto marker = false;
        auto val = a_move_indicating_type(mark_if_moved_type {&marker});
        marker = false;

        [[maybe_unused]]
        auto copy = val;
        EXPECT_FALSE(marker);
        auto moved = std::move(val);
        EXPECT_TRUE(marker);
    }

    {  // underlying type
        auto marker = false;
        auto val = a_move_indicating_type {mark_if_moved_type {&marker}};
        marker = false;

        [[maybe_unused]]
        auto copy = val.unwrap<a_move_indicating_type>();
        EXPECT_FALSE(marker);
        auto moved = std::move(val.unwrap<a_move_indicating_type>());
        EXPECT_TRUE(marker);
    }
}

struct a_string_type : stronk<a_string_type, std::string>
{
    using stronk::stronk;
};

TEST(constructor, can_construct_from_both_rvalue_lvalues_and_forwarded)
{
    auto str = std::string {"yoyo"};
    auto stronked_copy = a_string_type {str};
    EXPECT_EQ(stronked_copy.unwrap<a_string_type>(), str);

    auto stronked_moved = a_string_type {std::string {"lolo"}};
    EXPECT_EQ(stronked_moved.unwrap<a_string_type>(), "lolo");

    auto stronked_forward = a_string_type {"soso"};
    EXPECT_EQ(stronked_forward.unwrap<a_string_type>(), "soso");
}

struct implicit_wrapper
{
    double d;

    operator double() const  // NOLINT(google-explicit-constructor, hicpp-explicit-conversions) because that's the test
    {
        return this->d;
    }
};

struct explicit_wrapper
{
    double d;

    explicit operator double() const
    {
        return this->d;
    }
};

struct a_double_type : stronk<a_double_type, double>
{
    using stronk::stronk;
};

// no implicit conversions
static_assert(!std::convertible_to<double, a_double_type>);
static_assert(!std::convertible_to<implicit_wrapper, a_double_type>);
static_assert(!std::convertible_to<explicit_wrapper, a_double_type>);

// but can be constructed from implicitly convertible types
static_assert(std::constructible_from<a_double_type, double>);
static_assert(std::constructible_from<a_double_type, implicit_wrapper>);
static_assert(!std::constructible_from<a_double_type, explicit_wrapper>);

struct a_type_with_multiple_members
{
    int a;
    double b;

    a_type_with_multiple_members(int a, double b)
        : a(a)
        , b(b)
    {
    }
};

struct wrapping_multiple_member_type : stronk<wrapping_multiple_member_type, a_type_with_multiple_members>
{
    using stronk::stronk;
};

TEST(can_forward_multiple_args, correctly_passes_multiple_args_in_constructor)
{
    auto val = wrapping_multiple_member_type {1, 2.1};
    EXPECT_EQ(val.unwrap<wrapping_multiple_member_type>().a, 1);
    EXPECT_EQ(val.unwrap<wrapping_multiple_member_type>().b, 2.1);
}

struct an_orderable_type : stronk<an_orderable_type, int, can_order>
{
    using stronk::stronk;
};

TEST(can_order, when_given_an_array_of_orderable_it_can_be_sorted)
{
    auto arr = std::array<an_orderable_type, 5> {an_orderable_type {5},
                                                 an_orderable_type {4},
                                                 an_orderable_type {3},
                                                 an_orderable_type {1},
                                                 an_orderable_type {2}};

    std::sort(arr.begin(), arr.end());
    EXPECT_TRUE(std::is_sorted(arr.begin(), arr.end()));
}

TEST(can_order, order_behaves_similar_to_integers)
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            EXPECT_EQ(i <=> j, an_orderable_type {i} <=> an_orderable_type {j});
        }
    }
}

struct an_orderable_and_equatable_type : stronk<an_orderable_and_equatable_type, int, can_order, can_equate>
{
    using stronk::stronk;
};

TEST(can_order, enables_min_max_behavior_correctly)
{
    EXPECT_EQ(std::min(an_orderable_and_equatable_type {10}, an_orderable_and_equatable_type {5}),
              an_orderable_and_equatable_type {5});
    EXPECT_EQ(std::min(an_orderable_and_equatable_type {5}, an_orderable_and_equatable_type {10}),
              an_orderable_and_equatable_type {5});
    EXPECT_EQ(std::max(an_orderable_and_equatable_type {10}, an_orderable_and_equatable_type {5}),
              an_orderable_and_equatable_type {10});
    EXPECT_EQ(std::max(an_orderable_and_equatable_type {5}, an_orderable_and_equatable_type {10}),
              an_orderable_and_equatable_type {10});
}

TEST(can_order, min_max_behaves_similar_to_integers)
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            EXPECT_EQ(an_orderable_and_equatable_type {std::min(i, j)},
                      std::min(an_orderable_and_equatable_type {i}, an_orderable_and_equatable_type {j}));
            EXPECT_EQ(an_orderable_and_equatable_type {std::max(i, j)},
                      std::max(an_orderable_and_equatable_type {i}, an_orderable_and_equatable_type {j}));
        }
    }
}

struct an_equatable_type : stronk<an_equatable_type, int, can_equate>
{
    using stronk::stronk;
};

TEST(can_equate, can_equate_numbers_correctly)
{
    EXPECT_EQ(an_equatable_type {-1}, an_equatable_type {-1});
    EXPECT_NE(an_equatable_type {-1}, an_equatable_type {0});
    EXPECT_NE(an_equatable_type {-1}, an_equatable_type {1});

    EXPECT_NE(an_equatable_type {0}, an_equatable_type {-1});
    EXPECT_EQ(an_equatable_type {0}, an_equatable_type {0});
    EXPECT_NE(an_equatable_type {0}, an_equatable_type {1});

    EXPECT_NE(an_equatable_type {1}, an_equatable_type {-1});
    EXPECT_NE(an_equatable_type {1}, an_equatable_type {0});
    EXPECT_EQ(an_equatable_type {1}, an_equatable_type {1});

    EXPECT_EQ(an_equatable_type {1247865}, an_equatable_type {1247865});
    EXPECT_NE(an_equatable_type {1247865}, an_equatable_type {918256});
}

TEST(can_equate, equate_behaves_similar_to_integers)
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            EXPECT_EQ(i == j, an_equatable_type {i} == an_equatable_type {j});
        }
    }
}

struct a_less_than_greater_than_type : stronk<a_less_than_greater_than_type, int, can_less_than_greater_than>
{
    using stronk::stronk;
};

TEST(a_less_than_greater_than_type, can_less_than_correctly)
{
    EXPECT_FALSE(a_less_than_greater_than_type {-1} < a_less_than_greater_than_type {-1});
    EXPECT_LT(a_less_than_greater_than_type {-1}, a_less_than_greater_than_type {0});
    EXPECT_LT(a_less_than_greater_than_type {-1}, a_less_than_greater_than_type {1});

    EXPECT_FALSE(a_less_than_greater_than_type {0} < a_less_than_greater_than_type {-1});
    EXPECT_FALSE(a_less_than_greater_than_type {0} < a_less_than_greater_than_type {0});
    EXPECT_LT(a_less_than_greater_than_type {0}, a_less_than_greater_than_type {1});

    EXPECT_FALSE(a_less_than_greater_than_type {1} < a_less_than_greater_than_type {-1});
    EXPECT_FALSE(a_less_than_greater_than_type {1} < a_less_than_greater_than_type {0});
    EXPECT_FALSE(a_less_than_greater_than_type {1} < a_less_than_greater_than_type {1});

    EXPECT_FALSE(a_less_than_greater_than_type {1247865} < a_less_than_greater_than_type {1247865});
    EXPECT_FALSE(a_less_than_greater_than_type {1247865} < a_less_than_greater_than_type {918256});
}
TEST(a_less_than_greater_than_type, less_than_behaves_similar_to_integers)
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            EXPECT_EQ(i < j, a_less_than_greater_than_type {i} < a_less_than_greater_than_type {j});
        }
    }
}

TEST(a_less_than_greater_than_type, can_greater_than_correctly)
{
    EXPECT_FALSE(a_less_than_greater_than_type {-1} > a_less_than_greater_than_type {-1});
    EXPECT_FALSE(a_less_than_greater_than_type {-1} > a_less_than_greater_than_type {0});
    EXPECT_FALSE(a_less_than_greater_than_type {-1} > a_less_than_greater_than_type {1});

    EXPECT_GT(a_less_than_greater_than_type {0}, a_less_than_greater_than_type {-1});
    EXPECT_FALSE(a_less_than_greater_than_type {0} > a_less_than_greater_than_type {0});
    EXPECT_FALSE(a_less_than_greater_than_type {0} > a_less_than_greater_than_type {1});

    EXPECT_GT(a_less_than_greater_than_type {1}, a_less_than_greater_than_type {-1});
    EXPECT_GT(a_less_than_greater_than_type {1}, a_less_than_greater_than_type {0});
    EXPECT_FALSE(a_less_than_greater_than_type {1} > a_less_than_greater_than_type {1});

    EXPECT_FALSE(a_less_than_greater_than_type {1247865} > a_less_than_greater_than_type {1247865});
    EXPECT_GT(a_less_than_greater_than_type {1247865}, a_less_than_greater_than_type {918256});
}

TEST(a_less_than_greater_than_type, greater_than_behaves_similar_to_integers)
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            EXPECT_EQ(i > j, a_less_than_greater_than_type {i} > a_less_than_greater_than_type {j});
        }
    }
}

struct a_less_than_greater_than_or_equals_type
    : stronk<a_less_than_greater_than_or_equals_type, int, can_less_than_greater_than_or_equal>
{
    using stronk::stronk;
};

TEST(can_less_than_greater_than_or_equal, greater_than_or_equal_behaves_similar_to_integers)
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            EXPECT_EQ(i >= j,
                      a_less_than_greater_than_or_equals_type {i} >= a_less_than_greater_than_or_equals_type {j});
        }
    }
}

TEST(can_less_than_greater_than_or_equal, less_than_or_equal_behaves_similar_to_integers)
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            EXPECT_EQ(i <= j,
                      a_less_than_greater_than_or_equals_type {i} <= a_less_than_greater_than_or_equals_type {j});
        }
    }
}

struct an_addeable_type : stronk<an_addeable_type, int, can_add, can_equate>
{
    using stronk::stronk;
};

TEST(can_add, can_add_numbers_correctly)
{
    auto val_1 = an_addeable_type {5};
    auto val_2 = an_addeable_type {2};
    EXPECT_EQ(val_1 + val_2, an_addeable_type {7});
    val_1 += val_2;

    EXPECT_EQ(val_1 + val_2, an_addeable_type {9});
}

TEST(can_add, adding_behaves_similar_to_integers)
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            EXPECT_EQ(an_addeable_type {i + j}, an_addeable_type {i} + an_addeable_type {j});
        }
    }
}

struct a_subtractable_type : stronk<a_subtractable_type, int, can_subtract, can_equate>
{
    using stronk::stronk;
};

TEST(can_subtract, can_subtract_numbers_correctly)
{
    auto val_1 = a_subtractable_type {5};
    auto val_2 = a_subtractable_type {2};
    EXPECT_EQ(val_1 - val_2, a_subtractable_type {3});
    val_1 -= val_2;

    EXPECT_EQ(val_1 - val_2, a_subtractable_type {1});
}

TEST(can_subtract, subtracting_behaves_similar_to_integers)
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            EXPECT_EQ(a_subtractable_type {i - j}, a_subtractable_type {i} - a_subtractable_type {j});
        }
    }
}

struct a_multiplying_type : stronk<a_multiplying_type, int, can_multiply, can_equate>
{
    using stronk::stronk;
};

TEST(can_multiply, can_multiply_numbers_correctly)
{
    auto val_1 = a_multiplying_type {5};
    auto val_2 = a_multiplying_type {2};
    EXPECT_EQ(val_1 * val_2, a_multiplying_type {10});
    val_1 *= val_2;

    EXPECT_EQ(val_1 * val_2, a_multiplying_type {20});
}

TEST(can_multiply, multiplying_behaves_similar_to_integers)
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            EXPECT_EQ(a_multiplying_type {i * j}, a_multiplying_type {i} * a_multiplying_type {j});
        }
    }
}

struct a_dividing_type : stronk<a_dividing_type, int, can_divide, can_equate>
{
    using stronk::stronk;
};

TEST(can_divide, can_divide_numbers_correctly)
{
    auto divideable1 = a_dividing_type {8};
    auto divideable2 = a_dividing_type {2};
    EXPECT_EQ(divideable1 / divideable2, a_dividing_type {4});
    divideable1 /= divideable2;

    EXPECT_EQ(divideable1 / divideable2, a_dividing_type {2});
}

TEST(can_divide, dividing_behaves_similar_to_integers)
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            if (j == 0) {
                continue;
            }
            EXPECT_EQ(a_dividing_type {i / j}, a_dividing_type {i} / a_dividing_type {j});
        }
    }
}

struct a_fuzzy_equal_float_type : stronk<a_fuzzy_equal_float_type, double, can_equate_with_is_close_abs_tol_only>
{
    using stronk::stronk;
};

TEST(can_equate_with_is_close, is_close_is_applied_so_we_get_fuzzy_equal)
{
    EXPECT_EQ(a_fuzzy_equal_float_type {10}, a_fuzzy_equal_float_type {10});
    EXPECT_EQ(a_fuzzy_equal_float_type {-10}, a_fuzzy_equal_float_type {-10});
    EXPECT_EQ(a_fuzzy_equal_float_type {10 + 1e-9}, a_fuzzy_equal_float_type {10});
    EXPECT_NE(a_fuzzy_equal_float_type {10}, a_fuzzy_equal_float_type {-10});
    EXPECT_NE(a_fuzzy_equal_float_type {10 + 1e-7}, a_fuzzy_equal_float_type {10});
}

struct an_abs_min_max_type : stronk<an_abs_min_max_type, int, can_order, can_equate, can_abs>
{
    using stronk::stronk;
};

TEST(can_abs, abs_is_applied_correctly)
{
    EXPECT_EQ(twig::abs(an_abs_min_max_type {10}), an_abs_min_max_type {10});
    EXPECT_EQ(twig::abs(an_abs_min_max_type {-10}), an_abs_min_max_type {10});

    EXPECT_EQ(an_abs_min_max_type {10}.abs(), an_abs_min_max_type {10});
    EXPECT_EQ(an_abs_min_max_type {-10}.abs(), an_abs_min_max_type {10});
}

TEST(can_abs, abs_behaves_similar_to_integer_abs)
{
    for (auto i = -16; i < 16; i++) {
        EXPECT_EQ(an_abs_min_max_type {std::abs(i)}, twig::abs(an_abs_min_max_type {i}));
    }
}

struct an_isnan_type : stronk<an_isnan_type, float, can_isnan>
{
    using stronk::stronk;
};

TEST(can_isnan, isnan_works)
{
    EXPECT_FALSE(twig::isnan(an_isnan_type {0.F}));
    EXPECT_FALSE(twig::isnan(an_isnan_type {1.F}));
    EXPECT_FALSE(twig::isnan(an_isnan_type {-1.1F}));
    EXPECT_TRUE(twig::isnan(an_isnan_type {std::numeric_limits<float>::quiet_NaN()}));
    EXPECT_TRUE(twig::isnan(an_isnan_type {-std::numeric_limits<float>::quiet_NaN()}));
    EXPECT_TRUE(twig::isnan(an_isnan_type {std::numeric_limits<float>::signaling_NaN()}));
    EXPECT_TRUE(twig::isnan(an_isnan_type {-std::numeric_limits<float>::signaling_NaN()}));
}

TEST(can_isnan, quiet_NaN_isnan)
{
    EXPECT_TRUE(twig::isnan(an_isnan_type::quiet_NaN()));
}

TEST(can_isnan, signaling_NaN_isnan)
{
    EXPECT_TRUE(twig::isnan(an_isnan_type::signaling_NaN()));
}

struct a_negatable_type : stronk<a_negatable_type, int32_t, can_negate, can_equate>
{
    using stronk::stronk;
};

TEST(can_negate, when_negating_it_works_like_integers)
{
    for (auto i = -10; i < 10; i++) {
        EXPECT_EQ(-a_negatable_type {i}, a_negatable_type {-i});
    }
}

struct a_can_clamp_type : stronk<a_can_clamp_type, int, can_equate, can_order>
{
    using stronk::stronk;
};

TEST(can_clamp, clamping_behaves_similar_to_integers)
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            for (auto k = -16; k < 16; k++) {
                EXPECT_EQ(a_can_clamp_type {std::clamp(i, j, k)},
                          std::clamp(a_can_clamp_type {i}, a_can_clamp_type {j}, a_can_clamp_type {k}));
            }
        }
    }
}

struct a_flag_type : stronk_flag<a_flag_type>
{
    using stronk_flag::stronk_flag;
};

static_assert(a_flag_type::on().is_on());
static_assert(!a_flag_type::on().is_off());
static_assert(!a_flag_type::off().is_on());
static_assert(a_flag_type::off().is_off());

TEST(flag, flag_values_corrosponds_to_bool_values)
{
    EXPECT_FALSE(a_flag_type {false}.is_on());
    EXPECT_TRUE(a_flag_type {false}.is_off());
    EXPECT_TRUE(a_flag_type {true}.is_on());
    EXPECT_FALSE(a_flag_type {true}.is_off());

    EXPECT_FALSE(a_flag_type::off().is_on());
    EXPECT_TRUE(a_flag_type::off().is_off());

    EXPECT_TRUE(a_flag_type::on().is_on());
    EXPECT_FALSE(a_flag_type::on().is_off());
}

struct a_size_string_type : stronk<a_size_string_type, std::string, can_size>
{
    using stronk::stronk;
};

struct a_size_vector_type : stronk<a_size_vector_type, std::vector<int>, can_size>
{
    using stronk::stronk;
};

TEST(can_size, can_size_works_for_strings)
{
    EXPECT_TRUE(a_size_string_type("").empty());
    EXPECT_FALSE(a_size_string_type("a").empty());
    for (size_t i = 0; i < 16; i++) {
        EXPECT_EQ(a_size_string_type(std::to_string(i)).size(), std::to_string(i).size());
        EXPECT_EQ(a_size_string_type(std::to_string(i)).empty(), std::to_string(i).empty());
    }
}

TEST(can_size, can_size_works_for_vectors)
{
    for (size_t i = 0; i < 16; i++) {
        EXPECT_EQ(a_size_vector_type(std::vector<int>(i)).size(), std::vector<int>(i).size());
        EXPECT_EQ(a_size_vector_type(std::vector<int>(i)).empty(), std::vector<int>(i).empty());
    }
}

struct a_can_iterate_vector_type : stronk<a_can_iterate_vector_type, std::vector<int>, can_iterate>
{
    using stronk::stronk;
};

TEST(can_const_iterate, can_const_iterate_works_for_vectors)
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
            EXPECT_EQ(val, curr);
            curr++;
        }
    }
}

TEST(can_iterate, can_iterate_works_for_vectors)
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
            EXPECT_EQ(val, curr + 1);
            curr++;
        }
    }
}

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

struct type_which_is_an_underlying_type : stronk<type_which_is_an_underlying_type, int>
{
    using stronk::stronk;
};

struct wrapping_is_a_type : stronk<wrapping_is_a_type, type_which_is_an_underlying_type>
{
    using stronk::stronk;
};

TEST(is_a, is_a_works_at_multiple_levels)
{
    auto first_level = type_which_is_an_underlying_type {42};
    auto func = [](const int& x) { EXPECT_EQ(x, 42); };
    func(first_level.unwrap_as<type_which_is_an_underlying_type, int>());
    auto second_level = wrapping_is_a_type {first_level};
    func(second_level.unwrap_as<wrapping_is_a_type, int>());
    func(second_level.unwrap_as<wrapping_is_a_type, type_which_is_an_underlying_type>()
             .unwrap_as<type_which_is_an_underlying_type, int>());
};

struct a_can_multiply_with_int_and_double_type
    : stronk<a_can_multiply_with_int_and_double_type,
             double,
             can_multiply_with<int>::skill,
             can_multiply_with<double>::skill,
             can_equate_underlying_type_specific>
{
    using stronk::stronk;
};

TEST(can_multiply_with, can_multiply_with_integers_and_other_types)
{
    auto val = a_can_multiply_with_int_and_double_type {2};
    val *= 10;
    val = 10 * val;
    val = val * 10;
    EXPECT_EQ(val, a_can_multiply_with_int_and_double_type {2000});
    val *= 0.1;
    val = 0.1 * val;
    val = val * 0.1;
    EXPECT_EQ(val, a_can_multiply_with_int_and_double_type {2});
}

struct a_can_divide_with_int_and_double_type
    : stronk<a_can_divide_with_int_and_double_type,
             double,
             can_divide_with<int>::skill,
             can_divide_with<double>::skill,
             can_equate_underlying_type_specific>
{
    using stronk::stronk;
};

TEST(can_divide_with, can_divide_with_integers_and_other_types)
{
    auto val = a_can_divide_with_int_and_double_type {200};
    val /= 10;
    val = val / 10;
    EXPECT_EQ(val, a_can_divide_with_int_and_double_type {2}) << val.unwrap<a_can_divide_with_int_and_double_type>();
    val /= 0.1;
    val = val / 0.1;
    EXPECT_EQ(val, a_can_divide_with_int_and_double_type {200});
}

}  // namespace twig
