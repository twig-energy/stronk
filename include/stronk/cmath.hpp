#pragma once

#include <cmath>

#include "stronk/stronk.hpp"
#include "stronk/unit.hpp"

namespace twig
{

template<stronk_like StronkT>
    requires(!unit_value_like<StronkT>)  // units should divide by T(1) first
auto log(StronkT value) -> StronkT
{
    using std::log;  // ADL
    return StronkT {log(value.template unwrap<StronkT>())};
}

template<stronk_like StronkT>
    requires(!unit_value_like<StronkT>)  // units should divide by T(1) first
auto log2(StronkT value) -> StronkT
{
    using std::log2;  // ADL
    return StronkT {log2(value.template unwrap<StronkT>())};
}

template<stronk_like StronkT>
    requires(!unit_value_like<StronkT>)  // units should divide by T(1) first
auto log10(StronkT value) -> StronkT
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

// === Unit special functions ===

template<unit_value_like UnitT>
constexpr auto sqrt(const UnitT& elem) -> auto
{
    using scale_t = ratio_sqrt<typename UnitT::unit_t::scale_t>;
    using resulting_unit_t =
        twig::unit_lookup<typename UnitT::unit_t::dimensions_t::template root_t<2>>::template unit_t<scale_t>;
    using std::sqrt;  // ADL
    return make<resulting_unit_t>(sqrt(elem.template unwrap<UnitT>()));
}

template<int PowerV, unit_value_like UnitT>
constexpr auto pow(const UnitT& elem) -> auto
{
    using scale_t = twig::ratio_pow<typename UnitT::unit_t::scale_t, PowerV>;
    using resulting_unit_t =
        twig::unit_lookup<typename UnitT::unit_t::dimensions_t::template power_t<PowerV>>::template unit_t<scale_t>;
    using std::pow;
    return make<resulting_unit_t>(pow(elem.template unwrap<UnitT>(), static_cast<double>(PowerV)));
}

}  // namespace twig
