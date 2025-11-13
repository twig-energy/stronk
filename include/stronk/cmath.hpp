#pragma once

#include <cmath>

#include "stronk/stronk.hpp"
#include "stronk/unit.hpp"

namespace twig
{

template<stronk_like StronkT>
    requires(!unit_value_like<StronkT>)  // units should divide by T(1) first
auto log(StronkT value) -> decltype(log(value.template unwrap<StronkT>()))
{
    using std::log;  // ADL
    return StronkT {log(value.template unwrap<StronkT>())};
}

template<stronk_like StronkT>
    requires(!unit_value_like<StronkT>)  // units should divide by T(1) first
auto log2(StronkT value) -> decltype(log(value.template unwrap<StronkT>()))
{
    using std::log2;  // ADL
    return StronkT {log2(value.template unwrap<StronkT>())};
}

template<stronk_like StronkT>
    requires(!unit_value_like<StronkT>)  // units should divide by T(1) first
auto log10(StronkT value) -> decltype(log10(value.template unwrap<StronkT>()))
{
    using std::log10;  // ADL
    return StronkT {log10(value.template unwrap<StronkT>())};
}

template<stronk_like StronkT>
[[nodiscard]]
constexpr auto isnan(const StronkT& elem) -> bool
{
    using std::isnan;  // ADL
    return isnan(elem.template unwrap<StronkT>());
}

template<stronk_like StronkT>
[[nodiscard]]
constexpr auto abs(const StronkT& elem) -> StronkT
{
    using std::abs;  // ADL
    return StronkT {abs(elem.template unwrap<StronkT>())};
}

template<stronk_like StronkT>
    requires(!unit_value_like<StronkT>)
[[nodiscard]]
constexpr auto sqrt(const StronkT& elem) -> StronkT
{
    using std::sqrt;  // ADL
    return StronkT {sqrt(elem.template unwrap<StronkT>())};
}

}  // namespace twig
