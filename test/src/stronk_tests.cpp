#include <algorithm>
#include <array>

#include <gtest/gtest.h>
#include <stronk/extensions/fmt.h>
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

TEST(unwrap, when_unwrapping_it_works_with_its_own_argument)  // NOLINT
{
    auto a = an_int_test_type {42};
    EXPECT_EQ(a.unwrap<an_int_test_type>(), 42);

    auto b = a_float_test_type {42.1F};
    EXPECT_EQ(b.unwrap<a_float_test_type>(), 42.1F);
}

struct an_orderable_type : stronk<an_orderable_type, int, can_order>
{
    using stronk::stronk;
};

TEST(can_order, when_given_an_array_of_orderable_it_can_be_sorted)  // NOLINT
{
    auto arr = std::array<an_orderable_type, 5> {an_orderable_type {5},
                                                 an_orderable_type {4},
                                                 an_orderable_type {3},
                                                 an_orderable_type {1},
                                                 an_orderable_type {2}};

    std::sort(arr.begin(), arr.end());
    EXPECT_TRUE(std::is_sorted(arr.begin(), arr.end()));
}

TEST(can_order, order_behaves_similar_to_integers)  // NOLINT
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

TEST(can_order, enables_min_max_behavior_correctly)  // NOLINT
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

TEST(can_order, min_max_behaves_similar_to_integers)  // NOLINT
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

TEST(can_equate, can_equate_numbers_correctly)  // NOLINT
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

TEST(can_equate, equate_behaves_similar_to_integers)  // NOLINT
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

TEST(a_less_than_greater_than_type, can_less_than_correctly)  // NOLINT
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
TEST(a_less_than_greater_than_type, less_than_behaves_similar_to_integers)  // NOLINT
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            EXPECT_EQ(i < j, a_less_than_greater_than_type {i} < a_less_than_greater_than_type {j});
        }
    }
}

TEST(a_less_than_greater_than_type, can_greater_than_correctly)  // NOLINT
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

TEST(a_less_than_greater_than_type, greater_than_behaves_similar_to_integers)  // NOLINT
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

TEST(can_less_than_greater_than_or_equal, greater_than_or_equal_behaves_similar_to_integers)  // NOLINT
{
    for (auto i = -16; i < 16; i++) {
        for (auto j = -16; j < 16; j++) {
            EXPECT_EQ(i >= j,
                      a_less_than_greater_than_or_equals_type {i} >= a_less_than_greater_than_or_equals_type {j});
        }
    }
}

TEST(can_less_than_greater_than_or_equal, less_than_or_equal_behaves_similar_to_integers)  // NOLINT
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

TEST(can_add, can_add_numbers_correctly)  // NOLINT
{
    auto val_1 = an_addeable_type {5};
    auto val_2 = an_addeable_type {2};
    EXPECT_EQ(val_1 + val_2, an_addeable_type {7});
    val_1 += val_2;

    EXPECT_EQ(val_1 + val_2, an_addeable_type {9});
}

TEST(can_add, adding_behaves_similar_to_integers)  // NOLINT
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

TEST(can_subtract, can_subtract_numbers_correctly)  // NOLINT
{
    auto val_1 = a_subtractable_type {5};
    auto val_2 = a_subtractable_type {2};
    EXPECT_EQ(val_1 - val_2, a_subtractable_type {3});
    val_1 -= val_2;

    EXPECT_EQ(val_1 - val_2, a_subtractable_type {1});
}

TEST(can_subtract, subtracting_behaves_similar_to_integers)  // NOLINT
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

TEST(can_multiply, can_multiply_numbers_correctly)  // NOLINT
{
    auto val_1 = a_multiplying_type {5};
    auto val_2 = a_multiplying_type {2};
    EXPECT_EQ(val_1 * val_2, a_multiplying_type {10});
    val_1 *= val_2;

    EXPECT_EQ(val_1 * val_2, a_multiplying_type {20});
}

TEST(can_multiply, multiplying_behaves_similar_to_integers)  // NOLINT
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

TEST(can_divide, can_divide_numbers_correctly)  // NOLINT
{
    auto divideable1 = a_dividing_type {8};
    auto divideable2 = a_dividing_type {2};
    EXPECT_EQ(divideable1 / divideable2, a_dividing_type {4});
    divideable1 /= divideable2;

    EXPECT_EQ(divideable1 / divideable2, a_dividing_type {2});
}

TEST(can_divide, dividing_behaves_similar_to_integers)  // NOLINT
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

struct a_streamable_type : stronk<a_streamable_type, int, can_stream>
{
    using stronk::stronk;
};

TEST(can_stream, streaming_to_ostream_prints_the_value)  // NOLINT
{
    auto formattable = a_streamable_type {5};
    auto sstream = std::stringstream();
    sstream << formattable;

    EXPECT_EQ(sstream.str(), "5");
}

struct a_fuzzy_equal_float_type : stronk<a_fuzzy_equal_float_type, double, can_equate_with_is_close_abs_tol_only>
{
    using stronk::stronk;
};

TEST(can_equate_with_is_close, is_close_is_applied_so_we_get_fuzzy_equal)  // NOLINT
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

TEST(can_abs, abs_is_applied_correctly)  // NOLINT
{
    EXPECT_EQ(twig::abs(an_abs_min_max_type {10}), an_abs_min_max_type {10});
    EXPECT_EQ(twig::abs(an_abs_min_max_type {-10}), an_abs_min_max_type {10});

    EXPECT_EQ(an_abs_min_max_type {10}.abs(), an_abs_min_max_type {10});
    EXPECT_EQ(an_abs_min_max_type {-10}.abs(), an_abs_min_max_type {10});
}

TEST(can_abs, abs_behaves_similar_to_integer_abs)  // NOLINT
{
    for (auto i = -16; i < 16; i++) {
        EXPECT_EQ(an_abs_min_max_type {std::abs(i)}, twig::abs(an_abs_min_max_type {i}));
    }
}

struct an_isnan_type : stronk<an_isnan_type, float, can_isnan>
{
    using stronk::stronk;
};

TEST(can_isnan, isnan_works)  // NOLINT
{
    EXPECT_FALSE(twig::isnan(an_isnan_type {0.F}));
    EXPECT_FALSE(twig::isnan(an_isnan_type {1.F}));
    EXPECT_FALSE(twig::isnan(an_isnan_type {-1.1F}));
    EXPECT_TRUE(twig::isnan(an_isnan_type {std::numeric_limits<float>::quiet_NaN()}));
    EXPECT_TRUE(twig::isnan(an_isnan_type {-std::numeric_limits<float>::quiet_NaN()}));
    EXPECT_TRUE(twig::isnan(an_isnan_type {std::numeric_limits<float>::signaling_NaN()}));
    EXPECT_TRUE(twig::isnan(an_isnan_type {-std::numeric_limits<float>::signaling_NaN()}));
}

struct a_negatable_type : stronk<a_negatable_type, int32_t, can_negate, can_equate>
{
    using stronk::stronk;
};

TEST(can_negate, when_negating_it_works_like_integers)  // NOLINT
{
    for (auto i = -10; i < 10; i++) {
        EXPECT_EQ(-a_negatable_type {i}, a_negatable_type {-i});
    }
}

struct a_can_clamp_type : stronk<a_can_clamp_type, int, can_equate, can_order>
{
    using stronk::stronk;
};

TEST(can_clamp, clamping_behaves_similar_to_integers)  // NOLINT
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

TEST(flag, flag_values_corrosponds_to_bool_values)  // NOLINT
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
