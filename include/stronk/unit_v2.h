#pragma once
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
    typename T::template value<void>;
};

template<typename T>
concept unit_value_like = ::twig::stronk_like<T> && requires { typename T::unit_t; };

// Implementations

// Tag should either be the unit itself, or a dimensions_like type defining the dimensions of the unit.
template<typename Tag, template<typename StronkT> typename... SkillTs>
struct unit
{
    using dimensions_t = std::conditional_t<dimensions_like<Tag>, Tag, twig::create_dimensions_t<dimension<Tag, 1>>>;

    unit() = delete;  // Do not construct this type

    template<typename UnderlyingT>
    struct value : ::twig::stronk<value<UnderlyingT>, UnderlyingT, SkillTs...>
    {
        using unit_t = unit;
        using base_t = ::twig::stronk<value<UnderlyingT>, UnderlyingT, SkillTs...>;
        using base_t::base_t;

        // can convert to same unit with different underlying type
        template<typename T>
        constexpr explicit operator value<T>() const
        {
            return value<T> {static_cast<T>(this->val())};
        }
    };
};

template<>
struct unit<::twig::create_dimensions_t<>>
{
    using dimensions_t = ::twig::create_dimensions_t<>;

    unit() = delete;  // Do not construct this type

    template<typename UnderlyingT>
    using value = UnderlyingT;
};
using identity_unit = unit<::twig::create_dimensions_t<>>;

template<typename Tag, template<typename> typename... Skills>
using stronk_default_unit = unit<Tag,
                                 can_add,
                                 can_subtract,
                                 can_negate,
                                 can_order,
                                 can_equate_underlying_type_specific,
                                 transform_skill,
                                 Skills...>;

template<unit_like UnitT, typename UnderlyingT>
using unit_value_t = typename UnitT::template value<UnderlyingT>;

/**
 * @brief Lookup which specific type is the type for the given Dimensions.
 */
template<::twig::dimensions_like DimensionsT>
struct unit_lookup
{
    using unit_t = stronk_default_unit<DimensionsT>;
};

template<>
struct unit_lookup<::twig::create_dimensions_t<>>
{
    using unit_t = identity_unit;
};

template<::twig::dimensions_like DimensionsT>
    requires(DimensionsT::is_pure())
struct unit_lookup<DimensionsT>
{
    using unit_t = typename DimensionsT::first_t::unit_t;
};

// ==================
// Multiply
// ==================

template<unit_like A, unit_like B>
using multiplied_dimensions_t = typename A::dimensions_t::template multiply_t<typename B::dimensions_t>;

template<unit_like A, unit_like B>
using multiplied_unit_t = typename unit_lookup<multiplied_dimensions_t<A, B>>::unit_t;

// You can specialize this struct if you want another underlying multiply operation
template<unit_value_like T1, unit_value_like T2>
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

    using dimensions_t = multiplied_dimensions_t<typename A::unit_t, typename B::unit_t>;
    using resulting_unit = typename unit_lookup<dimensions_t>::unit_t;
    using underlying_t = decltype(res);

    using value_t = typename resulting_unit::template value<underlying_t>;
    return value_t {res};
}

template<typename T, unit_value_like B>
    requires(!unit_value_like<T>)
constexpr auto operator*(const T& a, const B& b) noexcept -> B
{
    return B {a * b.template unwrap<B>()};
}

template<unit_value_like A, typename T>
    requires(!unit_value_like<T>)
constexpr auto operator*(const A& a, const T& b) noexcept -> A
{
    return A {a.template unwrap<A>() * b};
}

template<unit_value_like A, typename T>
    requires(!unit_value_like<T>)
constexpr auto operator*=(A& a, const T& b) noexcept -> A&
{
    a.template unwrap<A>() *= b;
    return a;
}

// ==================
// Divide
// ==================

template<unit_like A, unit_like B>
using divided_dimensions_t = typename A::dimensions_t::template divide_t<typename B::dimensions_t>;

template<unit_like A, unit_like B>
using divided_unit_t = typename unit_lookup<divided_dimensions_t<A, B>>::unit_t;

// You can specialize this struct if you want another underlying divide operation
template<unit_value_like T1, unit_value_like T2>
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

    using dimensions_t = divided_dimensions_t<typename A::unit_t, typename B::unit_t>;
    using resulting_unit = typename unit_lookup<dimensions_t>::unit_t;
    using underlying_t = decltype(res);

    using value_t = typename resulting_unit::template value<underlying_t>;
    return value_t {res};
}

template<typename T, unit_value_like B>
    requires(!unit_value_like<T>)
constexpr auto operator/(const T& a, const B& b) noexcept
{
    auto res = a / b.template unwrap<B>();
    using new_dimensions = typename B::unit_t::dimensions_t::negate_t;
    using underlying_type = decltype(res);
    using new_unit = typename unit_lookup<new_dimensions>::unit_t;
    using new_unit_value = new_unit::template value<underlying_type>;
    return new_unit_value {res};
}

template<unit_value_like A, typename T>
    requires(!unit_value_like<T>)
constexpr auto operator/(const A& a, const T& b) noexcept -> A
{
    return A {a.template unwrap<A>() / b};
}

template<unit_value_like A, typename T>
    requires(!unit_value_like<T>)
constexpr auto operator/=(A& a, const T& b) noexcept -> A&
{
    a.template unwrap<A>() /= b;
    return a;
}

template<unit_like UnitT, typename UnderlyingT>
constexpr auto make(UnderlyingT&& value)
{
    return unit_value_t<UnitT, UnderlyingT> {std::forward<UnderlyingT>(value)};
}

}  // namespace twig::unit_v2
