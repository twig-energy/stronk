#include <ratio>

#include <stronk/prefabs.h>
#include <stronk/unit.h>

// We introduce a unit type with a default set of skills with the `stronk_default_unit` prefab
struct Watt : twig::stronk_default_unit<Watt, double>
{
    using stronk_default_unit::stronk_default_unit;
};
// And we also try to add an identity-unit type

void watts_and_identity_units()
{
    Watt watt = Watt {30.};
    watt += Watt {4.} - Watt {2.};  // we can add and subtract units

    // Multiplying and dividing with identity unit does not change the type.
    watt *= 2.;

    // However as identity unit divided by a unit results in a new unit type of.
    auto one_over_watt = 1.0 / watt;
    static_assert(!std::is_same_v<decltype(one_over_watt), Watt>);
}

// Next we introduce hours and a new unit_like type
struct Hours : twig::stronk<Hours, double, twig::unit>
{
    using stronk::stronk;
};

// We can now dynamically generate a new type based on our stronk types.
// The generated type is very verbose so adding an alias can be helpful
using WattHours = decltype(Watt {} * Hours {});

void watt_hours_and_generating_new_units()
{
    // Multiplying the right units together will automatically produce the new type
    WattHours watt_hours = Hours {3.5} * Watt {25.0};

    // this new type can also add and subtract from itself.
    watt_hours -= WattHours {10.} + WattHours {2.};

    // We can get back the hours or the watt by dividing by them out.
    Hours hours = watt_hours / Watt {25.0};
    Watt watt = watt_hours / Hours {3.5};
}

// Lets introduce money so we can really start combining types.
struct Euro : twig::stronk<Euro, double, twig::unit>
{
    using stronk::stronk;
};

void introducing_another_type()
{
    // There are make functions for std::ratio like types. It simply scales the input value and does not change the type
    WattHours watt_hours = twig::make<std::mega, WattHours>(1.);
    // Now we can generate a new type which consists of 3 types: `Euro / (Watt * Hours)`
    auto euros_per_watt_hour = Euro {300.} / watt_hours;

    // which ofcours also can be handled like any other unit which allows this flexible strongly typed system.
    Euro price_for_buying_5_megawatt = euros_per_watt_hour * twig::make<std::mega, WattHours>(5.);

    auto watt_hours_per_euro = 1. / euros_per_watt_hour;  // `(Watt * Hours) / Euro`
    WattHours watt_hours_affordable_for_500_euros = watt_hours_per_euro * Euro {500.};
}

auto main() -> int
{
    watts_and_identity_units();
    watt_hours_and_generating_new_units();
    introducing_another_type();
}
static_assert(__LINE__ == 75UL, "update readme if this changes");
