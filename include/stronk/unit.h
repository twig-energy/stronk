#pragma once
#include <utility>

#include <stronk/stronk.h>
#include <stronk/utilities/type_list.h>

namespace twig
{

template<typename T>
concept unit_like = std::same_as<typename T::is_unit_type, std::true_type>;

template<typename T>
concept identity_unit_like = std::same_as<typename T::is_identity_unit, std::true_type>;

template<typename T>
concept ratio_like = requires(T v)
{
    T::num;
    T::den;
};

template<typename T>
concept ratio_with_base_unit_like = ratio_like<T> && requires(T v)
{
    typename T::base_unit_t;
};

template<typename MultipliedUnitsTypeList, typename DividedUnitsTypeList>
struct UnitTypeLists
{
    using multiplied_part = MultipliedUnitsTypeList;
    using divided_part = DividedUnitsTypeList;

    using pure_t = typename multiplied_part::first_t;

    using is_unit_type = std::true_type;
    static constexpr bool is_unitless = multiplied_part::empty() && divided_part::empty();
    static constexpr bool is_single_unit = multiplied_part::size() == 1 && divided_part::empty();
};

template<>
struct UnitTypeLists<TypeList<>, TypeList<>>
{
    using is_identity_unit = std::true_type;
    static constexpr bool is_unitless = true;
    static constexpr bool is_single_unit = false;
};
using IdentityUnitTypeList = UnitTypeLists<TypeList<>, TypeList<>>;

// You can specialize this type if you want to add other skills to your dynamic
// types.
template<typename T, typename UnitTypeListsT>
struct NewUnitType
    : stronk<NewUnitType<T, UnitTypeListsT>, T, can_order, can_add, can_subtract, can_negate>
    , UnitTypeListsT
    , default_can_equate<T, NewUnitType<T, UnitTypeListsT>>
{
    using stronk_base = stronk<NewUnitType<T, UnitTypeListsT>, T, can_order, can_add, can_subtract, can_negate>;
    using stronk_base::stronk_base;
};

template<typename StronkT>
struct identity_unit : IdentityUnitTypeList  // use this as a stronk skill.
{
};

template<typename StronkT>
    requires(!is_none_unit_behaving<StronkT>)
struct unit : UnitTypeLists<TypeList<StronkT>,
                            TypeList<>>  // use this as a stronk skill.
{
    /**
     * @brief unwrap but in the ratio specified. Like the regular unwrap,
     * unwrap_as requires the ratio's base_unit_t==StronkT.
     *
     * @tparam RatioT a std::ratio like type with a baseunit_t (to compare to
     * StronkT)
     */
    template<ratio_with_base_unit_like RatioT>
        requires(std::is_same_v<StronkT, typename RatioT::base_unit_t>)
    [[nodiscard]] auto unwrap_as() const noexcept;
};

// You can specialize this struct if you want another type
template<stronk_like T1, stronk_like T2>
struct underlying_type_of_multiplying
{
    using type = decltype(std::declval<typename T1::underlying_type>() * std::declval<typename T2::underlying_type>());
};

// You can specialize this struct if you want another type
template<stronk_like T1, stronk_like T2>
struct underlying_type_of_dividing
{
    using type = decltype(std::declval<typename T1::underlying_type>() / std::declval<typename T2::underlying_type>());
};

// ==================
// Multiply
// ==================

template<unit_like A, unit_like B>
struct unit_lists_of_multiplying
{
    using all_multiplied = typename A::multiplied_part::template concat_sorted_t<typename B::multiplied_part>;
    using all_divided = typename A::divided_part::template concat_sorted_t<typename B::divided_part>;
    using new_multiplied_part = typename all_multiplied::template subtract_t<all_divided>;
    using new_divided_part = typename all_divided::template subtract_t<all_multiplied>;
    using unit_description_t = UnitTypeLists<new_multiplied_part, new_divided_part>;
};

template<unit_like A, unit_like B>
constexpr auto operator*(const A& a, const B& b) noexcept
{
    using underlying_type = typename underlying_type_of_multiplying<A, B>::type;
    auto res = static_cast<underlying_type>(a.template unwrap<A>() * b.template unwrap<B>());

    using unit_description_t = typename unit_lists_of_multiplying<A, B>::unit_description_t;
    if constexpr (unit_description_t::is_unitless) {
        return res;
    } else if constexpr (unit_description_t::is_single_unit) {
        using pure_t = typename unit_description_t::pure_t;  // unwrap out into original type.
        return pure_t {static_cast<typename pure_t::underlying_type>(res)};
    } else {
        return NewUnitType<underlying_type, unit_description_t> {res};
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
struct unit_lists_of_dividing
{
    using all_multiplied = typename A::multiplied_part::template concat_sorted_t<typename B::divided_part>;
    using all_divided = typename A::divided_part::template concat_sorted_t<typename B::multiplied_part>;
    using new_multiplied_part = typename all_multiplied::template subtract_t<all_divided>;
    using new_divided_part = typename all_divided::template subtract_t<all_multiplied>;
    using unit_description_t = UnitTypeLists<new_multiplied_part, new_divided_part>;
};

template<unit_like A, unit_like B>
constexpr auto operator/(const A& a, const B& b) noexcept
{
    using underlying_type = typename underlying_type_of_dividing<A, B>::type;
    auto res = static_cast<underlying_type>(a.template unwrap<A>() / b.template unwrap<B>());

    using unit_description_t = typename unit_lists_of_dividing<A, B>::unit_description_t;
    if constexpr (unit_description_t::is_unitless) {
        return res;
    } else if constexpr (unit_description_t::is_single_unit) {
        using pure_t = typename unit_description_t::pure_t;
        return pure_t {static_cast<typename pure_t::underlying_type>(res)};
    } else {
        return NewUnitType<underlying_type, unit_description_t> {res};
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
    return NewUnitType<typename B::underlying_type,
                       UnitTypeLists<typename B::divided_part, typename B::multiplied_part>> {a.template unwrap<T>()
                                                                                              / b.template unwrap<B>()};
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
    return NewUnitType<typename B::underlying_type,
                       UnitTypeLists<typename B::divided_part, typename B::multiplied_part>> {a
                                                                                              / b.template unwrap<B>()};
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

template<ratio_like RatioT, typename T, typename Arg>
    requires(stronk_like<T>&& unit_like<T>&& std::convertible_to<Arg, typename T::underlying_type>)
auto make(Arg&& args) -> T
{
    // RatioT from <ratio> (includes std::kilo, std::centi etc)
    using underlying_type = typename T::underlying_type;
    return T {std::forward<Arg>(args)} * static_cast<underlying_type>(RatioT::num)
        / static_cast<underlying_type>(RatioT::den);
}

template<ratio_with_base_unit_like RatioT, typename Arg>
    requires(std::convertible_to<Arg, typename RatioT::base_unit_t::underlying_type>)
auto make(Arg&& args)
{
    return make<RatioT, typename RatioT::base_unit_t>(std::forward<Arg>(args));
}

template<typename StronkT>
    requires(!is_none_unit_behaving<StronkT>)
template<ratio_with_base_unit_like RatioT>
    requires(std::is_same_v<StronkT, typename RatioT::base_unit_t>)
auto unit<StronkT>::unwrap_as() const noexcept
{
    using underlying_type = typename StronkT::underlying_type;
    return static_cast<const StronkT&>(*this).template unwrap<StronkT>() * static_cast<underlying_type>(RatioT::den)
        / static_cast<underlying_type>(RatioT::num);
}

}  // namespace twig
