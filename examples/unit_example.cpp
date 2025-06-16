#include <iostream>

#include "stronk/unit.hpp"

#include "stronk/skills/can_stream.hpp"
#include "stronk/utilities/ratio.hpp"

namespace twig
{

// Distance

struct meters : twig::unit<meters, twig::ratio<1>, can_ostream>
{
};

template<typename T>
using meters_t = meters::value<T>;

template<typename T>
using kilo_meters_t = meters::scaled_t<twig::kilo>::value<T>;

// TIME

struct seconds : twig::unit<seconds, twig::ratio<1>, can_ostream>
{
};

template<typename T>
using seconds_t = seconds::value<T>;

template<typename T>
using minutes_t = seconds::scaled_t<twig::ratio<60ULL>>::value<T>;

template<typename T>
using hours_t = seconds::scaled_t<twig::ratio<3600ULL>>::value<T>;

// MASS

struct kilograms : twig::unit<kilograms, twig::ratio<1>, can_ostream>
{
};

template<typename T>
using kilograms_t = kilograms::value<T>;

// The name of the generated type for `Distance` over `Time` is not really reader-friendly so making an alias can be
// nice.
using speed = divided_unit_t<meters, seconds>;
using acceleration = divided_unit_t<speed, seconds>;
using time_squared = multiplied_unit_t<seconds, seconds>;
using force = multiplied_unit_t<kilograms, acceleration>;

// Now we have it all set up
void example()
{
    auto two_hours = hours_t {2};
    std::cout << two_hours.to<twig::ratio<60>>() << " should be " << 120 << std::endl;

    is_unit<meters> auto ten_km = kilo_meters_t {10.};
    is_unit<seconds> auto forty_minutes = minutes_t {40};

    // Dividing different units will generate a new type (Distance/Time)
    is_unit<speed> auto fifteen_km_per_hour = (ten_km / forty_minutes);

    // And you get your original type out once there's only one type left
    is_unit<meters> auto distance_moved_over_2_hours_at_speed = two_hours * fifteen_km_per_hour;

    // units can be multiplied and divided by IdentityUnits (values without units)
    is_unit<meters> auto thirty_km = meters_t {30.} * 1000;
    std::cout << distance_moved_over_2_hours_at_speed.to<meters>() << " should be " << thirty_km << std::endl;
}

}  // namespace twig

auto main() -> int
{
    twig::example();
    return 0;
}
