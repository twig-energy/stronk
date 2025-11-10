#pragma once
#include <concepts>

#include "stronk/stronk.hpp"

namespace twig
{

template<typename StronkT>
struct can_abs
{
    [[nodiscard]]
    constexpr auto abs() const noexcept -> StronkT
    {
        using std::abs;  // ADL
        return StronkT {abs(static_cast<const StronkT&>(*this).template unwrap<StronkT>())};
    }
};

template<typename T>
concept can_abs_like = stronk_like<T> && requires(T v) {
    { v.abs() } -> std::same_as<T>;
};

template<can_abs_like StronkT>
[[nodiscard]]
constexpr auto abs(const StronkT& elem) noexcept
{
    return elem.abs();
}

}  // namespace twig
