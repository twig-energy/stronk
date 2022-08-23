#pragma once

#include <ostream>

#include <stronk/stronk.h>

namespace twig
{

template<typename StronkT>
struct can_gtest_print
{
    friend void PrintTo(const StronkT& val, std::ostream* os)
    {
        *os << val.template unwrap<StronkT>();
    }
};

}  // namespace twig
