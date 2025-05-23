#pragma once
#include <format>

#include "stronk/stronk.hpp"

template<twig::stronk_like StronkT>
struct std::formatter<StronkT> : std::formatter<typename StronkT::underlying_type>
{
    constexpr auto format(const StronkT& val, std::format_context& context) const
    {
        return std::formatter<typename StronkT::underlying_type>::format(val.template unwrap<StronkT>(), context);
    }
};
