#pragma once
#include <format>

#include <stronk/stronk.h>

template<twig::stronk_like StronkT>
struct std::formatter<StronkT> : std::formatter<typename StronkT::underlying_type>
{
    auto format(const StronkT& val, std::format_context& context) const
    {
        return std::formatter<typename StronkT::underlying_type>::format(val.template unwrap<StronkT>(), context);
    }
};
