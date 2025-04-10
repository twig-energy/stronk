cmake_minimum_required(VERSION 3.28)

project(stronkBenchmarks LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Dependencies ----
if (PROJECT_IS_TOP_LEVEL)
    find_package(stronk REQUIRED)
endif ()

find_package(benchmark CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(absl CONFIG REQUIRED)

# ---- Benchmarks ----
add_executable(stronk_benchmarks src/construction_benchmarks.cpp src/unit_benchmarks.cpp)

target_link_libraries(
    stronk_benchmarks
    PRIVATE absl::hash
            benchmark::benchmark_main
            fmt::fmt
            twig::stronk
)
target_compile_features(stronk_benchmarks PRIVATE cxx_std_20)

# TODO(anders.wind) remove the following after benchmark 1.7.1 is released on vcpkg https://github.com/microsoft/vcpkg/issues/27673
target_compile_definitions(stronk_benchmarks PRIVATE BENCHMARK_STATIC_DEFINE)

# ---- End-of-file commands ----
add_folders(Benchmarks)
