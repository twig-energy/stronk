#pragma once
#include <string>

#include <fmt/compile.h>
#include <fmt/core.h>
#include <stronk/stronk.h>

namespace twig
{
template<stronk_details::str::StringLiteral FormatStringV>
struct can_fmt_format_builder
{
    template<typename StronkT>
    struct skill
    {
        constexpr static const auto fmt_string = FormatStringV;
    };
};

template<typename StronkT>
struct can_fmt_format
{
    constexpr static const auto fmt_string = stronk_details::str::StringLiteral("{}");
};

template<typename T>
concept can_fmt_format_like = stronk_like<T> && requires(T v)
{
    {
        T::fmt_string.value
        } -> std::convertible_to<std::string_view>;
};

}  // namespace twig

template<twig::can_fmt_format_like T>
struct fmt::formatter<T> : formatter<string_view>
{
    template<typename FormatContext>
    auto format(const T& val, FormatContext& ctx) const
    {
        return formatter<string_view>::format(fmt::format(FMT_COMPILE(T::fmt_string.value), val.template unwrap<T>()),
                                              ctx);
    }
};
