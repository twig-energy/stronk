#pragma once

#include <vector>

#include "stronk/skills/can_index.hpp"
#include "stronk/skills/can_iterate.hpp"
#include "stronk/stronk.hpp"

namespace twig
{

template<typename Tag, typename InnerT, template<typename> typename... Skills>
using stronk_vector = stronk<
    Tag,
    std::vector<InnerT>,
    can_equate,
    can_size,
    can_index,
    can_iterate,
    can_be_const_viewed_as<std::span<const InnerT>, can_equate, can_size, can_iterate, can_index>::template skill,
    can_be_mutable_viewed_as<std::span<InnerT>, can_equate, can_size, can_iterate, can_index>::template skill,
    Skills...>;

}  // namespace twig
