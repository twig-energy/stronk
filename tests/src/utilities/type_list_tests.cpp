#include <stronk/utilities/type_list.h>

namespace twig
{
// Type list tests can mostly just be static asserts :)

// example
using TypeListExample = TypeList<int32_t, int64_t, int32_t, double, int32_t>;

// ============
// Add
// ============

static_assert(std::is_same_v<typename TypeListExample::add_t<int32_t>,
                             TypeList<int32_t, int64_t, int32_t, double, int32_t, int32_t>>);

// ============
// Remove First
// ============
static_assert(
    std::is_same_v<typename TypeListExample::remove_first_t<int32_t>, TypeList<int64_t, int32_t, double, int32_t>>);
static_assert(
    std::is_same_v<typename TypeListExample::remove_first_t<double>, TypeList<int32_t, int64_t, int32_t, int32_t>>);

// ============
// Remove All
// ============
static_assert(std::is_same_v<typename TypeListExample::remove_all_t<int32_t>, TypeList<int64_t, double>>);

// ============
// Contains
// ============
static_assert(TypeListExample::contains<int32_t>());
static_assert(!TypeListExample::contains<float>());

// ============
// Concat
// ============
static_assert(std::is_same_v<typename TypeListExample::concat_t<TypeList<int32_t, float>>,
                             TypeList<int32_t, int64_t, int32_t, double, int32_t, int32_t, float>>);

// ============
// Subtract
// ============
static_assert(std::is_same_v<typename TypeListExample::subtract_t<TypeList<int32_t>>,
                             TypeList<int64_t, int32_t, double, int32_t>>);

static_assert(std::is_same_v<typename TypeListExample::subtract_t<TypeList<int32_t, double>>,
                             TypeList<int64_t, int32_t, int32_t>>);

static_assert(std::is_same_v<typename TypeListExample::subtract_t<TypeList<int32_t, double, int32_t>>,
                             TypeList<int64_t, int32_t>>);

// ===========
// Concat Sorted (concat two sorted lists into one new sorted lists)
// ===========
static_assert(
    std::is_same_v<typename TypeList<int32_t>::concat_sorted_t<TypeList<int32_t>>, TypeList<int32_t, int32_t>>);

static_assert(std::is_same_v<typename TypeList<int32_t>::concat_sorted_t<TypeList<float>>, TypeList<float, int32_t>>);

static_assert(std::is_same_v<typename TypeList<float>::concat_sorted_t<TypeList<int32_t>>, TypeList<float, int32_t>>);

static_assert(std::is_same_v<typename TypeList<float, int32_t>::concat_sorted_t<TypeList<int32_t>>,
                             TypeList<float, int32_t, int32_t>>);
static_assert(std::is_same_v<typename TypeList<float>::concat_sorted_t<TypeList<int32_t, int32_t>>,
                             TypeList<float, int32_t, int32_t>>);

static_assert(std::is_same_v<typename TypeList<float, int32_t>::concat_sorted_t<TypeList<float>>,
                             TypeList<float, float, int32_t>>);
static_assert(std::is_same_v<typename TypeList<float>::concat_sorted_t<TypeList<float, int32_t>>,
                             TypeList<float, float, int32_t>>);

static_assert(std::is_same_v<typename TypeList<float, int32_t>::concat_sorted_t<TypeList<float, int32_t>>,
                             TypeList<float, float, int32_t, int32_t>>);

static_assert(std::is_same_v<typename TypeList<double>::concat_sorted_t<TypeList<float>>, TypeList<double, float>>);

static_assert(std::is_same_v<typename TypeList<double, int32_t>::concat_sorted_t<TypeList<float, int32_t>>,
                             TypeList<double, float, int32_t, int32_t>>);

static_assert(
    std::is_same_v<typename TypeList<double, int32_t, int32_t>::concat_sorted_t<TypeList<float, float, int32_t>>,
                   TypeList<double, float, float, int32_t, int32_t, int32_t>>);

}  // namespace twig
