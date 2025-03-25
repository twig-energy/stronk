#pragma once
#include <concepts>
#include <type_traits>
#include <utility>

#include <stronk/stronk.h>
#include <stronk/utilities/dimensions.h>
#include <stronk/utilities/macros.h>

namespace twig::unit_v2
{

// Concepts
template<typename T>
concept unit_like = requires {
    typename T::dimensions_t;
    typename T::template value<int>;  // dont know how to express this without int
};

template<typename T>
concept unit_value_like = ::twig::stronk_like<T> && requires { typename T::unit_t; };

// Implementations

struct identity_unit
{
    using dimensions_t = ::twig::create_dimensions_t<>;

    template<typename UnderlyingT>
    struct value
    {
        UnderlyingT val;

        constexpr operator UnderlyingT() const  // NOLINT
        {
            return val;
        }

        constexpr auto operator<=>(const value& other) const = default;

        using unit_t = identity_unit;
    };
};

template<typename DimensionT>
struct unit
{
    using dimensions_t = ::twig::create_dimensions_t<::twig::dimension<DimensionT, 1>>;
};

template<unit_like T>
struct value_of_unit
{
    using unit_t = T;
};

template<::twig::dimensions_like DimensionsT>
struct new_stronk_unit
{
    using dimensions_t = DimensionsT;

    // You can specialize this type if you want to add other skills to your dynamic types.
    template<typename UnderlyingT>
    struct value
        : ::twig::stronk<value<UnderlyingT>,
                         UnderlyingT,
                         ::twig::can_order,
                         ::twig::can_add,
                         ::twig::can_subtract,
                         ::twig::can_negate,
                         ::twig::default_can_equate_builder<UnderlyingT>::template skill>
        , value_of_unit<new_stronk_unit>
    {
        using stronk_base_t = ::twig::stronk<value<UnderlyingT>,
                                             UnderlyingT,
                                             ::twig::can_order,
                                             ::twig::can_add,
                                             ::twig::can_subtract,
                                             ::twig::can_negate,
                                             ::twig::default_can_equate_builder<UnderlyingT>::template skill>;
        using stronk_base_t::stronk_base_t;
    };
};

template<unit_like UniT, typename UnderlyingT>
using value_of_unit_t = typename UniT::template value<UnderlyingT>;

/**
 * @brief Lookup which specific type is the type for the given Dimensions.
 */
template<::twig::dimensions_like DimensionsT>
struct unit_lookup
{
    using type = new_stronk_unit<DimensionsT>;
};

template<unit_like StronkT, typename UnderlyingT>
auto choose_return_type(UnderlyingT res)
{
    using dimensions_t = typename StronkT::dimensions_t;
    if constexpr (dimensions_t::empty()) {
        return identity_unit::value<UnderlyingT> {res};
    } else if constexpr (dimensions_t::is_pure()) {
        using pure_value_t = typename dimensions_t::first_t::unit_t::template value<UnderlyingT>;
        return pure_value_t {res};
    } else {
        using unit_value_t = typename StronkT::template value<UnderlyingT>;
        return unit_value_t {res};
    }
}

// ==================
// Multiply
// ==================

template<unit_value_like A, unit_value_like B>
struct multiplied_dimensions
{
    // This is the result:
    using dimensions_t = typename A::unit_t::dimensions_t::template multiply_t<typename B::unit_t::dimensions_t>;
};

// You can specialize this struct if you want another underlying multiply operation
template<::twig::stronk_like T1, ::twig::stronk_like T2>
struct underlying_multiply_operation
{
    using res_type =
        decltype(std::declval<typename T1::underlying_type>() * std::declval<typename T2::underlying_type>());

