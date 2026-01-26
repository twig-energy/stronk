include(CMakeFindDependencyMacro)

find_dependency(Boost COMPONENTS type_index)

include("${CMAKE_CURRENT_LIST_DIR}/stronkTargets.cmake")
