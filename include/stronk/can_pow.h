#pragma once
#include <cmath>
#include <type_traits>

namespace twig
{

template<typename StronkT>
struct can_pow
{
    [[nodiscard]] constexpr auto pow(const auto& exponent) const noexcept -> StronkT
    {
        static_assert(std::is_floating_point_v<typename StronkT::underlying_type>);

        return StronkT {std::pow(static_cast<const StronkT&>(*this).template unwrap<StronkT>(), exponent)};
    }
};

}  // namespace twig
