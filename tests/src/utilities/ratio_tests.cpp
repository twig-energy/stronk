#include <concepts>

#include "stronk/utilities/ratio.hpp"

namespace twig
{

// supports very large numbers
static_assert(std::same_as<ratio_multiply<ratio<1'000'000'000'000>, ratio<1'000'000'000'000>>,
                           ratio<stronk_details::u_biggest_int_t {1'000'000'000'000}
                                 * stronk_details::u_biggest_int_t {1'000'000'000'000}>>);

}  // namespace twig
