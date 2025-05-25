#pragma once
// TODO(anders.wind) remove once downstream includes the right headers

#include "stronk/stronk.hpp"  // IWYU pragma: keep

// here because we want to be backwards compatible, fix when downstream has been updated to import these directly
#include "stronk/skills/can_abs.hpp"              // IWYU pragma: keep
#include "stronk/skills/can_be_used_as_flag.hpp"  // IWYU pragma: keep
#include "stronk/skills/can_divide.hpp"           // IWYU pragma: keep
#include "stronk/skills/can_index.hpp"            // IWYU pragma: keep
#include "stronk/skills/can_isnan.hpp"            // IWYU pragma: keep
#include "stronk/skills/can_iterate.hpp"          // IWYU pragma: keep
#include "stronk/skills/can_multiply.hpp"         // IWYU pragma: keep
