```
==================================================================

*      //   ) ) /__  ___/ //   ) )  //   ) ) /|    / / //   / /  *
*     ((          / /    //___/ /  //   / / //|   / / //__ / /   *
*       \\       / /    / ___ (   //   / / // |  / / //__  /     *
*         ) )   / /    //   | |  //   / / //  | / / //   \ \     *
*  ((___ / /   / /    //    | | ((___/ / //   |/ / //     \ \    *

==================================================================
```

=== An easy to customize strong type library with built in support for unit-like behavior ===

```
struct your_type : stronk<your_type, int, can_add, can_order, can_equate, can_hash>
{
    using stronk::stronk;
};
```

## Current list of skills
### Regular
- `can_negate`: unary `operator-`
- `can_add`: binary `operator+` `operator=+`
- `can_subtract`: binary `operator-` and `operator=-`
- `can_multiply`: binary `operator*` and `operator=*` (not compatible with units)
- `can_divide`: binary `operator/` and `operator=/` (not compatible with units)
- `can_abs`: `twig::abs` will accept strong-types with this skill. `twig::abs` will not accept strong types without this skill.
- `can_isnan`: `twig::isnan` will accept strong-types with this skill
- `can_stream`: `operator<<(std::ostream)`, stream the underlying value to the stream.
- `can_order`: `operator<=>`, note you probably also want to add `can_equate` or one of its relatives, since the compiler cannot generate equality with the `operator<=>` for stronk types (since its not default implemented)
- `can_equate`: `operator==` with regular equality
- `can_equate_with_is_close`: `operator==` but with numpy's `is_close` definition of equal
- `can_equate_with_is_close_nan_equals`: `operator==` but with numpy's `is_close` definition of equal, nans being equal
- `can_equate_with_is_close_abs_tol_only`: `operator==` with a small absolute tolorance for difference
- `can_less_than_greater_than`: `operator<` and `operator>` (prefer the `can_order` skill instead)
- `can_less_than_greater_than_or_equal`: operator <= and operator >= (prefer the `can_order` skill instead)
- `can_be_used_as_flag`: for boolean values used as flags

### Units
- `unit`: enables unit behavior for multiplication and division.
- `identity_unit`: enables unit behavior, but does not affect the type of multiplication and division.

### Third Party Library extensions (see `stronk/extensions/<library>.h`):
- `can_hash`: implements the `AbslHashValue` friend function.
- `can_gtest_print`: for printing the values in gtest check macros
- `can_format_builder<"an_fmt_formatstring{}">::type`: implements `struct fmt::formatter<T>`. Will enable `std::format` in the future as well.

### Prefabs: (see `stronk/prefabs.h`)
- `stronk_default_unit`: arithmetic unit with most of the regular operations
- `stronk_flag`: flag like boolean with equal operators etc.

Adding new skills is easy so feel free to add more.


Credit to https://www.foonathan.net/2016/10/strong-typedefs/ for a great amount of inspiration


# Building and installing

See the [BUILDING](BUILDING.md) document.

# Contributing

See the [CONTRIBUTING](CONTRIBUTING.md) document.

# Licensing

See the [LICENSE](LICENSE.md) document.
