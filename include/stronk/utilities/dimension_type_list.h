#pragma once
#include <concepts>
#include <cstddef>

#include <boost/type_index/ctti_type_index.hpp>
#include <stronk/utilities/constexpr_helpers.h>

namespace twig::experiments
{

template<typename UnitT, int16_t RankV>
struct Dimension
{
    static constexpr auto rank = RankV;
    using unit_t = UnitT;

    template<typename OtherDimT>
        requires std::same_as<UnitT, typename OtherDimT::unit_t>
    using add_t = Dimension<UnitT, RankV + OtherDimT::rank>;

    template<typename OtherDimT>
        requires std::same_as<UnitT, typename OtherDimT::unit_t>
    using subtract_t = Dimension<UnitT, RankV - OtherDimT::rank>;
};

template<typename... Ts>
struct DimensionList
{
    template<typename NewDimT>
    using append_t = DimensionList<Ts..., NewDimT>;

    template<typename... NewDimTs>
    using concat_t = DimensionList<Ts..., NewDimTs...>;
};

template<bool AddOrSubtractV, typename A, typename... As>
struct DimensionListMerge
{
    template<typename MergedDimensionTypeListT, typename B, typename... Bs>
    [[nodiscard]] constexpr static auto merge(MergedDimensionTypeListT)
    {
        static_assert(A::rank != 0, "Cannot merge dimensions with rank 0");
        static_assert(B::rank != 0, "Cannot merge dimensions with rank 0");

        using boost::typeindex::ctti_type_index;
        constexpr auto t_equals_o = std::is_same_v<typename A::unit_t, typename B::unit_t>;
        constexpr auto t_before_o =
            ctti_type_index::type_id<typename A::unit_t>().before(ctti_type_index::type_id<typename B::unit_t>());

        if constexpr (t_equals_o) {
            using new_dim = typename std::
                conditional_t<AddOrSubtractV, typename A::template add_t<B>, typename A::template subtract_t<B>>;
            if constexpr (new_dim::rank == 0) {
                if constexpr (sizeof...(As) == 0) {
                    using res_t = typename MergedDimensionTypeListT::template concat_t<Bs...>;
                    return res_t();
                } else if constexpr (sizeof...(Bs) == 0) {
                    using res_t = typename MergedDimensionTypeListT::template concat_t<As...>;
                    return res_t();
                } else {
                    return DimensionListMerge<AddOrSubtractV, As...>::template merge<MergedDimensionTypeListT, Bs...>(
                        MergedDimensionTypeListT());
                }
            } else {
                using new_merged_list = MergedDimensionTypeListT::template append_t<new_dim>;
                if constexpr (sizeof...(As) == 0) {
                    using res_t = typename new_merged_list::template concat_t<Bs...>;
                    return res_t();
                } else if constexpr (sizeof...(Bs) == 0) {
                    using res_t = typename new_merged_list::template concat_t<As...>;
                    return res_t();
                } else {
                    return DimensionListMerge<AddOrSubtractV, As...>::template merge<new_merged_list, Bs...>(
                        new_merged_list {});
                }
            }
        } else if constexpr (t_before_o) {
            if constexpr (sizeof...(As) == 0) {
                using res_t = typename MergedDimensionTypeListT::template concat_t<A, B, Bs...>;
                return res_t();
            } else {
                using res_t = MergedDimensionTypeListT::template append_t<A>;
                return DimensionListMerge<AddOrSubtractV, As...>::template merge<res_t, B, Bs...>(res_t());
            }
        } else {
            if constexpr (sizeof...(Bs) == 0) {
                using res_t = typename MergedDimensionTypeListT::template concat_t<B, A, As...>;
                return res_t();
            } else {
                using res_t = MergedDimensionTypeListT::template append_t<B>;
                return DimensionListMerge<AddOrSubtractV, A, As...>::template merge<res_t, Bs...>(res_t());
            }
        }
    }

    template<typename... OtherTs>
    using merge_t = decltype(merge<DimensionList<>, OtherTs...>(DimensionList<>()));
};

template<typename... As, typename... Bs>
auto add_dimensions(DimensionList<As...>, DimensionList<Bs...>)
{
    return DimensionListMerge<true, As...>::template merge<DimensionList<>, Bs...>(DimensionList<>());
}

template<typename ListA, typename ListB>
using add_dimensions_t = decltype(add_dimensions(ListA {}, ListB {}));

template<typename... As, typename... Bs>
auto subtract_dimensions(DimensionList<As...>, DimensionList<Bs...>)
{
    return DimensionListMerge<false, As...>::template merge<DimensionList<>, Bs...>(DimensionList<>());
}

template<typename ListA, typename ListB>
using subtract_dimensions_t = decltype(subtract_dimensions(ListA {}, ListB {}));

}  // namespace twig::experiments
