#pragma once
#include <concepts>
#include <type_traits>
#include <utility>

#include <stronk/utilities/dimensions.hpp>
#include <stronk/utilities/macros.hpp>

#include "stronk/stronk.hpp"
#include "stronk/utilities/ratio.hpp"

namespace twig
{

// Concepts
template<typename T>
concept unit_like = requires {
    typename T::dimensions_t;
    typename T::template value<void>;
};

template<typename T>
concept unit_value_like = ::twig::stronk_like<T> && requires { typename T::unit_t; };

template<typename T, typename ValueT>
concept is_unit = std::same_as<typename T::unit_t::dimensions_t, typename ValueT::dimensions_t>;

template<typename T>
concept scale_like = requires {
    T::num;
    T::den;
};

template<typename T>
concept canonical_scale_like =
    std::same_as<typename twig::ratio<T::num, T::den>::type, T> /*ratio is fully reduced*/ && scale_like<T>;

// Implementations

// Tag should either be the unit itself, or a dimensions_like type defining the dimensions of the unit.
template<typename Tag, canonical_scale_like ScaleT, template<typename StronkT> typename... SkillTs>
struct unit
{
    using dimensions_t = std::conditional_t<dimensions_like<Tag>, Tag, twig::create_dimensions_t<dimension<Tag, 1>>>;
    using scale_t = ScaleT;

    unit() = delete;  // Do not construct this type

    // we recreate ratio to make sure we have the gcd version of the scale.
    template<scale_like OtherScaleT>
    using scaled_t = unit<Tag, typename OtherScaleT::type, SkillTs...>;

    template<typename UnderlyingT>
    struct value : ::twig::stronk<value<UnderlyingT>, UnderlyingT, SkillTs...>
    {
        using unit_t = unit;
        using base_t = ::twig::stronk<value<UnderlyingT>, UnderlyingT, SkillTs...>;
        using base_t::base_t;

        // can static_cast to same unit with different underlying type
        template<typename NewUnderlyingT>
            requires(!std::same_as<NewUnderlyingT, UnderlyingT>)
        constexpr explicit operator value<NewUnderlyingT>() const
        {
            return value<NewUnderlyingT> {static_cast<NewUnderlyingT>(this->val())};
        }

        // utility function for static casting to same unit with different underlying type
        template<typename NewUnderlyingT>
        constexpr auto cast() const -> scaled_t<ScaleT>::template value<NewUnderlyingT>
        {
            return static_cast<value<NewUnderlyingT>>(*this);
        }

        /**
         * @brief Convert the value to another scale
         *
         * @tparam NewUnitValueT The new unit value type to convert to with same dimensions and underlying type
         * @return a new unit with same dimensions and underlying type, but with the specified scale
         */
        template<unit_value_like NewUnitValueT>
            requires(std::same_as<typename NewUnitValueT::unit_t::dimensions_t, dimensions_t>
                     && std::same_as<UnderlyingT, typename NewUnitValueT::underlying_type>)
        constexpr auto to() const -> NewUnitValueT
        {
            using new_scale_t = typename NewUnitValueT::unit_t::scale_t;
            using converter = twig::ratio_divide<ScaleT, new_scale_t>;
            using result_value_t = scaled_t<new_scale_t>::template value<UnderlyingT>;
            return result_value_t {this->val() * static_cast<UnderlyingT>(converter::num)
                                   / static_cast<UnderlyingT>(converter::den)};
        }
    };
};

template<>
struct unit<::twig::create_dimensions_t<>, twig::base_scale>
{
    using dimensions_t = ::twig::create_dimensions_t<>;
    using scale_t = twig::base_scale;
    using unit_t = unit;

    unit() = delete;  // Do not construct this type

    template<typename UnderlyingT>
    using value = UnderlyingT;

    template<typename OtherScaleT>
    using scaled_t = unit<::twig::create_dimensions_t<>, OtherScaleT>;
};
using identity_unit = unit<::twig::create_dimensions_t<>, twig::base_scale>;

template<scale_like ScaleT>
using identity_unit_t = unit<::twig::create_dimensions_t<>, ScaleT>;

template<scale_like ScaleT, typename UnderlyingT>
using identity_value_t = identity_unit_t<ScaleT>::template value<UnderlyingT>;

template<typename Tag, scale_like ScaleT, template<typename> typename... Skills>
using stronk_default_unit =
    unit<Tag, ScaleT, can_add, can_subtract, can_negate, can_order, can_equate_underlying_type_specific, Skills...>;

template<unit_like UnitT, typename UnderlyingT>
using unit_value_t = typename UnitT::template value<UnderlyingT>;

template<typename ScaleT, unit_like UnitT, typename UnderlyingT>
using unit_scaled_value_t = typename UnitT::template scaled_t<ScaleT>::template value<UnderlyingT>;

template<unit_like UnitT, typename ScaleT>
using unit_scaled_or_base_t =
    std::conditional_t<std::is_same_v<ScaleT, twig::base_scale>, UnitT, typename UnitT::template scaled_t<ScaleT>>;

/**
 * @brief Lookup which specific type is the type for the given Dimensions.
 */
template<::twig::dimensions_like DimensionsT>
struct unit_lookup
{
    template<typename ScaleT>
    using unit_t = stronk_default_unit<DimensionsT, ScaleT>;
};

template<>
struct unit_lookup<::twig::create_dimensions_t<>>
{
    template<typename ScaleT>
    using unit_t = identity_unit::scaled_t<ScaleT>;
};

template<::twig::dimensions_like DimensionsT>
    requires(DimensionsT::is_pure())
struct unit_lookup<DimensionsT>
{
    template<typename ScaleT>
    using unit_t = unit_scaled_or_base_t<typename DimensionsT::first_t::unit_t, ScaleT>;
};

// ==================
// Multiply
// ==================

template<unit_like A, unit_like B>
using multiplied_dimensions_t = typename A::dimensions_t::template multiply_t<typename B::dimensions_t>;

template<unit_like A, unit_like B>
using multiplied_unit_t = typename unit_lookup<multiplied_dimensions_t<A, B>>::template unit_t<
    twig::ratio_multiply<typename A::scale_t, typename B::scale_t>>;

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

    using resulting_unit = multiplied_unit_t<typename A::unit_t, typename B::unit_t>;
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
using divided_unit_t = typename unit_lookup<divided_dimensions_t<A, B>>::template unit_t<
    twig::ratio_divide<typename A::scale_t, typename B::scale_t>>;

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

    using resulting_unit = divided_unit_t<typename A::unit_t, typename B::unit_t>;
    using underlying_t = decltype(res);

    using value_t = typename resulting_unit::template value<underlying_t>;
    return value_t {res};
}

template<typename T, unit_value_like B>
    requires(!unit_value_like<T>)
constexpr auto operator/(const T& a, const B& b) noexcept
{
    auto res = a / b.template unwrap<B>();
    using resulting_unit = divided_unit_t<identity_unit, typename B::unit_t>;
    using underlying_t = decltype(res);

    using value_t = resulting_unit::template value<underlying_t>;
    return value_t {res};
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

template<typename ScaleT, unit_like UnitT, typename UnderlyingT>
constexpr auto make(UnderlyingT&& value)
{
    static_assert(UnitT::dimensions_t::size() == 1,
                  "this function semantically does not seem right with multiple units");
    static_assert(UnitT::dimensions_t::first_t::rank == 1, "this function is ambiguous for none rank 1");

    return unit_scaled_value_t<typename ScaleT::type, UnitT, UnderlyingT> {std::forward<UnderlyingT>(value)};
}

}  // namespace twig
