#include <stronk/specializers.h>

namespace twig::tests
{

struct specializer_type_a : stronk_default_unit<specializer_type_a, int32_t>
{
    using stronk_default_unit::stronk_default_unit;
};
struct specializer_type_b : stronk_default_unit<specializer_type_b, int32_t>
{
    using stronk_default_unit::stronk_default_unit;
};

}  // namespace twig::tests

namespace twig
{
STRONK_SPECIALIZE_MULTIPLY(tests::specializer_type_a, tests::specializer_type_a);
STRONK_SPECIALIZE_MULTIPLY(tests::specializer_type_a, tests::specializer_type_b, can_equate);
STRONK_SPECIALIZE_DIVIDE(tests::specializer_type_a, tests::specializer_type_b, can_equate, can_stream);

using specializer_type_a_squared = decltype(tests::specializer_type_a {} * tests::specializer_type_a {});
using specializer_type_a_times_b = decltype(tests::specializer_type_a {} * tests::specializer_type_b {});
using specializer_type_a_divided_by_b = decltype(tests::specializer_type_a {} / tests::specializer_type_b {});

STRONK_SPECIALIZE_MULTIPLY(specializer_type_a_squared, tests::specializer_type_a);  // a^3
STRONK_SPECIALIZE_MULTIPLY(specializer_type_a_squared, tests::specializer_type_b);  // a^2 * b
// STRONK_SPECIALIZE_MULTIPLY(specializer_type_a_times_b, tests::specializer_type_a); // a^2 * b
STRONK_SPECIALIZE_MULTIPLY(specializer_type_a_times_b, tests::specializer_type_b);  // a * b^2
STRONK_SPECIALIZE_MULTIPLY(specializer_type_a_divided_by_b, tests::specializer_type_a);  // a^2 / b
// STRONK_SPECIALIZE_MULTIPLY(specializer_type_a_divided_by_b, tests::specializer_type_b); // a

// STRONK_SPECIALIZE_DIVIDE(specializer_type_a_squared, tests::specializer_type_a); // a
// STRONK_SPECIALIZE_DIVIDE(specializer_type_a_squared, tests::specializer_type_b);  // a^2 / b
// STRONK_SPECIALIZE_DIVIDE(specializer_type_a_times_b, tests::specializer_type_a); // b
// STRONK_SPECIALIZE_DIVIDE(specializer_type_a_times_b, tests::specializer_type_b);  // a
STRONK_SPECIALIZE_DIVIDE(specializer_type_a_divided_by_b, tests::specializer_type_a);  // 1 / b
STRONK_SPECIALIZE_DIVIDE(specializer_type_a_divided_by_b, tests::specializer_type_b);  // a / b^2

// STRONK_SPECIALIZE_DIVIDE(tests::specializer_type_a, specializer_type_a_squared); // a
STRONK_SPECIALIZE_DIVIDE(tests::specializer_type_b, specializer_type_a_squared);  // b / a^2
// STRONK_SPECIALIZE_DIVIDE(tests::specializer_type_a, specializer_type_a_times_b); // b
// STRONK_SPECIALIZE_DIVIDE(tests::specializer_type_b, specializer_type_a_times_b); // a
// STRONK_SPECIALIZE_DIVIDE(tests::specializer_type_a, specializer_type_a_divided_by_b);  // b
STRONK_SPECIALIZE_DIVIDE(tests::specializer_type_b, specializer_type_a_divided_by_b);  // b^2

}  // namespace twig
