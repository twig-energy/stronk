# Project is configured with no languages, so tell GNUInstallDirs the lib dir
set(CMAKE_INSTALL_LIBDIR
    lib
    CACHE PATH ""
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Install headers under: <prefix>/include/stronk-<version>/
if (PROJECT_IS_TOP_LEVEL)
    set(CMAKE_INSTALL_INCLUDEDIR
        "include/stronk-${PROJECT_VERSION}"
        CACHE PATH ""
    )
endif ()

# find_package(stronk)
set(package stronk)

# Generate version file
write_basic_package_version_file("${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake" COMPATIBILITY SameMajorVersion ARCH_INDEPENDENT)

# Allow package maintainers to override config location
set(stronk_INSTALL_CMAKEDIR
    "${CMAKE_INSTALL_DATADIR}/${package}"
    CACHE PATH "CMake package config location relative to the install prefix"
)

mark_as_advanced(stronk_INSTALL_CMAKEDIR)

# Install target + exported usage requirements
install(
    TARGETS twig_stronk
    EXPORT stronkTargets
    FILE_SET HEADERS
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    INCLUDES
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT stronk_Development
)

# Install package config
install(
    FILES cmake/install-config.cmake
    DESTINATION "${stronk_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT stronk_Development
)

# Install package version file
install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${stronk_INSTALL_CMAKEDIR}"
    COMPONENT stronk_Development
)

# Install exported targets
install(
    EXPORT stronkTargets
    FILE stronkTargets.cmake
    NAMESPACE twig::
    DESTINATION "${stronk_INSTALL_CMAKEDIR}"
    COMPONENT stronk_Development
)

if (PROJECT_IS_TOP_LEVEL)
    include(CPack)
endif ()
