#pragma once
#include <glaze/core/meta.hpp>
#include <stronk/stronk.h>

template<twig::stronk_like T>
struct glz::meta<T>
{
    constexpr static auto value = &T::_you_should_not_be_using_this_but_rather_unwrap;
};
