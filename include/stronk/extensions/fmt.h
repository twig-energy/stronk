#pragma once
#include <string_view>

#include <fmt/compile.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <stronk/stronk.h>
#include <stronk/utilities/strings.h>

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
concept can_fmt_format_like = stronk_like<T> && requires(T v) {
    {
        T::fmt_string.value
    } -> std::convertible_to<std::string_view>;
};

}  // namespace twig

template<twig::can_fmt_format_like T>
struct fmt::formatter<T> : formatter<string_view>
{
    constexpr static auto fmt_string = std::string_view(T::fmt_string.value);

    template<typename FormatContext>
    auto format(const T& val, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), FMT_COMPILE(fmt::formatter<T>::fmt_string), val.template unwrap<T>());
    }
};
