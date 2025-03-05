#include <ratio>
#include <type_traits>

#include <stronk/prefabs.h>
#include <stronk/stronk.h>
#include <stronk/unit.h>

// We introduce a unit type with a default set of skills with the `stronk_default_unit` prefab
struct Watt : twig::stronk_default_unit<Watt, double>
{
    using stronk_default_unit::stronk_default_unit;
};

void watts_and_identity_units()
{
    Watt watt = Watt {30.};
    watt += Watt {4.} - Watt {2.};  // we can add and subtract units

    // Multiplying and dividing with an identity_unit (such as floats and integers) does not change the type.
    watt *= 2.;

    // However an identity_unit divided by a regular unit results in a new unit type.
    auto one_over_watt = 1.0 / watt;
    static_assert(!std::is_same_v<decltype(one_over_watt), Watt>);
}

// Let's introduce hours as a new unit_like type
struct Hours : twig::stronk<Hours, double, twig::unit>
{
    using stronk::stronk;
};

// We can now dynamically generate a new type!
using WattHours = decltype(Watt {} * Hours {});

void watt_hours_and_generating_new_units()
{
    // Multiplying the right units together will automatically produce the new type
    WattHours watt_hours = Hours {3.} * Watt {25.};

    // The new type supports adding, subtracting, comparing etc by default.
    watt_hours -= WattHours {10.} + WattHours {2.};

    // We can get back to Hours or Watt by dividing the opposite out.
    Hours hours = watt_hours / Watt {25.};
    Watt watt = watt_hours / Hours {3.};
}

// Let's introduce a type for euros, and start combining more types.
struct Euro : twig::stronk<Euro, double, twig::unit>
{
    using stronk::stronk;
};

void introducing_another_type()
{
    // twig::make allows you to scale the input value but it does not change the resulting type
    WattHours one_mega_watt_hour = twig::make<std::mega, WattHours>(1.);
    // Now we can generate a new type which consists of 3 types: `Euro / (Watt * Hours)`
    auto euros_per_mega_watt_hour = Euro {300.} / one_mega_watt_hour;

    // This flexibility allows us to write expessive code, while having the type system check our implementation.
    Euro price_for_buying_5_mega_watt_hours = euros_per_mega_watt_hour * twig::make<std::mega, WattHours>(5.);

    auto mega_watt_hours_per_euro = 1. / euros_per_mega_watt_hour;  // `(Watt * Hours) / Euro`
    WattHours mega_watt_hours_affordable_for_500_euros = mega_watt_hours_per_euro * Euro {500.};
}

auto main() -> int
{
    watts_and_identity_units();
    watt_hours_and_generating_new_units();
    introducing_another_type();
}
static_assert(__LINE__ == 75UL, "update readme if this changes");
