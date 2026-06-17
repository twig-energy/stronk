#pragma once

#include <type_traits>

#include "stronk/stronk.hpp"
namespace twig
{

template<typename StronkT, typename ViewT, template<typename> typename... Skills>
struct stronk_view_of : public stronk<stronk_view_of<StronkT, ViewT, Skills...>, ViewT, Skills...>
{
    using view_of_t = StronkT;
    using base_t = stronk<stronk_view_of, ViewT, Skills...>;

    explicit(false) stronk_view_of(StronkT& stronk)
        : base_t(ViewT {stronk.template unwrap<std::remove_const_t<StronkT>>()})
    {
    }

    constexpr auto operator==(StronkT& other) const noexcept -> bool
    {
        return this->val() == stronk_view_of {other}.val();
    }

    constexpr auto operator<=>(StronkT& other) const noexcept -> auto
    {
        return this->val() <=> stronk_view_of {other}.val();
    }

    constexpr explicit operator StronkT() const noexcept
        requires std::constructible_from<typename StronkT::underlying_type, ViewT>
    {
        using t = StronkT::underlying_type;
        return StronkT {t {this->val()}};
    }
};

template<typename ViewT, template<typename> typename... Skills>
struct can_be_const_viewed_as
{
    template<typename StronkT>
    struct skill
    {
        using view_t = stronk_view_of<const StronkT, ViewT, Skills...>;
    };
};

template<typename ViewT, template<typename> typename... Skills>
struct can_be_mutable_viewed_as
{
    template<typename StronkT>
    struct skill
    {
        using mutable_view_t = stronk_view_of<StronkT, ViewT, Skills...>;
    };
};

}  // namespace twig
