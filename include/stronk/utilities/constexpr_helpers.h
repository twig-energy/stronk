#pragma once
#include <type_traits>

namespace twig::stronk_details
{

template<typename T>
struct not_implemented_type : std::false_type
{
};

namespace variadic
{

template<typename T, typename... Ts>
struct first_type_of
{
    using type = T;
};

template<typename... Ts>
using first_type_of_t = typename first_type_of<Ts...>::type;

}  // namespace variadic

}  // namespace twig::stronk_details
