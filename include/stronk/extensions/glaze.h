#pragma once
#include <string_view>

#include <glaze/glaze.hpp>
#include <stronk/stronk.h>
#include <stronk/utilities/strings.h>

namespace twig
{

template<typename StronkT>
struct can_glaze_de_and_serialize
{
    constexpr static const auto can_glaze_serialize = true;
};

template<typename T>
concept can_glaze_de_and_serialize_like = stronk_like<T> && T::can_glaze_serialize;

}  // namespace twig

template<twig::can_glaze_de_and_serialize_like T>
struct glz::meta<T>
{
    constexpr static auto value = &T::_you_should_not_be_using_this_but_rather_unwrap;
};

#pragma once
