#pragma once

namespace twig
{

template<typename StronkT>
struct can_const_index
{
    [[nodiscard]]
    constexpr auto operator[](const auto& indexer) const noexcept -> const auto&
    {
        return static_cast<const StronkT&>(*this).template unwrap<StronkT>()[indexer];
    }

    [[nodiscard]]
    constexpr auto at(const auto& indexer) const -> const auto&
    {
        return static_cast<const StronkT&>(*this).template unwrap<StronkT>().at(indexer);
    }
};

template<typename StronkT>
struct can_index : can_const_index<StronkT>
{
    using can_const_index<StronkT>::operator[];
    using can_const_index<StronkT>::at;

    [[nodiscard]]
    constexpr auto operator[](const auto& indexer) noexcept -> auto&
    {
        return static_cast<StronkT&>(*this).template unwrap<StronkT>()[indexer];
    }

    [[nodiscard]]
    constexpr auto at(const auto& indexer) -> auto&
    {
        return static_cast<StronkT&>(*this).template unwrap<StronkT>().at(indexer);
    }
};

}  // namespace twig
