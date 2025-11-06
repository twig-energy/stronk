set(FORMAT_PATTERNS
    source/*.cpp
    source/*.hpp
    source/*.h
    src/*.cpp
    src/*.hpp
    src/*.h
    include/*.hpp
    include/*.h
    tests/*.cpp
    tests/*.hpp
    tests/*.h
    examples/*.cpp
    examples/*.hpp
    examples/*.h
    benchmarks/*.cpp
    benchmarks/*.hpp
    benchmarks/*.h
    CACHE STRING "; separated patterns relative to the project source dir to format"
)

set(FORMAT_COMMAND
    clang-format-21
    CACHE STRING "Formatter to use"
)

add_custom_target(
    format-check
    COMMAND "${CMAKE_COMMAND}" -D "FORMAT_COMMAND=${FORMAT_COMMAND}" -D "PATTERNS=${FORMAT_PATTERNS}" -P "${PROJECT_SOURCE_DIR}/cmake/lint.cmake"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    COMMENT "Linting the code"
    VERBATIM
)

add_custom_target(
    format-fix
    COMMAND "${CMAKE_COMMAND}" -D "FORMAT_COMMAND=${FORMAT_COMMAND}" -D "PATTERNS=${FORMAT_PATTERNS}" -D FIX=YES -P "${PROJECT_SOURCE_DIR}/cmake/lint.cmake"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    COMMENT "Fixing the code"
    VERBATIM
)
