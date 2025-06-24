#include "./construction_benchmarks.hpp"
#include "./unit_benchmarks.hpp"

auto main() -> int
{
    run_default_onto_reserved_vector_benchmarks();
    run_rand_onto_reserved_vector_benchmarks();
    run_copy_vector_of_benchmarks();

    run_add_units_benchmarks();
    run_add_units_simd_benchmarks();
    run_subtract_units_benchmarks();
    run_subtract_units_simd_benchmarks();
    run_multiply_units_benchmarks();
    run_multiply_units_simd_benchmarks();
    run_divide_units_benchmarks();
    run_divide_units_simd_benchmarks();

    return 0;
}
