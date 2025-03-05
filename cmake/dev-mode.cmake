include(cmake/folders.cmake)

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
