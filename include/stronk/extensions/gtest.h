#pragma once

#include <ostream>

#include <stronk/stronk.h>

namespace twig
{

template<twig::stronk_like StronkT>
void PrintTo(const StronkT& val, std::ostream* os)
{
    *os << val.template unwrap<StronkT>();
}

}  // namespace twig
