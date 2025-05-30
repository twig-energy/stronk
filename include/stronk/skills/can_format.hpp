#pragma once
// IWYU pragma: always_keep

#include <format>

#include "stronk/stronk.hpp"

template<twig::stronk_like StronkT>
struct std::formatter<StronkT>  // NOLINT(cert-dcl58-cpp) this is meant to be specialized
    : std::formatter<typename StronkT::underlying_type>
{
    constexpr auto format(const StronkT& val, std::format_context& context) const
    {
        return std::formatter<typename StronkT::underlying_type>::format(val.template unwrap<StronkT>(), context);
    }
};
