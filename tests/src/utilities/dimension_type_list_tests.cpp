#include <concepts>

#include <stronk/utilities/constexpr_helpers.h>
#include <stronk/utilities/dimension_type_list.h>

namespace twig::experiments
{

struct distance
{};

struct time
{};

using Speed = DimensionList<Dimension<distance, 1>, Dimension<time, -1>>;

static_assert(std::same_as<add_dimensions_t<Speed, Speed>, DimensionList<Dimension<distance, 2>, Dimension<time, -2>>>);
static_assert(std::same_as<subtract_dimensions_t<Speed, Speed>, DimensionList<>>);

}  // namespace twig::experiments
