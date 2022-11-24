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

// Lets say you want to use a custom defined stronk type for certain unit combinations.
// Lets introduce our own `Speed` type:
struct Speed : twig::stronk<Speed, double, twig::can_hash, twig::divided_unit<Distance, Time>::skill>
{
    using stronk::stronk;
};
// Notice we are adding the twig::divided_unit skill instead of twig::unit

// And then we need to specialize `unit_lookup`:
template<>
struct twig::unit_lookup<twig::divided_unit<Distance, Time>::unit_description_t, double>
{
    using res_type = Speed;
};

// The following of course also works for `multiplied_unit` and `unit_multiplied_resulting_unit_type`

// Sometimes you might want to specialize the multiply or divide operation for the underlying value
// Lets specialize `Time^2` to use int64_t as its underlying type.
template<>
struct twig::underlying_multiply_operation<Time, Time>
{
    using res_type = int64_t;

    constexpr static auto multiply(const typename Time::underlying_type& v1,
                                   const typename Time::underlying_type& v2) noexcept -> res_type
    {
        return static_cast<int64_t>(v1 * v2);
    }
};
// Now the automatically generated stronk unit for Time^2 will have int64_t as the underlying type.

auto main() -> int
{
    using SpeedDeduced = decltype(Distance {} / Time {});
    [[maybe_unused]] auto speed_hash = std::hash<SpeedDeduced> {}(SpeedDeduced {25.});  // Its hash-able
    static_assert((Time {2.} * Time {4.}).unwrap<decltype(Time {} * Time {})>() == 8ULL);
}
static_assert(__LINE__ == 52UL, "update readme if this changes");
