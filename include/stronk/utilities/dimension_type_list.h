#pragma once
#include <concepts>
#include <cstddef>

#include <boost/type_index/ctti_type_index.hpp>
#include <stronk/utilities/constexpr_helpers.h>

namespace twig::experiments
{

template<typename T>
concept is_dimension = requires(T) {
    typename T::unit_t;
    T::rank;
};

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

    using negate_t = Dimension<UnitT, -RankV>;
};

template<is_dimension... Ts>
struct Dimensions;

using EmptyDimensions = Dimensions<>;

template<is_dimension... As>
struct DimensionsMerge;

// Empty Case implementation
template<>
struct DimensionsMerge<>
{
    template<typename AccumulatedDimensionListT, typename... Bs>
    [[nodiscard]] constexpr static auto merge()
    {
        using res_t = typename AccumulatedDimensionListT::template concat_t<Bs...>;
        return res_t();
    }

    template<typename... OtherTs>
    using merge_t = decltype(merge<EmptyDimensions, OtherTs...>());
};

template<is_dimension A, is_dimension... As>
struct DimensionsMerge<A, As...>
{
    template<typename AccumulatedDimensionListT>
    [[nodiscard]] constexpr static auto merge()
    {
        using res_t = typename AccumulatedDimensionListT::template concat_t<A, As...>;
        return res_t();
    }

    template<typename AccumulatedDimensionListT, is_dimension B, is_dimension... Bs>
    [[nodiscard]] constexpr static auto merge()
    {
        static_assert(A::rank != 0, "Cannot merge dimensions with rank 0");
        static_assert(B::rank != 0, "Cannot merge dimensions with rank 0");

        using boost::typeindex::ctti_type_index;
        constexpr auto t_equals_o = std::is_same_v<typename A::unit_t, typename B::unit_t>;
        constexpr auto t_before_o =
            ctti_type_index::type_id<typename A::unit_t>().before(ctti_type_index::type_id<typename B::unit_t>());

        if constexpr (t_equals_o) {
            using new_dim = typename A::template add_t<B>;
            if constexpr (new_dim::rank == 0) {
                // The combination of the two dimensions results in a dimension with rank 0, so we can remove it
                return DimensionsMerge<As...>::template merge<AccumulatedDimensionListT, Bs...>();
            } else {
                using new_merged_list = AccumulatedDimensionListT::template push_back_t<new_dim>;
                return DimensionsMerge<As...>::template merge<new_merged_list, Bs...>();
            }
        } else if constexpr (t_before_o) {
            using new_merged_list = AccumulatedDimensionListT::template push_back_t<A>;
            return DimensionsMerge<As...>::template merge<new_merged_list, B, Bs...>();
        } else {
            using new_merged_list = AccumulatedDimensionListT::template push_back_t<B>;
            return DimensionsMerge<A, As...>::template merge<new_merged_list, Bs...>();
        }
    }

    template<is_dimension... OtherTs>
    using merge_t = decltype(merge<EmptyDimensions, OtherTs...>());
};

template<is_dimension... Ts>
struct Dimensions
{
    template<is_dimension NewDimT>
    using push_back_t = Dimensions<Ts..., NewDimT>;

    template<is_dimension... NewDimTs>
    using concat_t = Dimensions<Ts..., NewDimTs...>;

    using negate_t = Dimensions<typename Ts::negate_t...>;

    template<is_dimension... Bs>
    static auto multiply(Dimensions<Bs...>)
    {
        return DimensionsMerge<Ts...>::template merge<Dimensions<>, Bs...>();
    }
    template<typename OtherDimensionsT>
    using multiply_t = decltype(multiply(OtherDimensionsT {}));

    template<is_dimension... Bs>
    static auto divide(Dimensions<Bs...>)
    {
        using negated_b = Dimensions<Bs...>::negate_t;
        return multiply(negated_b {});
    }
    template<typename OtherDimensionsT>
    using divide_t = decltype(divide(OtherDimensionsT {}));
};

namespace
{

template<typename... ExistingDimTs>
auto create(Dimensions<ExistingDimTs...> dim)
{
    return dim;
}

template<typename... ExistingDimTs, is_dimension Dim, is_dimension... DimTs>
auto create(Dimensions<ExistingDimTs...> dims, Dim, DimTs... rest)
{
    return create(dims.multiply(Dimensions<Dim> {}), rest...);
}

}  // namespace

// Ensures order and uniqueness of dimensions
template<is_dimension... DimTs>
auto create_dimensions(DimTs... dims)
{
    return create(EmptyDimensions {}, dims...);
}

template<is_dimension... DimTs>
using create_dimensions_t = decltype(create_dimensions(DimTs {}...));

}  // namespace twig::experiments
