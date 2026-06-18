#pragma once

#include <string>
#include <string_view>  // IWYU pragma: keep, used in view skill

#include "stronk/skills/can_view.hpp"
#include "stronk/stronk.hpp"

namespace twig
{

template<typename Tag, template<typename> typename... Skills>
using stronk_string =
    stronk<Tag, std::string, can_equate, can_size, can_be_const_viewed_as<std::string_view>::skill, Skills...>;

}  // namespace twig
