#pragma once
// IWYU pragma: always_keep

#include <concepts>

#include <doctest/doctest.h>

#include "stronk/stronk.hpp"

namespace twig
{
template<typename U>
concept has_doctest_to_string = requires(U val) {
    { doctest::toString(val) } -> std::same_as<doctest::String>;
};
}  // namespace twig

template<twig::stronk_like T>
struct doctest::StringMaker<T>
{
    static auto convert(const T& val) -> doctest::String
    {
        using underlying_t = typename T::underlying_type;
        if constexpr (twig::has_doctest_to_string<underlying_t>) {
            return doctest::toString(val.template unwrap<T>());
        } else {
            return doctest::StringMaker<typename T::underlying_type>::convert(val.template unwrap<T>());
        }
    }
};
