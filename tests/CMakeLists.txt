cmake_minimum_required(VERSION 3.28)

project(stronkTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Dependencies ----
if (PROJECT_IS_TOP_LEVEL)
    find_package(stronk REQUIRED)
    enable_testing()
endif ()

find_package(GTest CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(glaze CONFIG REQUIRED)
find_package(absl CONFIG REQUIRED)
find_package(doctest CONFIG REQUIRED)

# ---- Tests ----
add_executable(
    stronk_test
    src/extensions/absl_tests.cpp
    src/extensions/doctest_tests.cpp
    src/extensions/fmt_tests.cpp
    src/extensions/glaze_tests.cpp
    src/extensions/gtest_tests.cpp
    src/extensions/nlohmann_json_tests.cpp
    src/main.cpp
    src/prefabs/stronk_arithmetic_tests.cpp
    src/prefabs/stronk_flag_tests.cpp
    src/prefabs/stronk_string_tests.cpp
    src/prefabs/stronk_vector_tests.cpp
    src/skills/can_abs_tests.cpp
    src/skills/can_decrement_tests.cpp
    src/skills/can_divide_tests.cpp
    src/skills/can_format_tests.cpp
    src/skills/can_hash_tests.cpp
    src/skills/can_increment_tests.cpp
    src/skills/can_index_tests.cpp
    src/skills/can_isnan_tests.cpp
    src/skills/can_iterate_tests.cpp
    src/skills/can_multiply_tests.cpp
    src/skills/can_stream_tests.cpp
    src/specializers_tests.cpp
    src/stronk_tests.cpp
    src/unit_tests.cpp
    src/utilities/dimensions_tests.cpp
    src/utilities/ratio_tests.cpp
)

target_link_libraries(
    stronk_test
    PRIVATE absl::hash
            doctest::doctest
            fmt::fmt
            glaze::glaze
            nlohmann_json::nlohmann_json
            twig::stronk
)
target_compile_features(stronk_test PRIVATE cxx_std_20)
target_compile_options(
    stronk_test
    PRIVATE # right now we don't know how to conversions inside units nicely
            $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
            -Wno-implicit-int-float-conversion
            -Wno-sign-conversion
            -Wno-implicit-int-conversion>
)

add_test(NAME stronk_test COMMAND stronk_test "--gtest_color=yes" "--gtest_output=xml:")

# ---- End-of-file commands ----
add_folders(Tests)

# ---- Code coverage ----
if (ENABLE_COVERAGE)
    target_code_coverage(
        stronk_test
        ALL
        EXCLUDE
        src
    )
endif ()
