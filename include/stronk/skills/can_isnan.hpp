#pragma once
#include <concepts>
#include <limits>
#include <type_traits>

#include "stronk/stronk.hpp"

namespace twig
{

template<typename StronkT>
struct can_isnan
{
    [[nodiscard]]
    constexpr auto isnan() const noexcept -> bool
    {
        static_assert(std::is_floating_point_v<typename StronkT::underlying_type>);
        return std::isnan(static_cast<const StronkT&>(*this).template unwrap<StronkT>());
    }

    [[nodiscard]]
    constexpr static auto quiet_NaN() -> StronkT  // NOLINT(readability-identifier-naming)
    {
        static_assert(std::is_floating_point_v<typename StronkT::underlying_type>);
        return StronkT {std::numeric_limits<typename StronkT::underlying_type>::quiet_NaN()};
    }

    [[nodiscard]]
    constexpr static auto signaling_NaN() -> StronkT  // NOLINT(readability-identifier-naming)
    {
        static_assert(std::is_floating_point_v<typename StronkT::underlying_type>);
        return StronkT {std::numeric_limits<typename StronkT::underlying_type>::signaling_NaN()};
    }
};

template<typename T>
concept can_isnan_like = stronk_like<T> && requires(T v) {
    {
        v.isnan()
    } -> std::same_as<bool>;
};

[[nodiscard]]
constexpr auto isnan(can_isnan_like auto elem) noexcept -> bool
{
    return elem.isnan();
}
}  // namespace twig
