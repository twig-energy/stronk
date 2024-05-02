#pragma once
#include <cmath>
#include <type_traits>

#include <stronk/stronk.h>

namespace twig
{

template<typename StronkT>
struct can_sqrt
{
    [[nodiscard]] constexpr auto sqrt() const noexcept -> StronkT
    {
        static_assert(std::is_floating_point_v<typename StronkT::underlying_type>);
        return StronkT {std::sqrt(static_cast<const StronkT&>(*this).template unwrap<StronkT>())};
    }
};

template<typename T>
concept can_sqrt_like = stronk_like<T> && requires(T v) {
    {
        v.sqrt()
    } -> std::same_as<T>;
};

[[nodiscard]] constexpr static auto sqrt(const can_sqrt_like auto& val) noexcept
{
    return val.sqrt();
};

}  // namespace twig
