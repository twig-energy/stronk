#pragma once
#include <stronk/stronk.h>
#include <stronk/unit.h>

namespace twig
{

template<typename Tag, typename T, template<typename> typename... Skills>
struct stronk_default_unit
    : stronk<Tag,
             T,
             unit,
             can_add,
             can_subtract,
             can_negate,
             can_order,
             transform_skill,
             default_can_equate_builder<T>::template skill,
             Skills...>
{
    using stronk<Tag,
                 T,
                 unit,
                 can_add,
                 can_subtract,
                 can_negate,
                 can_order,
                 transform_skill,
                 default_can_equate_builder<T>::template skill,
                 Skills...>::stronk;
};

template<typename Tag, template<typename> typename... Skills>
struct stronk_flag : stronk<Tag, bool, can_be_used_as_flag, can_order, can_equate, Skills...>
{
    using stronk<Tag, bool, can_be_used_as_flag, can_order, can_equate, Skills...>::stronk;
};

}  // namespace twig
