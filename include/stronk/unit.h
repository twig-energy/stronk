#pragma once
#include <utility>

#include <stronk/stronk.h>
#include <stronk/utilities/type_list.h>

namespace twig
{

struct is_unit_tag
{};
struct is_identity_unit_tag
{};

template<typename T>
concept unit_list_like = std::same_as<typename T::has_unit_lists, std::true_type>;

template<typename T>
concept identity_unit_like = unit_list_like<T> && std::same_as<typename T::unit_like_tag, is_identity_unit_tag>;

template<typename T>
concept unit_like = unit_list_like<T> && std::same_as<typename T::unit_like_tag, is_unit_tag>;

template<typename T>
concept pure_unit_like = unit_like<T> && !std::same_as<typename T::pure_t, void>;

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
    using has_unit_lists = std::true_type;
    using multiplied_part = MultipliedUnitsTypeList;
    using divided_part = DividedUnitsTypeList;

    static constexpr bool is_unitless = multiplied_part::empty() && divided_part::empty();
    static constexpr bool is_single_unit = multiplied_part::size() == 1 && divided_part::empty();
    using pure_t = std::conditional_t<is_single_unit, typename multiplied_part::first_t, void>;
    using unit_like_tag = std::conditional_t<is_unitless, is_identity_unit_tag, is_unit_tag>;
};

using IdentityUnitTypeList = UnitTypeLists<TypeList<>, TypeList<>>;

// Stronk Skill
template<typename StronkT>
struct identity_unit : IdentityUnitTypeList
{
    using unit_description_t = IdentityUnitTypeList;
};

// Stronk Skill
template<typename StronkT>
    requires(!is_none_unit_behaving<StronkT>)
struct unit : UnitTypeLists<TypeList<StronkT>, TypeList<>>
{
    using unit_description_t = UnitTypeLists<TypeList<StronkT>, TypeList<>>;

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

template<typename T>
struct default_identity_unit
    : stronk<default_identity_unit<T>,
             T,
             identity_unit,
             can_order,
             can_add,
             can_subtract,
             can_negate,
             default_can_equate_builder<T>::template skill,
             can_forward_constructor_args>
{
    using stronk<default_identity_unit,
                 T,
                 identity_unit,
                 can_order,
                 can_add,
                 can_subtract,
                 can_negate,
                 default_can_equate_builder<T>::template skill,
                 can_forward_constructor_args>::stronk;
};

template<typename UnitTypeListsT>
struct unit_type_list_skill_builder
{
    template<typename StronkT>
    struct skill : UnitTypeListsT
    {
        using unit_description_t = UnitTypeListsT;
    };
};

// You can specialize this type if you want to add other skills to your dynamic types.
template<typename T, typename UnitTypeListsT>
struct NewUnitType
    : stronk<NewUnitType<T, UnitTypeListsT>,
             T,
             can_order,
             can_add,
             can_subtract,
             can_negate,
             default_can_equate_builder<T>::template skill,
             can_forward_constructor_args,
             unit_type_list_skill_builder<UnitTypeListsT>::template skill>
{
    using stronk<NewUnitType<T, UnitTypeListsT>,
                 T,
                 can_order,
                 can_add,
                 can_subtract,
                 can_negate,
                 default_can_equate_builder<T>::template skill,
                 can_forward_constructor_args,
                 unit_type_list_skill_builder<UnitTypeListsT>::template skill>::stronk;
};

/**
 * @brief Lookup which specific type is the type for the given UnitTypeLists.
 */
template<unit_list_like UnitT, typename UnderlyingT>
struct unit_lookup
{
    static_assert(!stronk_like<UnitT>,
                  "use my_type::unit_description_t (the unit type lists) instead of the full stronk type");
    using underlying_type = UnderlyingT;
    using res_type =
        NewUnitType<underlying_type, UnitTypeLists<typename UnitT::multiplied_part, typename UnitT::divided_part>>;
};

template<pure_unit_like UnitT, typename UnderlyingT>
struct unit_lookup<UnitT, UnderlyingT>
{
    static_assert(!stronk_like<UnitT>,
                  "use my_type::unit_description_t (the unit type lists) instead of the full stronk type");
    //  In this case we have a single unit left and we can determine the underlying type and type in general from that
    using res_type = typename UnitT::pure_t;
    using underlying_type = typename res_type::underlying_type;
    // static_assert(std::is_same_v<underlying_type, UnderlyingT>,
    //               "Something went wrong with the type of your underlying result");
};

template<identity_unit_like UnitT, typename UnderlyingT>
struct unit_lookup<UnitT, UnderlyingT>
{
    using underlying_type = UnderlyingT;
    using res_type = default_identity_unit<underlying_type>;
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
    // This is the result:
    using unit_description_t = UnitTypeLists<new_multiplied_part, new_divided_part>;
};

// When you want the skill of multiplying two units
template<unit_like A, unit_like B>
struct multiplied_unit
{
    template<typename StronkT>
    struct skill : unit_lists_of_multiplying<A, B>::unit_description_t  // use this as a stronk skill.
    {
        using unit_description_t = typename unit_lists_of_multiplying<A, B>::unit_description_t;
    };
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

    using type_lists = typename unit_lists_of_multiplying<A, B>::unit_description_t;
    using resulting_unit = typename unit_lookup<type_lists, decltype(res)>::res_type;
    // check that the type is setup correctly. It might have been specialized.
    static_assert(std::is_same_v<type_lists, typename resulting_unit::unit_description_t>,
                  "Seems to be a mismatch in units for your specialized type. Maybe you added the wrong skill. "
                  "See multiplied_unit<A,B>::skill");
    if constexpr (resulting_unit::is_unitless) {
        return res;
    } else if constexpr (resulting_unit::is_single_unit) {
        using pure_t = typename resulting_unit::pure_t;  // unwrap out into original type.
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
struct unit_lists_of_dividing
{
    using all_multiplied = typename A::multiplied_part::template concat_sorted_t<typename B::divided_part>;
    using all_divided = typename A::divided_part::template concat_sorted_t<typename B::multiplied_part>;
    using new_multiplied_part = typename all_multiplied::template subtract_t<all_divided>;
    using new_divided_part = typename all_divided::template subtract_t<all_multiplied>;
    // This is the result:
    using unit_description_t = UnitTypeLists<new_multiplied_part, new_divided_part>;
};

template<unit_like A, unit_like B>
struct divided_unit
{
    template<typename StronkT>
    struct skill : unit_lists_of_dividing<A, B>::unit_description_t  // use this as a stronk skill.
    {
        using unit_description_t = typename unit_lists_of_dividing<A, B>::unit_description_t;
    };
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

    using type_lists = typename unit_lists_of_dividing<A, B>::unit_description_t;
    using resulting_unit = typename unit_lookup<type_lists, decltype(res)>::res_type;
    // check that the type is setup correctly. It might have been specialized.
    static_assert(std::is_same_v<type_lists, typename resulting_unit::unit_description_t>,
                  "Seems to be a mismatch in units for your specialized type. Maybe you added the wrong skill. "
                  "See multiplied_unit<A,B>::skill");
    if constexpr (resulting_unit::is_unitless) {
        return res;
    } else if constexpr (resulting_unit::is_single_unit) {
        using pure_t = typename resulting_unit::pure_t;
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
