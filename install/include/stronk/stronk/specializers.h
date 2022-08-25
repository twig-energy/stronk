#pragma once
#include <stronk/unit.h>

// Macro for getting the full type of multiplying two units
// Only works when T1 and T2 has one unit on the multiplication part
// NOLINTNEXTLINE
#define STRONK_MULTIPLY_TYPE(T1, T2)                                         \
    NewUnitType<typename twig::underlying_type_of_multiplying<T1, T2>::type, \
                typename twig::unit_lists_of_multiplying<T1, T2>::unit_description_t>

// Macro for getting the full type of dividing two units
// Only works when T1 and T2 has one unit on the multiplication part
// NOLINTNEXTLINE
#define STRONK_DIVIDE_TYPE(T1, T2)                                        \
    NewUnitType<typename twig::underlying_type_of_dividing<T1, T2>::type, \
                typename twig::unit_lists_of_dividing<T1, T2>::unit_description_t>

// Specialize the struct created when multiplying two types. Allows you to add
// other skills than the default prefab. Only works when T1 and T2 has one unit
// on the multiplication part `T1` is the first unit, `T2` the second and `...`
// is skills. Resulting type specializes=`decltype(T1 {} * T2 {})`
// NOLINTNEXTLINE
#define STRONK_SPECIALIZE_MULTIPLY(T1, T2, ...)                                                          \
    template<>                                                                                           \
    struct STRONK_MULTIPLY_TYPE(T1, T2)                                                                  \
        : stronk<STRONK_MULTIPLY_TYPE(T1, T2),                                                           \
                 typename twig::underlying_type_of_multiplying<T1, T2>::type __VA_OPT__(, ) __VA_ARGS__> \
        , twig::unit_lists_of_multiplying<T1, T2>::unit_description_t                                    \
    {                                                                                                    \
        using stronk::stronk;                                                                            \
    };                                                                                                   \
    static_assert(std::is_same_v<decltype(T1 {} * T2 {}), STRONK_MULTIPLY_TYPE(T1, T2)>)

// Specialize the struct created when dividing two types. Allows you to add
// other skills than the default prefab Only works when T1 and T2 has one unit
// on the multiplication part `T1` is the first unit, `T2` the second and `...`
// is skills. Resulting type specializes=`decltype(T1 {} / T2 {})`
// NOLINTNEXTLINE
#define STRONK_SPECIALIZE_DIVIDE(T1, T2, ...)                                                         \
    template<>                                                                                        \
    struct STRONK_DIVIDE_TYPE(T1, T2)                                                                 \
        : stronk<STRONK_DIVIDE_TYPE(T1, T2),                                                          \
                 typename twig::underlying_type_of_dividing<T1, T2>::type __VA_OPT__(, ) __VA_ARGS__> \
        , twig::unit_lists_of_dividing<T1, T2>::unit_description_t                                    \
    {                                                                                                 \
        using stronk::stronk;                                                                         \
    };                                                                                                \
    static_assert(std::is_same_v<decltype(T1 {} / T2 {}), STRONK_DIVIDE_TYPE(T1, T2)>)
