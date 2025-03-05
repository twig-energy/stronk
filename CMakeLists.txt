cmake_minimum_required(VERSION 3.28)

include(cmake/prelude.cmake)

file(READ "${CMAKE_CURRENT_LIST_DIR}/version.py" version_line)
string(
    REGEX MATCH
          "__version__ = \"(.*)\""
          _
          ${version_line}
)
set(version ${CMAKE_MATCH_1})

project(
    stronk
    VERSION ${version}
    DESCRIPTION "An easy to customize strong type library with built in support for unit-like behavior"
    HOMEPAGE_URL "https://github.com/twig-energy/stronk"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)
include(cmake/dev-mode.cmake)

find_package(boost_type_index CONFIG REQUIRED)

# ---- Declare library ----
add_library(twig_stronk INTERFACE)
add_library(twig::stronk ALIAS twig_stronk)

set_property(TARGET twig_stronk PROPERTY EXPORT_NAME stronk)

target_include_directories(twig_stronk ${warning_guard} INTERFACE "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>")

target_compile_features(twig_stronk INTERFACE cxx_std_20)

target_link_libraries(twig_stronk INTERFACE Boost::type_index)

# ---- Install rules ----
if (NOT CMAKE_SKIP_INSTALL_RULES)
    include(cmake/install-rules.cmake)
endif ()

if (PROJECT_IS_TOP_LEVEL)
    option(BUILD_TESTING "Build tests tree." "${stronk_DEVELOPER_MODE}")
    if (BUILD_TESTING)
        add_subdirectory(tests)
    endif ()

    option(BUILD_EXAMPLES "Build examples tree." "${stronk_DEVELOPER_MODE}")
    if (BUILD_EXAMPLES)
        add_subdirectory(examples)
    endif ()

    option(BUILD_BENCHMARKS "Build benchmarks tree." "${stronk_DEVELOPER_MODE}")
    if (BUILD_BENCHMARKS)
        add_subdirectory(benchmarks)
    endif ()
endif ()

# ---- Developer mode ----
if (NOT stronk_DEVELOPER_MODE)
    return()
elseif (NOT PROJECT_IS_TOP_LEVEL)
    message(AUTHOR_WARNING "Developer mode is intended for developers of stronk")
endif ()

# ---- Code coverage ----
if (ENABLE_COVERAGE)
    target_code_coverage(twig_stronk ALL)
endif ()
