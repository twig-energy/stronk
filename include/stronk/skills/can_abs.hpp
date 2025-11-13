#pragma once
#include <cstdlib>

#include "stronk/cmath.hpp"

namespace twig
{

template<typename StronkT>
struct can_abs
{
    [[nodiscard]]
    constexpr auto abs() const noexcept -> StronkT
    {
        using twig::abs;
        return StronkT {abs(static_cast<const StronkT&>(*this))};
    }
};

}  // namespace twig
