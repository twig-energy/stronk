#pragma once
#include <string>

#include <fmt/compile.h>
#include <fmt/core.h>
#include <stronk/stronk.h>

template<twig::can_format_like T>
struct fmt::formatter<T> : formatter<string_view>
{
    template<typename FormatContext>
    auto format(const T& val, FormatContext& ctx) const
    {
        return formatter<string_view>::format(fmt::format(FMT_COMPILE(T::fmt_string.value), val.template unwrap<T>()),
                                              ctx);
    }
};
