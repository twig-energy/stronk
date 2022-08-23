cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    stronk
    VERSION 0.1.0
    DESCRIPTION "An easy to customize strong type library with built in support for unit-like behavior"
    HOMEPAGE_URL "https://github.com/twig-energy/stronk"
    LANGUAGES NONE
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(stronk_stronk INTERFACE)
add_library(stronk::stronk ALIAS stronk_stronk)

set_property(
    TARGET stronk_stronk PROPERTY
    EXPORT_NAME stronk
)

target_include_directories(
    stronk_stronk ${warning_guard}
    INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_compile_features(stronk_stronk INTERFACE cxx_std_20)

find_package(fmt REQUIRED)
target_link_libraries(stronk_stronk INTERFACE fmt::fmt)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Examples ----

if(PROJECT_IS_TOP_LEVEL)
  option(BUILD_EXAMPLES "Build examples tree." "${stronk_DEVELOPER_MODE}")
  if(BUILD_EXAMPLES)
    add_subdirectory(example)
  endif()
endif()

# ---- Developer mode ----

if(NOT stronk_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of stronk"
  )
endif()

include(cmake/dev-mode.cmake)
