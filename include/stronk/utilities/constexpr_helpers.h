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

template<typename T, typename... Ts>
constexpr auto contains_type() -> bool
{
    return (... || std::is_same_v<T, Ts>);
}

}  // namespace variadic

}  // namespace twig::stronk_details
