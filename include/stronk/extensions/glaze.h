#pragma once
#include <glaze/core/meta.hpp>
#include <stronk/stronk.h>
#include <stronk/utilities/strings.h>

namespace twig
{

template<typename StronkT>
struct can_glaze_de_and_serialize
{
    struct glaze
    {
        constexpr static auto value = &StronkT::_you_should_not_be_using_this_but_rather_unwrap;
    };
};

}  // namespace twig
