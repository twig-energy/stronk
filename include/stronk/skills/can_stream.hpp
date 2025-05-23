#pragma once
#include <iostream>

namespace twig
{

template<typename StronkT>
struct can_ostream
{
    friend auto operator<<(std::ostream& os, const StronkT& elem) -> std::ostream&
    {
        return os << elem.template unwrap<StronkT>();
    }
};

template<typename StronkT>
struct can_istream
{
    friend auto operator>>(std::istream& is, StronkT& elem) -> std::istream&
    {
        return is >> elem.template unwrap<StronkT>();
    }
};

template<typename StronkT>
struct can_stream
    : can_ostream<StronkT>
    , can_istream<StronkT>
{
};

}  // namespace twig
