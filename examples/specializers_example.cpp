#include <concepts>
#include <cstdint>
#include <ratio>

#include "stronk/unit.hpp"

// Let's consider the following units:
struct meters_unit : twig::unit<meters_unit, twig::ratio<1>>
{
};

struct seconds_unit : twig::unit<seconds_unit, twig::ratio<1>>
{
};

// Let's say you want to use a custom defined stronk type for certain unit combinations.
// Let's introduce our own `Speed` type:
struct meters_per_second_unit : twig::unit<twig::divided_dimensions_t<meters_unit, seconds_unit>, twig::ratio<1>>
{
};
// Notice we are using twig::divided_dimensions_t instead of the regular tag

// To make it possible for stronk to find this type we need to specialize `unit_lookup`:
template<>
struct twig::unit_lookup<twig::divided_dimensions_t<meters_unit, seconds_unit>>
{
    template<scale_like ScaleT>  // scale is to support kilo meters / second, or nano meters / second
    using unit_t = twig::unit_scaled_or_base_t<meters_per_second_unit, ScaleT>;
};

// Now the automatically generated stronk unit for seconds^2 is meters_per_second

// The above of course also works for `multiplied_unit` and `unit_multiplied_resulting_unit_type`

// Sometimes you might want to specialize the multiply or divide operation for the underlying value
// Let's specialize `seconds^2 for double values` to use int64_t as its resulting type.

template<typename T>
using seconds = seconds_unit::value<T>;

template<>
struct twig::underlying_multiply_operation<seconds<double>, seconds<double>>
{
    using res_type = int64_t;

    constexpr static auto multiply(const double v1, const double v2) noexcept -> res_type
    {
        return static_cast<int64_t>(v1 * v2);
    }
};
// Now when multiplying two seconds<double>, the resulting type will be seconds^2::value<int64_t>

auto main() -> int
{
    using speed_deduced = twig::divided_unit_t<meters_unit, seconds_unit>;
    static_assert(std::same_as<speed_deduced, meters_per_second_unit>);
}
static_assert(__LINE__ == 58UL, "update readme if this changes");
