#pragma once
#include <array>
#include <cstddef>
#include <string_view>

namespace twig::stronk_details::str
{

// A string literal, useful for templating on compile time known strings.
template<size_t N>
struct string_literal
{
    std::array<char, N> value {};

    consteval explicit(false) string_literal(const char (&str)[N]) noexcept  // NOLINT
    {
        for (auto i = 0ULL; i < N; i++) {
            value[i] = str[i];
        }
    }

    constexpr explicit operator std::string_view() const noexcept
    {
        return std::string_view {this->value.data(), N - 1};
    }

    constexpr auto operator==(std::string_view other) const noexcept -> bool
    {
        return static_cast<std::string_view>(*this) == other;
    }
};

}  // namespace twig::stronk_details::str
