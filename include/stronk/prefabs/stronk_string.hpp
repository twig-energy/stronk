#pragma once

#include <string>

#include "stronk/stronk.hpp"

namespace twig
{

template<typename Tag, template<typename> typename... Skills>
using stronk_string = stronk<Tag, std::string, can_equate, can_size, Skills...>;

}  // namespace twig
