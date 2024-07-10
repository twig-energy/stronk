#include <concepts>

#include <stronk/utilities/constexpr_helpers.h>
#include <stronk/utilities/dimension_type_list.h>

namespace twig
{

struct distance
{};

struct time
{};

using Distance = Dimensions<Dimension<distance, 1>>;
using Time = Dimensions<Dimension<time, 1>>;
using Speed = Dimensions<Dimension<distance, 1>, Dimension<time, -1>>;

static_assert(std::same_as<create_dimensions_t<>, EmptyDimensions>);
static_assert(std::same_as<create_dimensions_t<Dimension<distance, 1>>, Distance>);
static_assert(std::same_as<create_dimensions_t<Dimension<distance, 1>, Dimension<time, -1>>, Speed>);
static_assert(std::same_as<create_dimensions_t<Dimension<distance, 1>, Dimension<time, -1>>,
                           create_dimensions_t<Dimension<time, -1>, Dimension<distance, 1>>>);
static_assert(std::same_as<create_dimensions_t<Dimension<distance, 1>, Dimension<distance, 1>>,
                           create_dimensions_t<Dimension<distance, 2>>>);

// multiply
static_assert(std::same_as<EmptyDimensions::multiply_t<EmptyDimensions>, EmptyDimensions>);
static_assert(std::same_as<Distance::multiply_t<Distance::negate_t>, EmptyDimensions>);
static_assert(std::same_as<Distance::multiply_t<Distance>, Dimensions<Dimension<distance, 2>>>);
static_assert(std::same_as<Distance::multiply_t<Distance>::multiply_t<Distance>, Dimensions<Dimension<distance, 3>>>);
static_assert(std::same_as<Distance::multiply_t<Time>, Dimensions<Dimension<distance, 1>, Dimension<time, 1>>>);
static_assert(std::same_as<Distance::multiply_t<Time>, Time::multiply_t<Distance>>);
static_assert(std::same_as<Distance::multiply_t<Time::negate_t>, Speed>);
static_assert(std::same_as<Time::negate_t::multiply_t<Distance>, Speed>);
static_assert(std::same_as<Distance::multiply_t<Time>::multiply_t<Distance>,
                           Dimensions<Dimension<distance, 2>, Dimension<time, 1>>>);
static_assert(std::same_as<Distance::negate_t::multiply_t<Time>::multiply_t<Distance>, Dimensions<Dimension<time, 1>>>);
static_assert(std::same_as<Speed::multiply_t<Speed>, Dimensions<Dimension<distance, 2>, Dimension<time, -2>>>);
static_assert(std::same_as<Speed::multiply_t<Speed::negate_t>, EmptyDimensions>);

// Divide
static_assert(std::same_as<EmptyDimensions::divide_t<EmptyDimensions>, EmptyDimensions>);
static_assert(std::same_as<Distance::divide_t<Distance>, EmptyDimensions>);
static_assert(std::same_as<Distance::divide_t<Distance>::divide_t<Distance>, Dimensions<Dimension<distance, -1>>>);
static_assert(std::same_as<Speed::divide_t<Speed>, EmptyDimensions>);
static_assert(std::same_as<Distance::divide_t<Time>, Speed>);
static_assert(std::same_as<Time::divide_t<Distance>, Dimensions<Dimension<distance, -1>, Dimension<time, 1>>>);

}  // namespace twig
