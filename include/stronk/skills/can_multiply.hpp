#pragma once
/**
 * @brief Consider when using these skills to instead make your type a proper unit using the stronk/unit.hpp header
 *
 */

namespace twig
{

template<typename StronkT>
struct can_multiply
{
    constexpr friend auto operator*=(StronkT& lhs, const StronkT& rhs) noexcept -> StronkT
    {
        lhs.template unwrap<StronkT>() *= rhs.template unwrap<StronkT>();
        return lhs;
    }

    constexpr friend auto operator*(const StronkT& lhs, const StronkT& rhs) noexcept -> StronkT
    {
        return StronkT {lhs.template unwrap<StronkT>() * rhs.template unwrap<StronkT>()};
    }
};

template<typename T>
struct can_multiply_with
{
    template<typename StronkT>
    struct skill
    {
        constexpr friend auto operator*=(StronkT& lhs, const T& rhs) noexcept -> StronkT
        {
            lhs.template unwrap<StronkT>() *= rhs;
            return lhs;
        }

        constexpr friend auto operator*(const StronkT& lhs, const T& rhs) noexcept -> StronkT
        {
            return StronkT {lhs.template unwrap<StronkT>() * rhs};
        }

        constexpr friend auto operator*(const T& lhs, const StronkT& rhs) noexcept -> StronkT
        {
            return StronkT {lhs * rhs.template unwrap<StronkT>()};
        }
    };
};

}  // namespace twig
