#pragma once
#include <cstdint>

namespace twig::str
{

// A string literal, useful for templating on compile time known strings.
template<size_t N>
struct StringLiteral
{
    consteval explicit(false) StringLiteral(const char (&str)[N])  // NOLINT
    {
        std::copy_n(static_cast<const char*>(str), N, this->value);  // NOLINT
    }

    char value[N];  // NOLINT
};

}  // namespace twig::str
