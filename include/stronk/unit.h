#pragma once
#include <type_traits>
#include <utility>

#include <stronk/stronk.h>
#include <stronk/utilities/dimension_type_list.h>
#include <stronk/utilities/macros.h>
#include <stronk/utilities/type_list.h>

namespace twig
{

template<typename StronkT>
struct unit
{
    constexpr static auto dimensions = Dimensions<Dimension<StronkT, 1>> {};
    using dimensions_t = Dimensions<Dimension<StronkT, 1>>;
};

template<dimensions_like DimensionsT>
struct unit_skill_builder
{
    template<typename StronkT>
    struct skill
    {
        constexpr static auto dimensions = DimensionsT {};
        using dimensions_t = DimensionsT;
    };
};

template<typename T>
concept unit_like = stronk_like<T> && requires { T::dimensions; };

template<typename T>
concept identity_unit_like = unit_like<T> && requires { T::dimensions::empty(); };

// You can specialize this type if you want to add other skills to your dynamic types.
template<typename UnderlyingT, dimensions_like DimensionsT>
struct NewStronkUnit
    : stronk<NewStronkUnit<UnderlyingT, DimensionsT>,
             UnderlyingT,
             can_order,
             can_add,
             can_subtract,
             can_negate,
             default_can_equate_builder<UnderlyingT>::template skill,
             unit_skill_builder<DimensionsT>::template skill>
{
    constexpr static auto dimensions = DimensionsT {};
    using dimensions_t = DimensionsT;

    using stronk<NewStronkUnit<UnderlyingT, DimensionsT>,
                 UnderlyingT,
                 can_order,
                 can_add,
                 can_subtract,
                 can_negate,
                 default_can_equate_builder<UnderlyingT>::template skill,
                 unit_skill_builder<DimensionsT>::template skill>::stronk;
};

/**
 * @brief Lookup which specific type is the type for the given Dimensions.
 */
template<dimensions_like DimensionsT, typename UnderlyingT>
struct unit_lookup
{
    using type = NewStronkUnit<UnderlyingT, DimensionsT>;
};

// ==================
// Multiply
// ==================

template<unit_like A, unit_like B>
struct multiplied_unit
{
    // This is the result:
    using dimensions_t = typename A::dimensions_t::template multiply_t<typename B::dimensions_t>;

    template<typename StronkT>
    using skill = typename unit_skill_builder<dimensions_t>::template skill<StronkT>;
};

// You can specialize this struct if you want another underlying multiply operation
template<stronk_like T1, stronk_like T2>
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

template<unit_like A, unit_like B>
STRONK_FORCEINLINE constexpr auto operator*(const A& a, const B& b) noexcept
{
    auto res = underlying_multiply_operation<A, B>::multiply(a.template unwrap<A>(), b.template unwrap<B>());

    using type_lists = typename multiplied_unit<A, B>::dimensions_t;
    using resulting_unit = typename unit_lookup<type_lists, decltype(res)>::type;

    // check that the type is setup correctly. It might have been specialized.
    static_assert(std::is_same_v<type_lists, typename resulting_unit::dimensions_t>,
                  "Seems to be a mismatch in units for your specialized type. Maybe you added the wrong skill. "
                  "See multiplied_unit<A,B>::skill");
    if constexpr (type_lists::empty()) {
        return res;
    } else if constexpr (type_lists::is_pure()) {
        using pure_t = typename resulting_unit::dimensions_t::first_t::unit_t;
        return pure_t {static_cast<typename pure_t::underlying_type>(res)};
    } else {
        return resulting_unit {res};
    }
}

template<unit_like A, unit_like B>
using multiply_t = decltype(A() * B());

template<identity_unit_like T>
constexpr auto operator*(const T& a, const T& b) noexcept -> T
{
    return T {a.template unwrap<T>() * b.template unwrap<T>()};
}

template<identity_unit_like T, unit_like B>
constexpr auto operator*(const T& a, const B& b) noexcept -> B
{
    return B {a.template unwrap<T>() * b.template unwrap<B>()};
}

template<unit_like A, identity_unit_like T>
constexpr auto operator*(const A& a, const T& b) noexcept -> A
{
    return A {a.template unwrap<A>() * b.template unwrap<T>()};
}

template<typename T, unit_like B>
    requires(std::floating_point<T> || std::integral<T>)
constexpr auto operator*(const T& a, const B& b) noexcept -> B
{
    return B {a * b.template unwrap<B>()};
}

template<unit_like A, typename T>
    requires(std::floating_point<T> || std::integral<T>)
constexpr auto operator*(const A& a, const T& b) noexcept -> A
{
    return A {a.template unwrap<A>() * b};
}

template<unit_like A, typename T>
    requires(std::floating_point<T> || std::integral<T>)
constexpr auto operator*=(A& a, const T& b) noexcept -> A&
{
    a.template unwrap<A>() *= b;
    return a;
}

template<unit_like A, identity_unit_like T>
constexpr auto operator*=(A& a, const T& b) noexcept -> A&
{
    a.template unwrap<A>() *= b.template unwrap<T>();
    return a;
}

// ==================
// Divide
// ==================

template<unit_like A, unit_like B>
struct divided_unit
{
    // This is the result:
    using dimensions_t = typename A::dimensions_t::template divide_t<typename B::dimensions_t>;