    STRONK_FORCEINLINE
    constexpr static auto multiply(const typename T1::underlying_type& v1,
                                   const typename T2::underlying_type& v2) noexcept -> res_type
    {
        return v1 * v2;
    }
};

template<unit_value_like A, unit_value_like B>
STRONK_FORCEINLINE constexpr auto operator*(const A& a, const B& b) noexcept
{
    auto res = underlying_multiply_operation<A, B>::multiply(a.template unwrap<A>(), b.template unwrap<B>());

    using dimensions_t = typename multiplied_dimensions<A, B>::dimensions_t;
    using resulting_unit = typename unit_lookup<dimensions_t>::type;

    // check that the type is setup correctly. It might have been specialized.
    static_assert(std::is_same_v<dimensions_t, typename resulting_unit::dimensions_t>,
                  "Seems to be a mismatch in units for your specialized type. Maybe you added the wrong skill. "
                  "See multiplied_dimensions<A,B>::skill");
    return choose_return_type<resulting_unit>(res);
}

template<unit_like A, unit_like B>
using multiply_t = decltype(value_of_unit_t<A, int>() * value_of_unit_t<B, int>())::unit_t;

template<typename T, unit_value_like B>
    requires(std::floating_point<T> || std::integral<T>)
constexpr auto operator*(const T& a, const B& b) noexcept -> B
{
    return B {a * b.template unwrap<B>()};
}

template<unit_value_like A, typename T>
    requires(std::floating_point<T> || std::integral<T>)
constexpr auto operator*(const A& a, const T& b) noexcept -> A
{
    return A {a.template unwrap<A>() * b};
}

template<unit_value_like A, typename T>
    requires(std::floating_point<T> || std::integral<T>)
constexpr auto operator*=(A& a, const T& b) noexcept -> A&
{
    a.template unwrap<A>() *= b;
    return a;
}

// ==================
// Divide
// ==================

template<unit_value_like A, unit_value_like B>
struct divided_dimensions
{
    // This is the result:
    using dimensions_t = typename A::unit_t::dimensions_t::template divide_t<typename B::unit_t::dimensions_t>;
};

// You can specialize this struct if you want another underlying divide operation
template<::twig::stronk_like T1, ::twig::stronk_like T2>
struct underlying_divide_operation
{
    using res_type =
        decltype(std::declval<typename T1::underlying_type>() / std::declval<typename T2::underlying_type>());

    STRONK_FORCEINLINE
    constexpr static auto divide(const typename T1::underlying_type& v1,
                                 const typename T2::underlying_type& v2) noexcept -> res_type
    {
        return v1 / v2;
    }
};

template<unit_value_like A, unit_value_like B>
STRONK_FORCEINLINE constexpr auto operator/(const A& a, const B& b) noexcept
{
    auto res = underlying_divide_operation<A, B>::divide(a.template unwrap<A>(), b.template unwrap<B>());

    using dimensions_t = typename divided_dimensions<A, B>::dimensions_t;
    using resulting_unit = typename unit_lookup<dimensions_t>::type;
    // check that the type is setup correctly. It might have been specialized.
    static_assert(std::is_same_v<dimensions_t, typename resulting_unit::dimensions_t>,
                  "Seems to be a mismatch in units for your specialized type. Maybe you added the wrong skill. "
                  "See divided_dimensions<A,B>::skill");
    return choose_return_type<resulting_unit>(res);
}

template<unit_like A, unit_like B>
using divide_t = decltype(value_of_unit_t<A, int>() / value_of_unit_t<B, int>())::unit_t;

template<typename T, unit_value_like B>
    requires(std::floating_point<T> || std::integral<T>)
constexpr auto operator/(const T& a, const B& b) noexcept
{
    auto res = a / b.template unwrap<B>();
    using new_dimensions = typename B::unit_t::dimensions_t::negate_t;
    using underlying_type = decltype(a / b.template unwrap<B>());
    using new_unit = typename unit_lookup<new_dimensions>::type;
    using new_unit_value = new_unit::template value<underlying_type>;
    return new_unit_value {res};
}

template<unit_value_like A, typename T>
    requires(std::floating_point<T> || std::integral<T>)
constexpr auto operator/(const A& a, const T& b) noexcept -> A
{
    return A {a.template unwrap<A>() / b};
}

template<unit_value_like A, typename T>
    requires(std::floating_point<T> || std::integral<T>)
constexpr auto operator/=(A& a, const T& b) noexcept -> A&
{
    a.template unwrap<A>() /= b;
    return a;
}

}  // namespace twig::unit_v2
