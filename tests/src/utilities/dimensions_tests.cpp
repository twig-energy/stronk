#include <concepts>

#include <stronk/utilities/dimensions.h>

namespace twig::details
{

struct distance
{
};

struct time
{
};

using Distance = dimensions<dimension<distance, 1>>;
using Time = dimensions<dimension<time, 1>>;
using Speed = dimensions<dimension<distance, 1>, dimension<time, -1>>;

static_assert(std::same_as<create_dimensions_t<>, empty_dimensions>);
static_assert(std::same_as<create_dimensions_t<dimension<distance, 1>>, Distance>);
static_assert(std::same_as<create_dimensions_t<dimension<distance, 1>, dimension<time, -1>>, Speed>);
static_assert(std::same_as<create_dimensions_t<dimension<distance, 1>, dimension<time, -1>>,
                           create_dimensions_t<dimension<time, -1>, dimension<distance, 1>>>);
static_assert(std::same_as<create_dimensions_t<dimension<distance, 1>, dimension<distance, 1>>,
                           create_dimensions_t<dimension<distance, 2>>>);

// multiply
static_assert(std::same_as<empty_dimensions::multiply_t<empty_dimensions>, empty_dimensions>);
static_assert(std::same_as<Distance::multiply_t<Distance::negate_t>, empty_dimensions>);
static_assert(std::same_as<Distance::multiply_t<Distance>, dimensions<dimension<distance, 2>>>);
static_assert(std::same_as<Distance::multiply_t<Distance>::multiply_t<Distance>, dimensions<dimension<distance, 3>>>);
static_assert(std::same_as<Distance::multiply_t<Time>, dimensions<dimension<distance, 1>, dimension<time, 1>>>);
static_assert(std::same_as<Distance::multiply_t<Time>, Time::multiply_t<Distance>>);
static_assert(std::same_as<Distance::multiply_t<Time::negate_t>, Speed>);
static_assert(std::same_as<Time::negate_t::multiply_t<Distance>, Speed>);
static_assert(std::same_as<Distance::multiply_t<Time>::multiply_t<Distance>,
                           dimensions<dimension<distance, 2>, dimension<time, 1>>>);
static_assert(std::same_as<Distance::negate_t::multiply_t<Time>::multiply_t<Distance>, dimensions<dimension<time, 1>>>);
static_assert(std::same_as<Speed::multiply_t<Speed>, dimensions<dimension<distance, 2>, dimension<time, -2>>>);
static_assert(std::same_as<Speed::multiply_t<Speed::negate_t>, empty_dimensions>);

// Divide
static_assert(std::same_as<empty_dimensions::divide_t<empty_dimensions>, empty_dimensions>);
static_assert(std::same_as<Distance::divide_t<Distance>, empty_dimensions>);
static_assert(std::same_as<Distance::divide_t<Distance>::divide_t<Distance>, dimensions<dimension<distance, -1>>>);
static_assert(std::same_as<Speed::divide_t<Speed>, empty_dimensions>);
static_assert(std::same_as<Distance::divide_t<Time>, Speed>);
static_assert(std::same_as<Time::divide_t<Distance>, dimensions<dimension<distance, -1>, dimension<time, 1>>>);

}  // namespace twig::details
