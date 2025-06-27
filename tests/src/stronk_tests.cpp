#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

#include "stronk/stronk.hpp"

#include <doctest/doctest.h>

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

TEST_SUITE("unwrap")
{
    TEST_CASE("when unwrapping it works with its own argument")
    {
        auto a = an_int_test_type {42};
        CHECK_EQ(a.unwrap<an_int_test_type>(), 42);

        auto b = a_float_test_type {42.1F};
        CHECK_EQ(b.unwrap<a_float_test_type>(), 42.1F);
    }
}

TEST_SUITE("transform")
{
    TEST_CASE("transform gets called on the underlying type and returns a new copy")
    {
        auto val = an_int_test_type {2};
        val = val.transform([](auto v) { return v * 10; }).transform([](auto v) { return v + 22; });
        CHECK_EQ(val.unwrap<an_int_test_type>(), 42);
    }
}

template<an_int_test_type Val>
struct type_which_requires_stronk_none_type_template_param
{
    constexpr static an_int_test_type value = Val;
};

TEST_SUITE("none_type_template_parameter")
{
    TEST_CASE("stronk types can be used as none type template parameters")
    {
        auto val = type_which_requires_stronk_none_type_template_param<an_int_test_type {25}> {};
        CHECK_EQ(val.value.unwrap<an_int_test_type>(), 25);
    }
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

TEST_SUITE("move")
{
    TEST_CASE("stronk allows to move and move out with unwrap")
    {
        {  // wrapping type
            auto marker = false;
            auto val = a_move_indicating_type(mark_if_moved_type {&marker});
            marker = false;

            [[maybe_unused]]
            auto copy = val;
            CHECK_FALSE(marker);
            [[maybe_unused]]
            auto moved = std::move(val);  // has side effect
            CHECK(marker);
        }

        {  // underlying type
            auto marker = false;
            auto val = a_move_indicating_type {mark_if_moved_type {&marker}};
            marker = false;

            [[maybe_unused]]
            auto copy = val.unwrap<a_move_indicating_type>();
            CHECK_FALSE(marker);
            auto moved = std::move(val.unwrap<a_move_indicating_type>());
            CHECK(marker);
        }
    }
}

struct a_str_type : stronk<a_str_type, std::string>
{
    using stronk::stronk;
};

TEST_SUITE("constructor")
{
    TEST_CASE("can construct from both rvalue lvalues and forwarded")
    {
        auto str = std::string {"yoyo"};
        auto stronked_copy = a_str_type {str};
        CHECK_EQ(stronked_copy.unwrap<a_str_type>(), str);

        auto stronked_moved = a_str_type {std::string {"lolo"}};
        CHECK_EQ(stronked_moved.unwrap<a_str_type>(), "lolo");

        auto stronked_forward = a_str_type {"soso"};
        CHECK_EQ(stronked_forward.unwrap<a_str_type>(), "soso");
    }
}

struct implicit_wrapper
{
    double d;

    operator double() const  // NOLINT(google-explicit-constructor, hicpp-explicit-conversions)
                             // because that's the test
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

    a_type_with_multiple_members(int a_, double b_)
        : a(a_)
        , b(b_)
    {
    }
};

struct wrapping_multiple_member_type : stronk<wrapping_multiple_member_type, a_type_with_multiple_members>
{
    using stronk::stronk;
};

TEST_SUITE("can_forward_multiple_args")
{
    TEST_CASE("correctly passes multiple args in constructor")
    {
        auto val = wrapping_multiple_member_type {1, 2.1};
        CHECK_EQ(val.unwrap<wrapping_multiple_member_type>().a, 1);
        CHECK_EQ(val.unwrap<wrapping_multiple_member_type>().b, 2.1);
    }
}

struct an_orderable_type : stronk<an_orderable_type, int, can_order>
{
    using stronk::stronk;
};

TEST_SUITE("can_order")
{
    TEST_CASE("when given an array of orderable it can be sorted")
    {
        auto arr = std::array<an_orderable_type, 5> {an_orderable_type {5},
                                                     an_orderable_type {4},
                                                     an_orderable_type {3},
                                                     an_orderable_type {1},
                                                     an_orderable_type {2}};

        std::sort(arr.begin(), arr.end());
        CHECK(std::is_sorted(arr.begin(), arr.end()));
    }

    TEST_CASE("order behaves similar to integers")
    {
        for (auto i = -16; i < 16; i++) {
            for (auto j = -16; j < 16; j++) {
                CHECK_EQ(i <=> j, an_orderable_type {i} <=> an_orderable_type {j});
            }
        }
    }

    struct an_orderable_and_equatable_type : stronk<an_orderable_and_equatable_type, int, can_order, can_equate>
    {
        using stronk::stronk;
    };

    TEST_CASE("enables min max behavior correctly")
    {
        CHECK_EQ(std::min(an_orderable_and_equatable_type {10}, an_orderable_and_equatable_type {5}),
                 an_orderable_and_equatable_type {5});
        CHECK_EQ(std::min(an_orderable_and_equatable_type {5}, an_orderable_and_equatable_type {10}),
                 an_orderable_and_equatable_type {5});
        CHECK_EQ(std::max(an_orderable_and_equatable_type {10}, an_orderable_and_equatable_type {5}),
                 an_orderable_and_equatable_type {10});
        CHECK_EQ(std::max(an_orderable_and_equatable_type {5}, an_orderable_and_equatable_type {10}),
                 an_orderable_and_equatable_type {10});
    }

    TEST_CASE("min max behaves similar to integers")
    {
        for (auto i = -16; i < 16; i++) {
            for (auto j = -16; j < 16; j++) {
                CHECK_EQ(an_orderable_and_equatable_type {std::min(i, j)},
                         std::min(an_orderable_and_equatable_type {i}, an_orderable_and_equatable_type {j}));
                CHECK_EQ(an_orderable_and_equatable_type {std::max(i, j)},
                         std::max(an_orderable_and_equatable_type {i}, an_orderable_and_equatable_type {j}));
            }
        }
    }
}

struct an_equatable_type : stronk<an_equatable_type, int, can_equate>
{
    using stronk::stronk;
};

TEST_SUITE("can_equate")
{
    TEST_CASE("can equate numbers correctly")
    {
        CHECK_EQ(an_equatable_type {-1}, an_equatable_type {-1});
        CHECK_NE(an_equatable_type {-1}, an_equatable_type {0});
        CHECK_NE(an_equatable_type {-1}, an_equatable_type {1});

        CHECK_NE(an_equatable_type {0}, an_equatable_type {-1});
        CHECK_EQ(an_equatable_type {0}, an_equatable_type {0});
        CHECK_NE(an_equatable_type {0}, an_equatable_type {1});

        CHECK_NE(an_equatable_type {1}, an_equatable_type {-1});
        CHECK_NE(an_equatable_type {1}, an_equatable_type {0});
        CHECK_EQ(an_equatable_type {1}, an_equatable_type {1});

        CHECK_EQ(an_equatable_type {1247865}, an_equatable_type {1247865});
        CHECK_NE(an_equatable_type {1247865}, an_equatable_type {918256});
    }

    TEST_CASE("equate behaves similar to integers")
    {
        for (auto i = -16; i < 16; i++) {
            for (auto j = -16; j < 16; j++) {
                CHECK_EQ(i == j, an_equatable_type {i} == an_equatable_type {j});
            }
        }
    }
}

struct a_less_than_greater_than_type : stronk<a_less_than_greater_than_type, int, can_less_than_greater_than>
{
    using stronk::stronk;
};

TEST_SUITE("a_less_than_greater_than_type")
{
    TEST_CASE("can less than correctly")
    {
        CHECK_FALSE(a_less_than_greater_than_type {-1} < a_less_than_greater_than_type {-1});
        CHECK_LT(a_less_than_greater_than_type {-1}, a_less_than_greater_than_type {0});
        CHECK_LT(a_less_than_greater_than_type {-1}, a_less_than_greater_than_type {1});

        CHECK_FALSE(a_less_than_greater_than_type {0} < a_less_than_greater_than_type {-1});
        CHECK_FALSE(a_less_than_greater_than_type {0} < a_less_than_greater_than_type {0});
        CHECK_LT(a_less_than_greater_than_type {0}, a_less_than_greater_than_type {1});

        CHECK_FALSE(a_less_than_greater_than_type {1} < a_less_than_greater_than_type {-1});
        CHECK_FALSE(a_less_than_greater_than_type {1} < a_less_than_greater_than_type {0});
        CHECK_FALSE(a_less_than_greater_than_type {1} < a_less_than_greater_than_type {1});

        CHECK_FALSE(a_less_than_greater_than_type {1247865} < a_less_than_greater_than_type {1247865});
        CHECK_FALSE(a_less_than_greater_than_type {1247865} < a_less_than_greater_than_type {918256});
    }
    TEST_CASE("less than behaves similar to integers")
    {
        for (auto i = -16; i < 16; i++) {
            for (auto j = -16; j < 16; j++) {
                CHECK_EQ(i < j, a_less_than_greater_than_type {i} < a_less_than_greater_than_type {j});
            }
        }
    }

    TEST_CASE("can_greater_than correctly")
    {
        CHECK_FALSE(a_less_than_greater_than_type {-1} > a_less_than_greater_than_type {-1});
        CHECK_FALSE(a_less_than_greater_than_type {-1} > a_less_than_greater_than_type {0});
        CHECK_FALSE(a_less_than_greater_than_type {-1} > a_less_than_greater_than_type {1});

        CHECK_GT(a_less_than_greater_than_type {0}, a_less_than_greater_than_type {-1});
        CHECK_FALSE(a_less_than_greater_than_type {0} > a_less_than_greater_than_type {0});
        CHECK_FALSE(a_less_than_greater_than_type {0} > a_less_than_greater_than_type {1});

        CHECK_GT(a_less_than_greater_than_type {1}, a_less_than_greater_than_type {-1});
        CHECK_GT(a_less_than_greater_than_type {1}, a_less_than_greater_than_type {0});
        CHECK_FALSE(a_less_than_greater_than_type {1} > a_less_than_greater_than_type {1});

        CHECK_FALSE(a_less_than_greater_than_type {1247865} > a_less_than_greater_than_type {1247865});
        CHECK_GT(a_less_than_greater_than_type {1247865}, a_less_than_greater_than_type {918256});
    }

    TEST_CASE("greater than behaves similar to integers")
    {
        for (auto i = -16; i < 16; i++) {
            for (auto j = -16; j < 16; j++) {
                CHECK_EQ(i > j, a_less_than_greater_than_type {i} > a_less_than_greater_than_type {j});
            }
        }
    }
}

struct a_less_than_greater_than_or_equals_type
    : stronk<a_less_than_greater_than_or_equals_type, int, can_less_than_greater_than_or_equal>
{
    using stronk::stronk;
};

TEST_SUITE("can_less_than_greater_than_or_equal")
{
    TEST_CASE("greater than or equal behaves similar to integers")
    {
        for (auto i = -16; i < 16; i++) {
            for (auto j = -16; j < 16; j++) {
                CHECK_EQ(i >= j,
                         a_less_than_greater_than_or_equals_type {i} >= a_less_than_greater_than_or_equals_type {j});
            }
        }
    }

    TEST_CASE("less than or equal behaves similar to integers")
    {
        for (auto i = -16; i < 16; i++) {
            for (auto j = -16; j < 16; j++) {
                CHECK_EQ(i <= j,
                         a_less_than_greater_than_or_equals_type {i} <= a_less_than_greater_than_or_equals_type {j});
            }
        }
    }
}

struct an_addeable_type : stronk<an_addeable_type, int, can_add, can_equate>
{
    using stronk::stronk;
};

TEST_SUITE("can_add")
{
    TEST_CASE("can_add works correctly")
    {
        auto val_1 = an_addeable_type {5};
        auto val_2 = an_addeable_type {2};
        CHECK_EQ(val_1 + val_2, an_addeable_type {7});
        val_1 += val_2;

        CHECK_EQ(val_1 + val_2, an_addeable_type {9});
    }

    TEST_CASE("adding behaves similar to integers")
    {
        for (auto i = -16; i < 16; i++) {
            for (auto j = -16; j < 16; j++) {
                CHECK_EQ(an_addeable_type {i + j}, an_addeable_type {i} + an_addeable_type {j});
            }
        }
    }
}

struct a_subtractable_type : stronk<a_subtractable_type, int, can_subtract, can_equate>
{
    using stronk::stronk;
};

TEST_SUITE("can_subtract")
{
    TEST_CASE("can_subtract works correctly")
    {
        auto val_1 = a_subtractable_type {5};
        auto val_2 = a_subtractable_type {2};
        CHECK_EQ(val_1 - val_2, a_subtractable_type {3});
        val_1 -= val_2;

        CHECK_EQ(val_1 - val_2, a_subtractable_type {1});
    }

    TEST_CASE("subtracting behaves similar to integers")
    {
        for (auto i = -16; i < 16; i++) {
            for (auto j = -16; j < 16; j++) {
                CHECK_EQ(a_subtractable_type {i - j}, a_subtractable_type {i} - a_subtractable_type {j});
            }
        }
    }
}

struct a_fuzzy_equal_float_type : stronk<a_fuzzy_equal_float_type, double, can_equate_with_is_close_abs_tol_only>
{
    using stronk::stronk;
};

TEST_SUITE("can_equate_with_is_close")
{
    TEST_CASE("is_close is applied so we get fuzzy equal")
    {
        CHECK_EQ(a_fuzzy_equal_float_type {10}, a_fuzzy_equal_float_type {10});
        CHECK_EQ(a_fuzzy_equal_float_type {-10}, a_fuzzy_equal_float_type {-10});
        CHECK_EQ(a_fuzzy_equal_float_type {10 + 1e-9}, a_fuzzy_equal_float_type {10});
        CHECK_NE(a_fuzzy_equal_float_type {10}, a_fuzzy_equal_float_type {-10});
        CHECK_NE(a_fuzzy_equal_float_type {10 + 1e-7}, a_fuzzy_equal_float_type {10});
    }
}
struct a_negatable_type : stronk<a_negatable_type, int32_t, can_negate, can_equate>
{
    using stronk::stronk;
};

TEST_SUITE("can_negate")
{
    TEST_CASE("when negating it works like integers")
    {
        for (auto i = -10; i < 10; i++) {
            CHECK_EQ(-a_negatable_type {i}, a_negatable_type {-i});
        }
    }
}

struct a_can_clamp_type : stronk<a_can_clamp_type, int, can_equate, can_order>
{
    using stronk::stronk;
};

TEST_SUITE("can_clamp")
{
    TEST_CASE("clamping behaves similar to integers")
    {
        for (auto i = -16; i < 16; i++) {
            for (auto j = -16; j < 16; j++) {
                for (auto k = j; k < 16; k++) {
                    CHECK_EQ(a_can_clamp_type {std::clamp(i, j, k)},
                             std::clamp(a_can_clamp_type {i}, a_can_clamp_type {j}, a_can_clamp_type {k}));
                }
            }
        }
    }
}

struct a_size_string_type : stronk<a_size_string_type, std::string, can_size>
{
    using stronk::stronk;
};

struct a_size_vector_type : stronk<a_size_vector_type, std::vector<int>, can_size>
{
    using stronk::stronk;
};

TEST_SUITE("can_size")
{
    TEST_CASE("can_size works for strings")
    {
        CHECK(a_size_string_type("").empty());
        CHECK_FALSE(a_size_string_type("a").empty());
        for (size_t i = 0; i < 16; i++) {
            CHECK_EQ(a_size_string_type(std::to_string(i)).size(), std::to_string(i).size());
            CHECK_EQ(a_size_string_type(std::to_string(i)).empty(), std::to_string(i).empty());
        }
    }

    TEST_CASE("can_size works for vectors")
    {
        for (size_t i = 0; i < 16; i++) {
            CHECK_EQ(a_size_vector_type(std::vector<int>(i)).size(), std::vector<int>(i).size());
            CHECK_EQ(a_size_vector_type(std::vector<int>(i)).empty(), std::vector<int>(i).empty());
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

TEST_SUITE("is_a")
{
    TEST_CASE("is_a works at multiple levels")
    {
        auto first_level = type_which_is_an_underlying_type {42};
        auto func = [](const int& x) { CHECK_EQ(x, 42); };
        func(first_level.unwrap_as<type_which_is_an_underlying_type, int>());
        auto second_level = wrapping_is_a_type {first_level};
        func(second_level.unwrap_as<wrapping_is_a_type, int>());
        func(second_level.unwrap_as<wrapping_is_a_type, type_which_is_an_underlying_type>()
                 .unwrap_as<type_which_is_an_underlying_type, int>());
    };
}

}  // namespace twig
