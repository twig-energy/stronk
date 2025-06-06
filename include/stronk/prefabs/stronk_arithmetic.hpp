#pragma once

#include "stronk/stronk.hpp"

namespace twig
{

template<typename Tag, typename InnerT, template<typename> typename... Skills>
using stronk_arithmetic = stronk<Tag, InnerT, can_equate, can_order, can_add, can_subtract, can_negate, Skills...>;

}  // namespace twig
