#pragma once

#include "stronk/stronk.hpp"
namespace twig
{

template<typename ViewT>
struct can_be_const_viewed_as
{
    template<typename StronkT>
    struct skill
    {
        struct view_t : stronk<view_t, ViewT, can_equate>
        {
            using stronk<view_t, ViewT, can_equate>::stronk;
            constexpr auto begin() const noexcept -> auto
            {
                return this->_you_should_not_be_using_this_but_rather_unwrap.begin();
            }
            constexpr auto end() const noexcept -> auto
            {
                return this->_you_should_not_be_using_this_but_rather_unwrap.end();
            }
            constexpr auto operator[](auto idx) const noexcept -> auto&
            {
                return this->_you_should_not_be_using_this_but_rather_unwrap[idx];
            }
            constexpr auto size() const noexcept -> auto
            {
                return this->_you_should_not_be_using_this_but_rather_unwrap.size();
            }
            constexpr auto operator==(const StronkT& rhs) const -> bool
            {
                return this->_you_should_not_be_using_this_but_rather_unwrap == rhs.template unwrap<StronkT>();
            }

            constexpr explicit operator StronkT() const
            {
                return StronkT {
                    typename StronkT::underlying_type {this->_you_should_not_be_using_this_but_rather_unwrap}};
            }
        };

        // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
        constexpr explicit(false) operator view_t() const noexcept
        {
            return view_t {ViewT {static_cast<const StronkT&>(*this).template unwrap<StronkT>()}};
        }
    };
};

template<typename ViewT>
struct can_be_mutable_viewed_as
{
    template<typename StronkT>
    struct skill
    {
        struct mutable_view_t : stronk<mutable_view_t, ViewT, can_equate>
        {
            using stronk<mutable_view_t, ViewT, can_equate>::stronk;

            constexpr auto begin() const noexcept -> auto
            {
                return this->_you_should_not_be_using_this_but_rather_unwrap.begin();
            }
            constexpr auto end() const noexcept -> auto
            {
                return this->_you_should_not_be_using_this_but_rather_unwrap.end();
            }
            constexpr auto operator[](auto idx) const noexcept -> auto&
            {
                return this->_you_should_not_be_using_this_but_rather_unwrap[idx];
            }
            constexpr auto size() const noexcept -> auto
            {
                return this->_you_should_not_be_using_this_but_rather_unwrap.size();
            }
            constexpr auto operator==(const StronkT& rhs) const -> bool
            {
                return this->_you_should_not_be_using_this_but_rather_unwrap == rhs.template unwrap<StronkT>();
            }

            constexpr explicit operator StronkT() const
            {
                return StronkT {
                    typename StronkT::underlying_type {this->_you_should_not_be_using_this_but_rather_unwrap}};
            }
        };

        // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
        constexpr explicit(false) operator mutable_view_t() noexcept
        {
            return mutable_view_t {ViewT {static_cast<StronkT&>(*this).template unwrap<StronkT>()}};
        }
    };
};

}  // namespace twig
