#pragma once

namespace twig
{

template<typename StronkT>
struct can_increment
{
    constexpr auto operator++() noexcept -> StronkT&
    {
        ++static_cast<StronkT&>(*this).template unwrap<StronkT>();
        return static_cast<StronkT&>(*this);
    }

    constexpr auto operator++(int) noexcept -> StronkT
    {
        auto copy = static_cast<StronkT&>(*this);
        ++static_cast<StronkT&>(*this).template unwrap<StronkT>();
        return copy;
    }
};

}  // namespace twig
