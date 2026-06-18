#pragma once

#include "stronk/stronk.hpp"

namespace twig
{

template<typename ConvertibleTo>
struct can_explicitly_convert_to_stronk_of
{
    template<typename StronkT>
    struct skill
    {
        // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
        constexpr explicit(false) operator ConvertibleTo() const noexcept
        {
            const auto& self = static_cast<const StronkT&>(*this);
            const auto& underlying = self.template unwrap<StronkT>();
            using new_underlying_t = ConvertibleTo::underlying_type;
            return ConvertibleTo {new_underlying_t {underlying}};
        }
    };
};

template<typename EquatableWithT>
struct stronk_equatable_with
{
    template<typename StronkT>
    struct skill
    {
        constexpr auto operator==(const EquatableWithT& rhs) -> bool
        {
            return static_cast<const StronkT&>(*this).val() == rhs.template unwrap<EquatableWithT>();
        }
    };
};

template<typename ViewT, template<typename> typename... Skills>
struct can_be_const_viewed_as
{
    template<typename StronkT>
    struct skill
    {
        struct view_t
            : stronk<view_t,
                     ViewT,
                     can_explicitly_convert_to_stronk_of<StronkT>::template skill,
                     stronk_equatable_with<StronkT>::template skill,
                     Skills...>
        {
            using stronk<view_t,
                         ViewT,
                         can_explicitly_convert_to_stronk_of<StronkT>::template skill,
                         stronk_equatable_with<StronkT>::template skill,
                         Skills...>::stronk;
        };

        // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
        constexpr explicit(false) operator view_t() const noexcept
        {
            using t = view_t::underlying_type;
            return view_t {t {static_cast<const StronkT&>(*this).template unwrap<StronkT>()}};
        }
    };
};

template<typename ViewT, template<typename> typename... Skills>
struct can_be_mutable_viewed_as
{
    template<typename StronkT>
    struct skill
    {
        struct mutable_view_t
            : stronk<mutable_view_t,
                     ViewT,
                     can_explicitly_convert_to_stronk_of<StronkT>::template skill,
                     stronk_equatable_with<StronkT>::template skill,
                     Skills...>
        {
            using stronk<mutable_view_t,
                         ViewT,
                         can_explicitly_convert_to_stronk_of<StronkT>::template skill,
                         stronk_equatable_with<StronkT>::template skill,
                         Skills...>::stronk;
        };

        // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
        constexpr explicit(false) operator mutable_view_t() noexcept
        {
            using t = mutable_view_t::underlying_type;
            return mutable_view_t {t {static_cast<StronkT&>(*this).template unwrap<StronkT>()}};
        }
    };
};

}  // namespace twig
