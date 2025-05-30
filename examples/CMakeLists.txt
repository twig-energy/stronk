cmake_minimum_required(VERSION 3.28)

project(stronkExamples CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

if (PROJECT_IS_TOP_LEVEL)
    find_package(stronk REQUIRED)
endif ()

add_custom_target(run-examples)

function (add_example NAME)
    add_executable("${NAME}" "${NAME}.cpp")
    target_link_libraries("${NAME}" PRIVATE twig::stronk)
    target_compile_features("${NAME}" PRIVATE cxx_std_20)
    target_compile_options(
        "${NAME}"
        PRIVATE $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
                -Wno-unused-variable
                -Wno-implicit-int-float-conversion
                -Wno-unused-but-set-variable>
    )

    add_custom_target(
        "run_${NAME}"
        COMMAND "${NAME}"
        VERBATIM
    )
    add_dependencies("run_${NAME}" "${NAME}")
    add_dependencies(run-examples "run_${NAME}")

endfunction ()

add_example(unit_example)
add_example(firstname_lastname_example)
add_example(unit_energy_example)
add_example(specializers_example)

add_folders(Examples)
