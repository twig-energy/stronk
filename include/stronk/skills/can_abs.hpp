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
        return StronkT {std::abs(static_cast<const StronkT&>(*this).template unwrap<StronkT>())};
    }
};

template<typename T>
concept can_abs_like = stronk_like<T> && requires(T v) {
    { v.abs() } -> std::same_as<T>;
};

[[nodiscard]]
constexpr auto abs(can_abs_like auto elem) noexcept
{
    return elem.abs();
}

}  // namespace twig
