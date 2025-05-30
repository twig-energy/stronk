#pragma once

namespace twig
{

template<typename StronkT>
struct can_const_iterate
{
    [[nodiscard]]
    constexpr auto begin() const noexcept
    {
        return static_cast<const StronkT&>(*this).template unwrap<StronkT>().begin();
    }
    [[nodiscard]]
    constexpr auto end() const noexcept
    {
        return static_cast<const StronkT&>(*this).template unwrap<StronkT>().end();
    }

    [[nodiscard]]
    constexpr auto cbegin() const noexcept
    {
        return static_cast<const StronkT&>(*this).template unwrap<StronkT>().begin();
    }
    [[nodiscard]]
    constexpr auto cend() const noexcept
    {
        return static_cast<const StronkT&>(*this).template unwrap<StronkT>().end();
    }
};

template<typename StronkT>
struct can_iterate : can_const_iterate<StronkT>
{
    using can_const_iterate<StronkT>::begin;
    using can_const_iterate<StronkT>::end;

    using can_const_iterate<StronkT>::cbegin;
    using can_const_iterate<StronkT>::cend;

    [[nodiscard]]
    constexpr auto begin() noexcept
    {
        return static_cast<StronkT&>(*this).template unwrap<StronkT>().begin();
    }
    [[nodiscard]]
    constexpr auto end() noexcept
    {
        return static_cast<StronkT&>(*this).template unwrap<StronkT>().end();
    }
};

}  // namespace twig
