#pragma once
#include <concepts>
#include <cstddef>
#include <functional>
#include <type_traits>

namespace twig
{

template<typename StronkT>
struct can_hash
{
    using can_hash_indicator = std::true_type;
};
template<typename StronkT>
concept can_hash_like = std::same_as<typename StronkT::can_hash_indicator, std::true_type>;

}  // namespace twig
template<twig::can_hash_like T>
struct std::hash<T>  // NOLINT(cert-dcl58-cpp) std::hash is exempt from this rule
{
    [[nodiscard]]
    auto operator()(const T& s) const noexcept -> std::size_t
    {
        return std::hash<typename T::underlying_type> {}(s.template unwrap<T>());
    }
};
