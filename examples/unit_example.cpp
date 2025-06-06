#include <iostream>

#include "stronk/unit.hpp"

#include "stronk/skills/can_stream.hpp"
#include "stronk/utilities/ratio.hpp"

namespace twig
{

// Distance

struct meters_unit : twig::unit<meters_unit, twig::ratio<1>, can_ostream>
{
};

template<typename T>
using meters = meters_unit::value<T>;

template<typename T>
using kilo_meters = meters_unit::scaled_t<twig::kilo>::value<T>;

// TIME

struct seconds_unit : twig::unit<seconds_unit, twig::ratio<1>, can_ostream>
{
};

template<typename T>
using seconds = seconds_unit::value<T>;

template<typename T>
using minutes = seconds_unit::scaled_t<twig::ratio<60ULL>>::value<T>;

template<typename T>
using hours = seconds_unit::scaled_t<twig::ratio<3600ULL>>::value<T>;

// MASS

struct kilograms_unit : twig::unit<kilograms_unit, twig::ratio<1>, can_ostream>
{
};

template<typename T>
using kilograms = kilograms_unit::value<T>;

// The name of the generated type for `Distance` over `Time` is not really reader-friendly so making an alias can be
// nice.
using meters_per_second_unit = divided_unit_t<meters_unit, seconds_unit>;
using acceleration_unit = divided_unit_t<meters_per_second_unit, seconds_unit>;
using time_squared_unit = multiplied_unit_t<seconds_unit, seconds_unit>;
using force_unit = multiplied_unit_t<kilograms_unit, acceleration_unit>;

// Now we have it all set up
void example()
{
    auto two_hours = hours {2};
    std::cout << two_hours.to<minutes>() << " should be " << 120 << std::endl;

    is_unit<meters_unit> auto ten_km = kilo_meters {10.};
    is_unit<seconds_unit> auto forty_minutes = minutes {40};

    // Dividing different units will generate a new type (Distance/Time)
    is_unit<meters_per_second_unit> auto fifteen_km_per_hour = (ten_km / forty_minutes);

    // And you get your original type out once there's only one type left
    is_unit<meters_unit> auto distance_moved_over_2_hours_at_speed = two_hours * fifteen_km_per_hour;

    // units can be multiplied and divided by IdentityUnits (values without units)
    is_unit<meters_unit> auto thirty_km = meters {30.} * 1000;
    std::cout << distance_moved_over_2_hours_at_speed.to<meters>() << " should be " << thirty_km << std::endl;
}

}  // namespace twig

auto main() -> int
{
    twig::example();
    return 0;
}
