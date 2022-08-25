#pragma once
#include <stronk/stronk.h>
#include <stronk/unit.h>

namespace twig
{

template<typename Tag, typename T, template<typename> typename... Skills>
struct stronk_default_unit
    : stronk<Tag, T, unit, can_add, can_subtract, can_negate, can_order, Skills...>
    , default_can_equate<T, Tag>
{
    using stronk_base = stronk<Tag, T, unit, can_add, can_subtract, can_negate, can_order, Skills...>;
    using stronk_base::stronk_base;
};

template<typename Tag, template<typename> typename... Skills>
struct stronk_flag : stronk<Tag, bool, can_be_used_as_flag, can_order, can_equate, Skills...>
{
    using stronk_base = stronk<Tag, bool, can_be_used_as_flag, can_order, can_equate, Skills...>;
    using stronk_base::stronk_base;
};

}  // namespace twig
