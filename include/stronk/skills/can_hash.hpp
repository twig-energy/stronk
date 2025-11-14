#pragma once
#include <cstddef>
#include <functional>
#include <type_traits>

#include "stronk/stronk.hpp"

namespace twig
{

template<typename StronkT>
struct [[deprecated("std::hash<T> now just works for all stronk types which can be hashed")]] can_hash
{
    using can_hash_indicator = std::true_type;
};

}  // namespace twig

template<twig::stronk_like T>
struct std::hash<T>  // NOLINT(cert-dcl58-cpp) std::hash is exempt from this rule
{
    [[nodiscard]]
    auto operator()(const T& s) const noexcept -> std::size_t
    {
        return std::hash<typename T::underlying_type> {}(s.template unwrap<T>());
    }
};
