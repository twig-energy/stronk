#include <concepts>
#include <ratio>

#include <stronk/stronk.hpp>
#include <stronk/unit.hpp>

// We introduce a unit type with a default set of skills with the `stronk_default_unit` prefab
struct joules_unit : twig::stronk_default_unit<joules_unit, twig::ratio<1>>
{
};

template<typename T>
using joules = joules_unit::value<T>;

void joules_and_identity_units()
{
    auto energy = joules<double> {30.};
    energy += joules<double> {4.} - joules<double> {2.};  // we can add and subtract units

    // Multiplying and dividing with an identity_unit (such as floats and integers) does not change the type.
    energy *= 2.;

    // However an identity_unit divided by a regular unit results in a new unit type.
    auto one_over_joules = 1.0 / energy;
    static_assert(!std::same_as<decltype(one_over_joules), joules<double>>);
}

// Let's introduce seconds as a new unit
struct seconds_unit : twig::stronk_default_unit<seconds_unit, twig::ratio<1>>
{
};

template<typename T>
using seconds = seconds_unit::value<T>;

// We can define ratios of a specific unit - these scaled units have the same dimension
template<typename T>
using hours = seconds_unit::scaled_t<twig::ratio<60 * 60>>::value<T>;

// We can now dynamically generate a new type!
using watt_unit = twig::divided_unit_t<joules_unit, seconds_unit>;

template<typename T>
using watt = watt_unit::value<T>;

// or make custom names for already known types (joules) with specific scale
using watt_hours_unit = decltype(watt<double> {} * hours<double> {})::unit_t;

template<typename T>
using watt_hours = watt_hours_unit::value<T>;

void watt_hours_and_generating_new_units()
{
    // Multiplying the right units together will automatically produce the new type
    watt_hours<double> watt_hours_val = hours<double> {3.} * watt<double> {25.};

    // The new type supports adding, subtracting, comparing etc by default.
    watt_hours_val -= watt_hours<double> {10.} + watt_hours<double> {2.};

    // We can get back to Hours or Watt by dividing the opposite out.
    hours<double> hours_val = watt_hours_val / watt<double> {25.};
    watt<double> watt_val = watt_hours_val / hours<double> {3.};
}

// Let's introduce a type for euros, and start combining more types.
struct euro_unit : twig::stronk_default_unit<euro_unit, twig::ratio<1>>
{
};
template<typename T>
using euro = euro_unit::value<T>;

template<typename T>
using mega_watt_hours =
    joules_unit::scaled_t<twig::ratio_multiply<twig::mega, typename watt_hours_unit::scale_t>>::value<T>;

void introducing_another_type()
{
    // twig::make allows you to scale the input value but it does not change the resulting type
    mega_watt_hours<double> one_mega_watt_hour = mega_watt_hours<double> {1.};
    // Now we can generate a new type which consists of 3 types: `Euro / (Watt * Hours)`
    auto euros_per_mega_watt_hour = euro<double> {300.} / one_mega_watt_hour;

    // This flexibility allows us to write expessive code, while having the type system check our implementation.
    euro<double> price_for_buying_5_mega_watt_hours =
        euros_per_mega_watt_hour * (twig::identity_value_t<twig::mega, double> {1} * watt_hours<double> {5.});

    auto mega_watt_hours_per_euro = 1. / euros_per_mega_watt_hour;  // `(Watt * Hours) / Euro`
    mega_watt_hours<double> mega_watt_hours_affordable_for_500_euros = mega_watt_hours_per_euro * euro<double> {500.};
}

auto main() -> int
{
    joules_and_identity_units();
    watt_hours_and_generating_new_units();
    introducing_another_type();
}
static_assert(__LINE__ == 97UL, "update readme if this changes");
