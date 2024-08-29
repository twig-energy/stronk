cmake_minimum_required(VERSION 3.14)

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
    src/can_decrement_tests.cpp
    src/can_format_tests.cpp
    src/can_hash_tests.cpp
    src/can_increment_tests.cpp
    src/can_stream_tests.cpp
    src/extensions/absl_tests.cpp
    src/extensions/doctest_tests.cpp
    src/extensions/fmt_tests.cpp
    src/extensions/glaze_tests.cpp
    src/extensions/gtest_tests.cpp
    src/extensions/nlohmann_json_tests.cpp
    src/specializers_tests.cpp
    src/stronk_tests.cpp
    src/unit_tests.cpp
    src/utilities/dimensions_tests.cpp
)

target_link_libraries(
    stronk_test
    PRIVATE twig::stronk
            absl::hash
            doctest::doctest
            fmt::fmt
            glaze::glaze
            GTest::gtest_main
            nlohmann_json::nlohmann_json
)
target_compile_features(stronk_test PRIVATE cxx_std_20)

add_test(NAME stronk_test COMMAND stronk_test "--gtest_color=yes" "--gtest_output=xml:")

# ---- End-of-file commands ----
add_folders(Tests)
