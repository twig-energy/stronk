#pragma once
// IWYU pragma: always_keep

#include <concepts>
#include <string_view>
#include <type_traits>

#include <fmt/base.h>
#include <fmt/compile.h>
#include <fmt/core.h>
#include <fmt/format.h>

#include "stronk/stronk.hpp"
#include "stronk/utilities/strings.hpp"

namespace twig
{
template<stronk_details::str::string_literal FormatStringV>
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
    constexpr static const auto fmt_string = stronk_details::str::string_literal("{}");
};

template<typename T>
concept can_special_fmt_format_like = stronk_like<T> && requires(T v) {
    {
        T::fmt_string.value
    } -> std::convertible_to<std::string_view>;
};

}  // namespace twig

template<twig::can_special_fmt_format_like T>
struct fmt::formatter<T>
    : std::conditional_t<std::string_view(T::fmt_string.value) == "{}",
                         formatter<typename T::underlying_type>,
                         formatter<fmt::string_view>>
{
    constexpr static auto fmt_string = std::string_view(T::fmt_string.value);

    template<typename FormatContext>
    auto format(const T& val, FormatContext& ctx) const
    {
        if constexpr (fmt_string == "{}") {
            return formatter<typename T::underlying_type>::format(val.template unwrap<T>(), ctx);
        } else {
            return formatter<fmt::string_view>::format(fmt::format(FMT_COMPILE(fmt_string), val.template unwrap<T>()),
                                                       ctx);
        }
    }
};

/**
 * @brief Allows all stronk values to be fmt formattable.
 *   Use the can_fmt_format skill to specify the format string.
 */
template<twig::stronk_like T>
    requires(!twig::can_special_fmt_format_like<T>)
struct fmt::formatter<T> : formatter<typename T::underlying_type>
{
    template<typename FormatContext>
    auto format(const T& val, FormatContext& ctx) const
    {
        return formatter<typename T::underlying_type>::format(val.template unwrap<T>(), ctx);
    }
};
