#pragma once
#include <concepts>
#include <ratio>
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
    typename T::template value<std::ratio<1>, void>;
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

    template<typename ScaleT, typename UnderlyingT>
    struct value : ::twig::stronk<value<ScaleT, UnderlyingT>, UnderlyingT, SkillTs...>
    {
        using unit_t = unit;
        using base_t = ::twig::stronk<value<ScaleT, UnderlyingT>, UnderlyingT, SkillTs...>;
        using base_t::base_t;
        using scale_t = ScaleT;

        // can convert to same unit with different underlying type, or different scale
        template<typename NewScaleT, typename NewUnderlyingT>
            requires(!std::same_as<NewUnderlyingT, UnderlyingT> || !std::same_as<ScaleT, NewScaleT>)
        constexpr explicit operator value<NewScaleT, NewUnderlyingT>() const
        {
            using converter = std::ratio_divide<ScaleT, NewScaleT>;
            return value<NewScaleT, NewUnderlyingT> {static_cast<NewUnderlyingT>(this->val()) * converter::num
                                                     / converter::den};
        }
    };
};

template<>
struct unit<::twig::create_dimensions_t<>>
{
    using dimensions_t = ::twig::create_dimensions_t<>;

    unit() = delete;  // Do not construct this type

    template<typename ScaleT, typename UnderlyingT>
        requires(std::same_as<ScaleT, std::ratio<1>>)  // identity does not expose scale so we require it to be 1
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
using unit_value_t = typename UnitT::template value<std::ratio<1>, UnderlyingT>;

template<typename ScaleT, unit_like UnitT, typename UnderlyingT>
using unit_scaled_value_t = typename UnitT::template value<ScaleT, UnderlyingT>;

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
    using scale_t = std::ratio_multiply<typename A::scale_t, typename B::scale_t>;

    using value_t = typename resulting_unit::template value<scale_t, underlying_t>;
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
    using scale_t = std::ratio_divide<typename A::scale_t, typename B::scale_t>;

    using value_t = typename resulting_unit::template value<scale_t, underlying_t>;
    return value_t {res};
}

template<typename T, unit_value_like B>
    requires(!unit_value_like<T>)
constexpr auto operator/(const T& a, const B& b) noexcept
{
    auto res = a / b.template unwrap<B>();
    using dimensions_t = typename B::unit_t::dimensions_t::negate_t;
    using resulting_unit = typename unit_lookup<dimensions_t>::unit_t;
    using underlying_t = decltype(res);
    using scale_t = std::ratio_divide<std::ratio<1>, typename B::scale_t>;

    using value_t = resulting_unit::template value<scale_t, underlying_t>;
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

    constexpr auto abs = [](auto val) constexpr { return val < 0 ? -val : val; };
    constexpr auto constexpr_pow = [](auto val, auto rank) consteval
    {
        for (auto r = 1; r < rank; ++r) {
            val *= val;
        }
        return val;
    };
    constexpr auto rank = abs(UnitT::dimensions_t::first_t::rank);
    using raw_ratio = std::ratio<constexpr_pow(ScaleT::num, rank), constexpr_pow(ScaleT::den, rank)>;

    return unit_scaled_value_t<std::ratio<raw_ratio::num, raw_ratio::den>, UnitT, UnderlyingT> {
        std::forward<UnderlyingT>(value)};
}

}  // namespace twig::unit_v2
