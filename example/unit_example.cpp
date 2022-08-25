#include <iostream>
#include <ratio>

#include <stronk/stronk.h>
#include <stronk/unit.h>

namespace twig
{

// First we define our stronk types:
struct Distance : stronk<Distance, double, unit, can_equate_with_is_close, can_stream>
{
    using stronk::stronk;
};

struct Time : stronk<Time, int64_t, unit, can_equate, can_stream>
{
    using stronk::stronk;
};

struct Mass : stronk<Mass, int64_t, unit, can_equate, can_stream>
{
    using stronk::stronk;
};

// Some systems might want to create a stronk type for `Meters`, `Kilometers`, `LightYears`, etc. but we can also
// utilize the std::ratio system to have a single type for `Distance`.
struct Meters : std::ratio<1, 1>
{
    using base_unit_t = Distance;
};

struct Kilometers : std::kilo
{
    using base_unit_t = Distance;
};

struct Minutes : std::ratio<1'000'000'000ULL * 60, 1>
{
    using base_unit_t = Time;
};

struct Hours : std::ratio<1'000'000'000ULL * 60 * 60, 1>
{
    using base_unit_t = Time;
};

// The name of the generated type for `Distance` over `Time` is not really reader-friendly so making an alias can be
// nice.
using Speed = decltype(Distance {} / Time {});
using Acceleration = decltype(Speed {} / Time {});
using TimeSquared = decltype(Time {} * Time {});
using Force = decltype(Mass {} * Acceleration {});

// Now we have it all set up
void example()
{
    Time two_hours = make<Hours>(2);
    std::cout << two_hours.unwrap_as<Minutes>() << " should be " << 120 << std::endl;

    Distance ten_km = make<Kilometers>(10.);
    Time forty_minutes = make<Minutes>(40);

    // Dividing different units will generate a new type (Distance/Time)
    Speed fifteen_km_per_hour = ten_km / forty_minutes;
    // And you get your original type out once there's only one type left
    Distance distance_moved_over_2_hours_at_speed = two_hours * fifteen_km_per_hour;

    // units can be multiplied and divided by IdentityUnits (values without units)
    Distance thirty_km = make<Meters>(30.) * 1000;
    std::cout << distance_moved_over_2_hours_at_speed << " should be " << thirty_km << std::endl;
}

}  // namespace twig

auto main() -> int
{
    twig::example();
    return 0;
}
