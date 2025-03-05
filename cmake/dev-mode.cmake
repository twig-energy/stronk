include(cmake/folders.cmake)

if (NOT stronk_DEVELOPER_MODE)
    return()
elseif (NOT PROJECT_IS_TOP_LEVEL)
    message(AUTHOR_WARNING "Developer mode is intended for developers of stronk")
endif ()

option(ENABLE_COVERAGE "Enable coverage support separate from CTest's" OFF)
if (ENABLE_COVERAGE)
    include(cmake/code-coverage.cmake)
    add_code_coverage_all_targets(
        EXCLUDE
        tests/*
        examples/*
        benchmarks/*
    )
endif ()

include(CTest)

if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    include(cmake/open-cpp-coverage.cmake OPTIONAL)
endif ()

include(cmake/lint-targets.cmake)
include(cmake/spell-targets.cmake)

add_folders(Project)
