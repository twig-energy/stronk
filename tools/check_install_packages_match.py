"""
Check that the packages defined by find_package matches the ones in cmake/install-config.cmake
These are used by downstream dependencies and therefore helps users install the library.
"""


def extract_components(
    dependency_line: str, dependecy_to_components: dict[str, list[str]]
) -> None:
    words = dependency_line[
        dependency_line.find("(") + 1 : dependency_line.rfind(")")
    ].split(" ")
    keywords = ["REQUIRED", "CONFIG"]
    dependency = words[0]
    in_component_list = False
    for word in words[1:]:
        word = word.strip()
        if word == "COMPONENTS":
            in_component_list = True
            continue

        if in_component_list:
            if word in keywords:
                break

            dependecy_to_components[dependency].append(word)


# %%
def main() -> None:
    """
    Check that the packages defined by find_package matches the ones in cmake/install-config.cmake
    """
    dependency_str_lines: list[str] = []

    with open("CMakeLists.txt", "r", encoding="utf-8") as f:
        for line in f:
            if line.startswith("find_package("):
                dependency_str_lines.append(line)

    dependencies = [line.split("(")[1].split(" ")[0] for line in dependency_str_lines]
    dependency_components = {x: [] for x in dependencies}
    for dependency_line in dependency_str_lines:
        extract_components(dependency_line, dependency_components)

    install_dependency_str_lines = []
    with open("cmake/install-config.cmake", "r", encoding="utf-8") as f:
        for line in f:
            if line.startswith("find_dependency("):
                install_dependency_str_lines.append(line)

    install_dependencies = [
        line.split("(")[1].split(")")[0].split(" ")[0]
        for line in install_dependency_str_lines
    ]
    install_dependency_components = {x: [] for x in install_dependencies}
    for dependency_line in install_dependency_str_lines:
        extract_components(dependency_line, install_dependency_components)

    assert set(install_dependencies) == set(dependencies), (
        f"Dependencies in find_package and install-config.cmake do not match.\n"
        f"find_package: {dependencies}\n"
        f"install-config.cmake: {install_dependencies}"
    )

    assert install_dependency_components == dependency_components, (
        f"Component Dependencies in find_package and install-config.cmake do not match.\n"
        f"find_package: {dependency_components}\n"
        f"install-config.cmake: {install_dependency_components}"
    )

    print(
        "Dependencies in find_package and install-config.cmake match."
        f"find_package: {dependencies}\n"
        f"install-config.cmake: {install_dependencies}"
    )


if __name__ == "__main__":
    main()
