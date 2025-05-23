#pragma once

#include <concepts>
#include <ostream>

#include "stronk/stronk.hpp"

namespace twig
{

template<typename T>
concept is_stronk_and_can_ostream = stronk_like<T> && requires(T v, std::ostream& os) {
    {
        os << v.template unwrap<T>()
    } -> std::same_as<std::ostream&>;
};

template<twig::is_stronk_and_can_ostream StronkT>
void PrintTo(const StronkT& val, std::ostream* os)
{
    *os << val.template unwrap<StronkT>();
}

}  // namespace twig
