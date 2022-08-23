cmake_minimum_required(VERSION 3.14)

project(stronkTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Dependencies ----
if(PROJECT_IS_TOP_LEVEL)
  find_package(stronk REQUIRED)
  enable_testing()
endif()

find_package(GTest CONFIG REQUIRED)

# ---- Tests ----
add_executable(stronk_test
  src/stronk_tests.cpp
)

target_link_libraries(
  stronk_test PRIVATE
  stronk::stronk
  GTest::gtest_main
)
target_compile_features(stronk_test PRIVATE cxx_std_20)
add_test(NAME stronk_test COMMAND stronk_test "--gtest_color=yes" "--gtest_output=xml:")

# ---- End-of-file commands ----
add_folders(Test)