    template<typename StronkT>
    using skill = unit_skill_builder<dimensions_t>::template skill<StronkT>;
};

// You can specialize this struct if you want another underlying divide operation
template<stronk_like T1, stronk_like T2>
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

template<unit_like A, unit_like B>
STRONK_FORCEINLINE constexpr auto operator/(const A& a, const B& b) noexcept
{
    auto res = underlying_divide_operation<A, B>::divide(a.template unwrap<A>(), b.template unwrap<B>());

    using type_lists = typename divided_unit<A, B>::dimensions_t;
    using resulting_unit = typename unit_lookup<type_lists, decltype(res)>::type;
    // check that the type is setup correctly. It might have been specialized.
    static_assert(std::is_same_v<type_lists, typename resulting_unit::dimensions_t>,
                  "Seems to be a mismatch in units for your specialized type. Maybe you added the wrong skill. "
                  "See divided_unit<A,B>::skill");
    if constexpr (type_lists::empty()) {
        return res;
    } else if constexpr (type_lists::is_pure()) {
        using pure_t = typename resulting_unit::dimensions_t::first_t::unit_t;
        return pure_t {static_cast<typename pure_t::underlying_type>(res)};
    } else {
        return resulting_unit {res};
    }
}

template<unit_like A, unit_like B>
using divide_t = decltype(A() / B());

template<identity_unit_like T>
constexpr auto operator/(const T& a, const T& b) noexcept -> T
{
    return T {a.template unwrap<T>() / b.template unwrap<T>()};
}

template<identity_unit_like T, unit_like B>
constexpr auto operator/(const T& a, const B& b) noexcept
{
    using new_dimensions = B::dimensions_t::negate_t;
    using new_unit = typename unit_lookup<new_dimensions, typename B::underlying_type>::type;
    return new_unit {a.template unwrap<T>() / b.template unwrap<B>()};
}

template<unit_like A, identity_unit_like T>
constexpr auto operator/(const A& a, const T& b) noexcept -> A
{
    return A {a.template unwrap<A>() / b.template unwrap<T>()};
}

template<typename T, unit_like B>
    requires(std::floating_point<T> || std::integral<T>)
constexpr auto operator/(const T& a, const B& b) noexcept
{
    using new_dimensions = B::dimensions_t::negate_t;
    using new_unit = typename unit_lookup<new_dimensions, typename B::underlying_type>::type;
    return new_unit {a / b.template unwrap<B>()};
}

template<unit_like A, typename T>
    requires(std::floating_point<T> || std::integral<T>)
constexpr auto operator/(const A& a, const T& b) noexcept -> A
{
    return A {a.template unwrap<A>() / b};
}

template<unit_like A, typename T>
    requires(std::floating_point<T> || std::integral<T>)
constexpr auto operator/=(A& a, const T& b) noexcept -> A&
{
    a.template unwrap<A>() /= b;
    return a;
}
template<unit_like A, identity_unit_like T>
constexpr auto operator/=(A& a, const T& b) noexcept -> A&
{
    a.template unwrap<A>() /= b.template unwrap<T>();
    return a;
}

}  // namespace twig
