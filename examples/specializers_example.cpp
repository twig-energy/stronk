#include <stronk/specializers.h>

// Lets consider the following units:
struct Distance : twig::stronk<Distance, double, twig::unit>
{
    using stronk::stronk;
};

struct Time : twig::stronk<Time, double, twig::unit>
{
    using stronk::stronk;
};

// Note: For the specializer macros you need to call them from within the twig namespace:
namespace twig
{
// Lets say we want to have Distance / Time specialized to be hashable.
// We can use the STRONK_SPECIALIZE_DIVIDE macro to specialize the generated type.
STRONK_SPECIALIZE_DIVIDE(Distance, Time, can_hash);
// Now any expression resulting the `Distance{} / Time{}` type will result in a unit type with the can_hash skill

}  // namespace twig

// Lets specialize Time^2 to use int64_t as its underlying type.
template<>
struct twig::underlying_multiply_operation<Time, Time>
{
    using res_type = int64_t;

    STRONK_FORCEINLINE
    constexpr static auto multiply(const typename Time::underlying_type& v1,
                                   const typename Time::underlying_type& v2) noexcept -> res_type
    {
        return static_cast<int64_t>(v1 * v2);
    }
};

auto main() -> int
{
    using Speed = decltype(Distance {} / Time {});
    auto speed_hash = std::hash<Speed> {}(Speed {25.});
    static_assert((Time {2.} * Time {4.}).unwrap<decltype(Time {} * Time {})>() == 8ULL);
}
static_assert(__LINE__ == 44UL, "update readme if this changes");
