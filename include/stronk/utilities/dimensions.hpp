#pragma once
#include <concepts>
#include <cstddef>
#include <cstdint>

#include <boost/type_index/ctti_type_index.hpp>
#include <stronk/utilities/constexpr_helpers.hpp>

namespace twig
{

template<typename T>
concept dimension_like = requires(T) {
    typename T::unit_t;
    T::rank;
};

template<typename UnitT, int16_t RankV>
struct dimension
{
    constexpr static auto rank = RankV;
    using unit_t = UnitT;

    // Since dimension ranks are "exponents" we have:  unit^6 * unit^3 = unit^(6+3) = unit^9
    template<typename OtherDimT>
        requires std::same_as<UnitT, typename OtherDimT::unit_t>
    using multiply_t = dimension<UnitT, RankV + OtherDimT::rank>;

    // Since dimension ranks are "exponents" we have:  unit^6 / unit^3 = unit^(6-3) = unit^3
    template<typename OtherDimT>
        requires std::same_as<UnitT, typename OtherDimT::unit_t>
    using divide_t = dimension<UnitT, RankV - OtherDimT::rank>;

    using negate_t = dimension<UnitT, -RankV>;

    // Since dimension ranks are "exponents" we have:  sqrt(unit^6) = (unit^6)^(1/2) = unit^(6/2)
    template<auto RootV>
        requires(RootV != 0) && (RankV % RootV == 0)
    using root_t = dimension<UnitT, RankV / RootV>;

    // Since dimension ranks are "exponents" we have:  pow(unit^6, 3) = (unit^6)^3 = unit^(6*3)
    template<auto PowerV>
    using power_t = dimension<UnitT, RankV * PowerV>;
};

namespace details
{
template<dimension_like... Ts>
struct dimensions;

using empty_dimensions = dimensions<>;

template<dimension_like... As>
struct dimensions_merge;

// Empty Case implementation
template<>
struct dimensions_merge<>
{
    template<typename AccumulatedDimensionListT, typename... Bs>
    [[nodiscard]]
    constexpr static auto merge()
    {
        using res_t = typename AccumulatedDimensionListT::template concat_t<Bs...>;
        return res_t();
    }

    template<typename... OtherTs>
    using merge_t = decltype(merge<empty_dimensions, OtherTs...>());
};

template<dimension_like A, dimension_like... As>
struct dimensions_merge<A, As...>
{
    template<typename AccumulatedDimensionListT>
    [[nodiscard]]
    constexpr static auto merge()
    {
        using res_t = typename AccumulatedDimensionListT::template concat_t<A, As...>;
        return res_t();
    }

    template<typename AccumulatedDimensionListT, dimension_like B, dimension_like... Bs>
    [[nodiscard]]
    constexpr static auto merge()
    {
        static_assert(A::rank != 0, "Cannot merge dimensions with rank 0");
        static_assert(B::rank != 0, "Cannot merge dimensions with rank 0");

        using boost::typeindex::ctti_type_index;
        constexpr auto a_equals_b = std::is_same_v<typename A::unit_t, typename B::unit_t>;
        constexpr auto a_before_b =
            ctti_type_index::type_id<typename A::unit_t>().before(ctti_type_index::type_id<typename B::unit_t>());

        if constexpr (a_equals_b) {
            using new_dim = typename A::template multiply_t<B>;
            if constexpr (new_dim::rank == 0) {
                // The combination of the two dimensions results in a dimension with rank 0, so we can remove it
                return dimensions_merge<As...>::template merge<AccumulatedDimensionListT, Bs...>();
            } else {
                using new_merged_list = typename AccumulatedDimensionListT::template push_back_t<new_dim>;
                return dimensions_merge<As...>::template merge<new_merged_list, Bs...>();
            }
        } else if constexpr (a_before_b) {
            using new_merged_list = typename AccumulatedDimensionListT::template push_back_t<A>;
            return dimensions_merge<As...>::template merge<new_merged_list, B, Bs...>();
        } else {
            using new_merged_list = typename AccumulatedDimensionListT::template push_back_t<B>;
            return dimensions_merge<A, As...>::template merge<new_merged_list, Bs...>();
        }
    }

    template<dimension_like... OtherTs>
    using merge_t = decltype(merge<empty_dimensions, OtherTs...>());
};

// Use create_dimensions_t to instantiate this type
template<dimension_like... Ts>
struct dimensions
{
    [[nodiscard]]
    constexpr static auto size() -> std::size_t
    {
        return sizeof...(Ts);
    }

    [[nodiscard]]
    constexpr static auto empty() -> bool
    {
        return size() == 0;
    }

    [[nodiscard]]
    constexpr static auto is_pure() -> bool
    {
        return size() == 1 && first_t::rank == 1;
    }

    using first_t = stronk_details::variadic::first_type_of_t<Ts..., dimension<void, 0>>;  // void if empty

    template<dimension_like NewDimT>
    using push_back_t = dimensions<Ts..., NewDimT>;

    template<dimension_like... NewDimTs>
    using concat_t = dimensions<Ts..., NewDimTs...>;

    using negate_t = dimensions<typename Ts::negate_t...>;

    template<dimension_like... Bs>
    static auto multiply([[maybe_unused]] dimensions<Bs...> dims)
    {
        return dimensions_merge<Ts...>::template merge<dimensions<>, Bs...>();
    }
    template<typename OtherDimensionsT>
    using multiply_t = decltype(multiply(OtherDimensionsT {}));

    template<dimension_like... Bs>
    static auto divide([[maybe_unused]] dimensions<Bs...> dims)
    {
        using negated_b = typename dimensions<Bs...>::negate_t;
        return multiply(negated_b {});
    }
    template<typename OtherDimensionsT>
    using divide_t = decltype(divide(OtherDimensionsT {}));

    template<auto RootV>
    using root_t = dimensions<typename Ts::template root_t<RootV>...>;

    template<auto PowerV>
    using power_t = dimensions<typename Ts::template power_t<PowerV>...>;
};

template<typename... ExistingDimTs>
auto create(dimensions<ExistingDimTs...> dim)
{
    return dim;
}

template<typename... ExistingDimTs, dimension_like Dim, dimension_like... DimTs>
auto create(dimensions<ExistingDimTs...> dims, [[maybe_unused]] Dim dim_for_type_deduction, DimTs... rest)
{
    // Insert each template dimension, merging as we go - this ensures order and uniqueness
    return create(dims.multiply(details::dimensions<Dim> {}), rest...);
}

}  // namespace details

using empty_dimensions = details::empty_dimensions;
template<typename T>
concept dimensions_like = requires(T) {
    typename T::first_t;
    T::size();
    T::empty();
    typename T::negate_t;
    typename T::template multiply_t<empty_dimensions>;
    typename T::template divide_t<empty_dimensions>;
};

// Ensures order and uniqueness of dimensions
template<dimension_like... DimTs>
auto create_dimensions(DimTs... dims)
{
    return details::create(details::empty_dimensions {}, dims...);
}

template<dimension_like... DimTs>
using create_dimensions_t = decltype(create_dimensions(DimTs {}...));

}  // namespace twig
