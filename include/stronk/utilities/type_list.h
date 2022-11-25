#pragma once
#include <type_traits>

#include <boost/type_index/ctti_type_index.hpp>
#include <stronk/utilities/constexpr_helpers.h>

namespace twig
{

namespace details
{

/**
 *
 * TypeList is a simple CONS based structure for holding and manipulating collections of types.
 *
 */

template<typename... Ts>
struct BaseTypeList
{
};

// ============
// Cons
// ============

template<typename T, typename... Ts>
struct Cons;

template<typename T, typename... Ts>
struct Cons<T, BaseTypeList<Ts...>>
{
    using type = BaseTypeList<T, Ts...>;
};

template<typename... Ts>
struct Concat;

template<typename... Ts>
struct Concat<BaseTypeList<Ts...>>
{
    template<typename... OtherTs>
    struct With
    {
        using type = BaseTypeList<Ts..., OtherTs...>;
    };
};

// ============
// Remove First
// ============

template<typename R, typename... Ts>
struct EraseFirstOfT;

template<typename R>
struct EraseFirstOfT<R>
{
    using type = BaseTypeList<>;
};

template<typename R, typename T, typename... Ts>
struct EraseFirstOfT<R, T, Ts...>
{
    using type = typename std::conditional_t<std::is_same_v<R, T>,
                                             BaseTypeList<Ts...>,
                                             typename Cons<T, typename EraseFirstOfT<R, Ts...>::type>::type>;
};

template<typename R, typename... Ts>
struct EraseAllOfT;

template<typename R>
struct EraseAllOfT<R>
{
    using type = BaseTypeList<>;
};

template<typename R, typename T, typename... Ts>
struct EraseAllOfT<R, T, Ts...>
{
    using type = typename std::conditional_t<std::is_same_v<R, T>,
                                             typename EraseAllOfT<R, Ts...>::type,
                                             typename Cons<T, typename EraseAllOfT<R, Ts...>::type>::type>;
};

/**
 * @brief Can merge to already sorted type lists.
 *
 * @tparam IndexV current index to compare in Ts...
 * @tparam Ts
 */
template<typename A, typename... As>
struct TypeListMerge
{
    template<typename MergedBaseTypeListT, typename B, typename... Bs>
    [[nodiscard]] constexpr static auto merge(MergedBaseTypeListT)
    {
        using boost::typeindex::ctti_type_index;
        constexpr auto t_before_o = ctti_type_index::type_id<A>().before(ctti_type_index::type_id<B>());

        if constexpr (t_before_o) {
            if constexpr (sizeof...(As) == 0) {
                using res_t = typename Concat<MergedBaseTypeListT>::template With<A, B, Bs...>::type;
                return res_t();
            } else {
                using res_t = typename Concat<MergedBaseTypeListT>::template With<A>::type;
                return TypeListMerge<As...>::template merge<res_t, B, Bs...>(res_t());
            }
        } else {
            if constexpr (sizeof...(Bs) == 0) {
                using res_t = typename Concat<MergedBaseTypeListT>::template With<B, A, As...>::type;
                return res_t();
            } else {
                using res_t = typename Concat<MergedBaseTypeListT>::template With<B>::type;
                return TypeListMerge<A, As...>::template merge<res_t, Bs...>(res_t());
            }
        }
    }

    template<typename... OtherTs>
    using merge_t = decltype(merge<BaseTypeList<>, OtherTs...>(BaseTypeList<>()));
};

}  // namespace details

template<typename... Ts>
struct TypeList
{
    [[nodiscard]] constexpr static auto size() -> std::size_t { return sizeof...(Ts); }

    [[nodiscard]] constexpr static auto empty() -> bool { return size() == 0; }

    using first_t = stronk_details::variadic::first_type_of_t<Ts..., void>;  // void if empty

    template<typename T>
    [[nodiscard]] constexpr static auto push_front() -> TypeList<T, Ts...>
    {
        return TypeList<T, Ts...> {};
    }

    template<typename T>
    using push_front_t = decltype(push_front<T>());

    template<typename T>
    [[nodiscard]] constexpr static auto push_back() -> TypeList<Ts..., T>
    {
        return TypeList<Ts..., T> {};
    }

    template<typename T>
    using push_back_t = decltype(push_back<T>());

    template<typename T>
    [[nodiscard]] constexpr static auto contains() -> bool
    {
        return (... || std::is_same_v<T, Ts>);
    }

    template<typename R>
    [[nodiscard]] constexpr static auto erase_first_of()
    {
        using type = typename details::EraseFirstOfT<R, Ts...>::type;
        return from_impl_typelist(type());
    }

    template<typename T>
    using erase_first_of_t = decltype(erase_first_of<T>());

    template<typename R>
    [[nodiscard]] constexpr static auto erase_all_of()
    {
        using type = typename details::EraseAllOfT<R, Ts...>::type;
        return from_impl_typelist(type());
    }

    template<typename T>
    using erase_all_of_t = decltype(erase_all_of<T>());

    //
    // Functions between TypeLists
    //

    template<typename... OtherTs>
    [[nodiscard]] constexpr static auto concat(TypeList<OtherTs...>)
    {
        return TypeList<Ts..., OtherTs...>();
    }

    template<typename TypeListT>
    using concat_t = decltype(concat(TypeListT()));

    // Concatenates two sorted typelists into a new sorted type list
    template<typename... OtherTs>
    [[nodiscard]] constexpr static auto concat_sorted(TypeList<OtherTs...>)
    {
        if constexpr (sizeof...(OtherTs) == 0) {
            return TypeList<Ts...>();
        } else if constexpr (sizeof...(Ts) == 0) {
            return TypeList<OtherTs...>();
        } else {
            using res_type = typename details::TypeListMerge<Ts...>::template merge_t<OtherTs...>;
            return from_impl_typelist(res_type());
        }
    }

    template<typename TypeListT>
    using concat_sorted_t = decltype(concat_sorted(TypeListT()));

    [[nodiscard]] constexpr static auto subtract(TypeList<>) -> TypeList<Ts...> { return TypeList<Ts...>(); }

    template<typename OtherT, typename... OtherTs>
    [[nodiscard]] constexpr static auto subtract(TypeList<OtherT, OtherTs...>)
    {
        using type = decltype(erase_first_of_t<OtherT>::subtract(TypeList<OtherTs...>()));
        return type();
    }

    template<typename TypeListT>
    using subtract_t = decltype(subtract(TypeListT()));

  private:
    template<typename... OtherTs>
    [[nodiscard]] constexpr static auto from_impl_typelist(details::BaseTypeList<OtherTs...>)
    {
        return TypeList<OtherTs...>();
    }
};

}  // namespace twig
