#pragma once
#include <algorithm>
#include <array>
#include <cstddef>

namespace twig::stronk_details::str
{

// A string literal, useful for templating on compile time known strings.
template<size_t N>
struct string_literal
{
    std::array<char, N> value {};  // NOLINT

    consteval explicit(false) string_literal(const char (&str)[N]) noexcept  // NOLINT
    {
        std::copy_n(static_cast<const char*>(str), N, this->value.data());  // NOLINT
    }
};

}  // namespace twig::stronk_details::str
