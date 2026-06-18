#pragma once

namespace twig
{

template<typename ViewT>
struct can_be_const_viewed_as
{
    template<typename StronkT>
    struct skill
    {
        struct view_t
        {
            ViewT view;

            constexpr auto val() const noexcept -> auto
            {
                return this->view;
            }

            constexpr auto begin() const noexcept -> auto
            {
                return this->view.begin();
            }
            constexpr auto end() const noexcept -> auto
            {
                return this->view.end();
            }
            constexpr auto operator[](auto idx) const noexcept -> auto&
            {
                return this->view[idx];
            }
            constexpr auto size() const noexcept -> auto
            {
                return this->view.size();
            }
            constexpr auto operator==(const StronkT& rhs) const -> bool
            {
                return this->view == rhs.template unwrap<StronkT>();
            }
            constexpr auto operator==(const view_t& rhs) const -> bool = default;

            constexpr explicit operator StronkT() const
            {
                return StronkT {typename StronkT::underlying_type {view}};
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
        struct mutable_view_t
        {
            ViewT view;

            constexpr auto val() const noexcept -> auto
            {
                return this->view;
            }

            constexpr auto begin() const noexcept -> auto
            {
                return this->view.begin();
            }
            constexpr auto end() const noexcept -> auto
            {
                return this->view.end();
            }
            constexpr auto operator[](auto idx) const noexcept -> auto&
            {
                return this->view[idx];
            }
            constexpr auto size() const noexcept -> auto
            {
                return this->view.size();
            }
            constexpr auto operator==(const StronkT& rhs) const -> bool
            {
                return this->view == rhs.template unwrap<StronkT>();
            }
            constexpr auto operator==(const mutable_view_t& rhs) const -> bool = default;

            constexpr explicit operator StronkT() const
            {
                return StronkT {typename StronkT::underlying_type {view}};
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
