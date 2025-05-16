#pragma once
#include <stronk/stronk.h>

namespace twig
{

template<typename Tag, template<typename> typename... Skills>
struct stronk_flag : stronk<Tag, bool, can_be_used_as_flag, can_order, can_equate, Skills...>
{
    using stronk<Tag, bool, can_be_used_as_flag, can_order, can_equate, Skills...>::stronk;
};

}  // namespace twig
