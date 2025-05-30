#pragma once
#include <utility>

#include <absl/hash/hash.h>  // IWYU pragma: keep

namespace twig
{
template<class StronkT>
struct can_absl_hash
{
    template<typename H>
    friend auto AbslHashValue(H h, const StronkT& c) -> H
    {
        return H::combine(std::move(h), c.template unwrap<StronkT>());
    }
};
}  // namespace twig
