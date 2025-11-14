#pragma once
#include <cstdlib>

#include "stronk/cmath.hpp"

namespace twig
{

template<typename StronkT>
struct [[deprecated("twig::abs now just works for all stronk types which support abs")]] can_abs
{
    [[nodiscard]]
    constexpr auto abs() const noexcept -> StronkT
    {
        using twig::abs;
        return StronkT {abs(static_cast<const StronkT&>(*this))};
    }
};

}  // namespace twig
