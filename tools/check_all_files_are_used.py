"""
Script for checking that all c++ files are used in the project.
"""

import sys
from glob import glob
from pathlib import Path

REPO_ROOT = Path(__file__).parent.parent.absolute()

projects = [
    REPO_ROOT,
]


def get_all_include_files(project: Path):
    """
    Returns a list of all the header files in the include directory
    """
    return [
        Path(x).absolute()
        for x in glob(f"{project}/include/**/*.hpp", recursive=True)
    ]


def get_all_source_files(project: Path):
    """
    Returns a list of all the source files in the src directory
    """
    return [Path(x).absolute() for x in glob(f"{project}/source/**/*.cpp", recursive=True)]


def parse_target_sources_in_cmake(project: Path):
    """
    Returns a list of all the source files, and header files defined in the target_sources of the main library component
    """
    keywords = [
        "PRIVATE",
        "PUBLIC",
        "INTERFACE",
        "FILE_SET",
        "TYPE",
        "BASE_DIRS",
        "FILES",
    ]
    sources = []
    headers = []
    with open(project /"CMakeLists.txt", "r", encoding="utf-8") as f:
        state = "KEYWORDS"
        text = f.read()

        while "target_sources(" in text:
            start = text.find("target_sources(")
            end = text.find(")", start)
            sources_text = text[start:end]

            for word in sources_text.split(" "):
                word = word.strip()
                if word == "":
                    continue
                if word in keywords:
                    state = word
                elif state == "PRIVATE":
                    sources.append((project / word).absolute())
                elif state == "FILES":
                    headers.append((project / word).absolute())

            text= text[end:]

    return sources, headers


def main():
    """
    Checks that all files in the include directory and all the sources in the src directory are referred to in the CMakeLists.txt
    """
    
    total_include_files = 0
    total_cmake_includes = 0


    for project in projects:
        print(f"Checking project {project}")
        include_files = get_all_include_files(project)
        source_files = get_all_source_files(project)
        cmake_sources, cmake_includes = parse_target_sources_in_cmake(project)
    
        missing_includes = []
        for x in include_files:
            if x not in cmake_includes:
                missing_includes.append(str(x.relative_to(project)))
        assert len(missing_includes) == 0, f"The following header files were not present in CMakeLists.txt: {missing_includes}"

        missing_sources = []
        for x in source_files:
            if x not in cmake_sources:
                missing_sources.append(str(x.relative_to(project)))
        assert len(missing_sources) == 0, f"The following source files were not present in CMakeLists.txt: {missing_sources}"

        total_include_files += len(include_files)
        total_cmake_includes += len(cmake_includes)

    assert total_include_files > 0, "No include files found"
    assert total_cmake_includes > 0, "No include files found in CMakeLists.txt"

    print("All files library files are in file_sets")

    return 0


if __name__ == "__main__":
    sys.exit(main())
