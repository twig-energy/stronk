# Building and installing with CMake

## Dependencies

For a list of dependencies, please refer to [vcpkg.json](vcpkg.json).

## Build and installing
Make sure you have at least CMake 3.15 installed

### Build on Unix
building in release mode with a single-configuration
generator, like the Unix Makefiles one:

```sh
cmake -S . -B build -D CMAKE_BUILD_TYPE=Release
cmake --build build
```

Install the release mode artifacts
```sh
cmake --install build
```

### Build using Visual studio


Building in release mode with a multi-configuration
generator, like the Visual Studio ones:

```sh
cmake -S . -B build
cmake --build build --config Release
```

Install the release mode artifacts
```sh
cmake --install build --config Release
```

### Building on Apple Silicon

CMake supports building on Apple Silicon properly since 3.20.1. Make sure you
have the [latest version][1] installed and follow the Unix build and installation instruction.


### Building with MSVC

Note that MSVC by default is not standards compliant and you need to pass some
flags to make it behave properly. See the `flags-windows` preset in the
[CMakePresets.json](CMakePresets.json) file for the flags and with what
variable to provide them to CMake during configuration.

## CMake package

This project exports a CMake package to be used with the [`find_package`][3]
command of CMake:

* Package name: `stronk`
* Target name: `twig::stronk`

Example usage:

```cmake
find_package(stronk REQUIRED)
# Declare the imported target as a build requirement using PRIVATE, where
# project_target is a target created in the consuming project
target_link_libraries(
    project_target PRIVATE
    twig::stronk
)
```

[1]: https://cmake.org/download/
[2]: https://cmake.org/cmake/help/latest/manual/cmake.1.html#install-a-project
[3]: https://cmake.org/cmake/help/latest/command/find_package.html
