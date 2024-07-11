#pragma once
#include <stronk/prefabs.h>
#include <stronk/stronk.h>
#include <stronk/unit.h>

// Macro for getting the full type of multiplying two units
// Only works when T1 and T2 has one unit on the multiplication part
// NOLINTNEXTLINE
#define STRONK_MULTIPLY_TYPE(T1, T2)                                              \
    NewStronkUnit<typename twig::underlying_multiply_operation<T1, T2>::res_type, \
                  typename twig::multiplied_unit<T1, T2>::dimensions_t>

// Macro for getting the full type of dividing two units
// Only works when T1 and T2 has one unit on the multiplication part
// NOLINTNEXTLINE
#define STRONK_DIVIDE_TYPE(T1, T2)                                              \
    NewStronkUnit<typename twig::underlying_divide_operation<T1, T2>::res_type, \
                  typename twig::divided_unit<T1, T2>::dimensions_t>

// Specialize the struct created when multiplying two types. Allows you to add
// other skills than the default prefab. Only works when T1 and T2 has one unit
// on the multiplication part `T1` is the first unit, `T2` the second and `...`
// is skills. Resulting type specializes=`decltype(T1 {} * T2 {})`
// NOLINTNEXTLINE
#define STRONK_SPECIALIZE_MULTIPLY(T1, T2, ...)                                            \
    template<>                                                                             \
    struct STRONK_MULTIPLY_TYPE(T1, T2)                                                    \
        : stronk<STRONK_MULTIPLY_TYPE(T1, T2),                                             \
                 typename twig::underlying_multiply_operation<T1, T2>::res_type,           \
                 twig::multiplied_unit<T1, T2>::template skill __VA_OPT__(, ) __VA_ARGS__> \
                                                                                           \
    {                                                                                      \
        using stronk::stronk;                                                              \
    };                                                                                     \
    static_assert(std::is_same_v<decltype(T1 {} * T2 {}), STRONK_MULTIPLY_TYPE(T1, T2)>)

// Specialize the struct created when dividing two types. Allows you to add
// other skills than the default prefab Only works when T1 and T2 has one unit
// on the multiplication part `T1` is the first unit, `T2` the second and `...`
// is skills. Resulting type specializes=`decltype(T1 {} / T2 {})`
// NOLINTNEXTLINE
#define STRONK_SPECIALIZE_DIVIDE(T1, T2, ...)                                           \
    template<>                                                                          \
    struct STRONK_DIVIDE_TYPE(T1, T2)                                                   \
        : stronk<STRONK_DIVIDE_TYPE(T1, T2),                                            \
                 typename twig::underlying_divide_operation<T1, T2>::res_type,          \
                 twig::divided_unit<T1, T2>::template skill __VA_OPT__(, ) __VA_ARGS__> \
                                                                                        \
    {                                                                                   \
        using stronk::stronk;                                                           \
    };                                                                                  \
    static_assert(std::is_same_v<decltype(T1 {} / T2 {}), STRONK_DIVIDE_TYPE(T1, T2)>)
