#pragma once

#include <concepts>
namespace twig
{

template<typename StronkT>
struct can_be_used_as_flag
{
    [[nodiscard]]
    constexpr auto is_on() const noexcept -> bool
    {
        static_assert(std::same_as<typename StronkT::underlying_type, bool>);
        return static_cast<const StronkT&>(*this).template unwrap<StronkT>();
    }

    [[nodiscard]]
    constexpr auto is_off() const noexcept -> bool
    {
        return !this->is_on();
    }

    [[nodiscard]]
    static constexpr auto on() noexcept -> StronkT
    {
        static_assert(std::same_as<typename StronkT::underlying_type, bool>);
        return StronkT {true};
    }

    [[nodiscard]]
    static constexpr auto off() noexcept -> StronkT
    {
        static_assert(std::same_as<typename StronkT::underlying_type, bool>);
        return StronkT {false};
    }
};

}  // namespace twig
