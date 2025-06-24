cmake_minimum_required(VERSION 3.28)

project(stronkBenchmarks LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Dependencies ----
if (PROJECT_IS_TOP_LEVEL)
    find_package(stronk REQUIRED)
endif ()

find_package(fmt CONFIG REQUIRED)
find_package(absl CONFIG REQUIRED)
find_package(nanobench CONFIG REQUIRED)

# ---- Benchmarks ----
add_executable(stronk_benchmarks src/main.cpp)
target_link_libraries(
    stronk_benchmarks
    PRIVATE absl::hash
            fmt::fmt
            nanobench::nanobench
            twig::stronk
)
target_compile_features(stronk_benchmarks PRIVATE cxx_std_20)
target_compile_options(stronk_benchmarks PRIVATE $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>: -Wno-conversion>)

# ---- End-of-file commands ----
add_folders(Benchmarks)
