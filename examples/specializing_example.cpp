#include <stronk/specializers.h>

// In case you want to modify the strong type or the underlying type you can utilize the specializer header.
// Lets consider the following units:
struct Distance : twig::stronk<Distance, double, twig::unit>
{
    using stronk::stronk;
};

struct Time : twig::stronk<Time, double, twig::unit>
{
    using stronk::stronk;
};

// Note: For the macros you need to call them from within the twig namespace:
namespace twig
{
// By default the units are generated with the stronk_default_prefab type, but lets say we want to have Distance / Time
// specialized to be hashable. We can use the STRONK_SPECIALIZE_DIVIDE macro to specialize the generated type.
STRONK_SPECIALIZE_DIVIDE(Distance, Time, can_hash);
}  // namespace twig

// Now the Distance{} / Time{} will result in a unit stronk type with the can_hash skill
using Speed = decltype(Distance {} / Time {});  // this will now use the specialized version from above.

// You can also specialize the underlying type of multiplying two units:
// The default underlying type is the default result of multiplying or dividing the underlying types of the two units.
// Lets specialize Time^2 to use doubles as its underlying type.
template<>
struct twig::underlying_type_of_multiplying<Time, Time>
{
    using type = int64_t;
};

auto main() -> int
{
    auto speed_hash = std::hash<Speed> {}(Speed {25.});
    static_assert((Time {2.} * Time {4.}).unwrap<decltype(Time {} * Time {})>() == 8ULL);
}
