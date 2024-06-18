#pragma once
#include <cmath>

#include <stronk/utilities/constexpr_helpers.h>

namespace twig::stronk_details
{

template<std::floating_point T>
inline constexpr auto default_abs_tol = []() -> T
{
    if constexpr (std::is_same_v<T, float>) {
        return T {1e-5F};
    } else if constexpr (std::is_same_v<T, double>) {
        return T {1e-8};
    } else {
        static_assert(not_implemented_type<T>());
    }
};

template<std::floating_point T>
inline constexpr auto default_rel_tol = []() -> T
{
    if constexpr (std::is_same_v<T, float>) {
        return T {1e-3F};
    } else if constexpr (std::is_same_v<T, double>) {
        return T {1e-5};
    } else {
        static_assert(not_implemented_type<T>());
    }
};

/**
 * @brief create a comparator lambda for floating points with given absolute and relative tolerances
 *
 */
template<typename T>
constexpr auto is_close(T abs_tol, T rel_tol, bool nan_equals = false)
{
    return [abs_tol, rel_tol, nan_equals](const T& a, const T& b) -> bool
    {
        // Taken from https://numpy.org/devdocs/reference/generated/numpy.allclose.html
        auto val_equals = std::abs(a - b) <= (abs_tol + rel_tol * std::abs(b));
        auto both_nan = nan_equals && std::isnan(a) && std::isnan(b);
        return both_nan || val_equals;
    };
};

/**
 * @brief compare two floating point values with the given abs_tol or rel_tol;
 *
 */
template<typename T>
constexpr auto is_close(const T& a, const T& b, T abs_tol, T rel_tol, bool nan_equals = false) -> bool
{
    return is_close(abs_tol, rel_tol, nan_equals)(a, b);
};

}  // namespace twig::stronk_details
