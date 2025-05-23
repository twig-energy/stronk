#pragma once
// IWYU pragma: always_keep

#include <glaze/core/meta.hpp>

#include "stronk/stronk.hpp"

template<twig::stronk_like StronkT>
struct glz::meta<StronkT>
{
    using T = StronkT;
    constexpr static auto value = &T::_you_should_not_be_using_this_but_rather_unwrap;
};
