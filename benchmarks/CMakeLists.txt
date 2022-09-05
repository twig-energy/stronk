cmake_minimum_required(VERSION 3.14)

project(stronkBenchmarks LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Dependencies ----
if(PROJECT_IS_TOP_LEVEL)
  find_package(stronk REQUIRED)
endif()

find_package(benchmark CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(absl CONFIG REQUIRED)

# ---- Benchmarks ----
add_executable(stronk_benchmarks
  src/construction_benchmarks.cpp
  src/unit_benchmarks.cpp
)

target_link_libraries(
  stronk_benchmarks
  PRIVATE
  twig::stronk
  absl::hash
  fmt::fmt
  benchmark::benchmark_main
)
target_compile_features(stronk_benchmarks PRIVATE cxx_std_20)

# ---- End-of-file commands ----
add_folders(Benchmarks)
