"""
  Parse markdown document and embed the referenced code into code blocks

  Example syntax for code blocks:
    ```cpp:file=./examples/unit_energy_example.cpp:line_start=0:line_end=29
    ```
"""

from asyncore import file_dispatcher
from dataclasses import dataclass
from io import TextIOWrapper
import itertools
from nis import cat
import sys
from pathlib import Path
from typing import Dict, List, Optional
import json


def remove_leading_spaces(code_string: str) -> str:
    lines = code_string.splitlines()
    result = ""
    min_leading = sys.maxsize
    for line in lines:
        if len(line) != 0:
            min_leading = min(min_leading, len(line) - len(line.lstrip()))

    for line in lines:
        result += line[min_leading:] + "\n"

    return result


def parse_code_file(file_handle: TextIOWrapper,
                    line_start=0,
                    line_end=None) -> str:
    code = ""
    for line in file_handle.readlines()[line_start:line_end]:
        code += line
    return remove_leading_spaces(code)


@dataclass(unsafe_hash=True)
class Section:
    language: str
    file: Path
    line_start: int = 0
    line_end: Optional[int] = None

    def __post_init__(self):
        self.file = Path(self.file)
        self.line_start = int(self.line_start)
        self.line_end = int(self.line_end) if self.line_end else None

    @staticmethod
    def is_valid_section(code_section_start_line: str):
        stripped = code_section_start_line.strip()
        stripped = stripped[stripped.find("```") + 3:]
        return len(stripped.split(":")) >= 2 and "file=" in stripped

    @staticmethod
    def parse_section(code_section_start_line: str):
        stripped = code_section_start_line.strip()
        stripped = stripped[stripped.find("```") + 3:]
        properties = stripped.split(":")
        keyword_properties = {
            x.split("=")[0]: x.split("=")[1]
            for x in properties[1:]
        }
        return Section(language=properties[0], **keyword_properties)


def find_code_sections(lines: List[str]):
    section = []
    for number, line in enumerate(lines):
        if line.strip().startswith("```"):
            section.append(number)
            if len(section) == 2:
                if Section.is_valid_section(lines[section[0]]):
                    yield (section[0], section[1])
                section.clear()


def parse_markdown(file: Path) -> List[Section]:
    res = []
    with open(file) as fp:
        lines = fp.readlines()

    sections = list(find_code_sections(lines))

    for section_start, _ in sections:
        section = Section.parse_section(lines[section_start])
        assert section.file.exists(
        ), f"Failed to parse section: {lines[section_start]}: '{section.file.absolute()}' did not exist"
        res.append(section)
    return res


def load_code_sections(snippets: List[Section]) -> Dict[Section, str]:
    res = {}
    for snippet in snippets:
        with open(snippet.file) as fp:
            res[snippet] = parse_code_file(fp, snippet.line_start,
                                           snippet.line_end)
    return res


def embed_data_in_docs(files_to_embed_into: Path,
                       files_to_code: Dict[Section, str]) -> str:
    with open(files_to_embed_into) as fp:
        lines = fp.readlines()

    res = ""
    last_index = 0
    for section_start, section_end in find_code_sections(lines):
        for line in lines[last_index:section_start + 1]:
            res += line
        section = Section.parse_section(lines[section_start])
        assert section in files_to_code, files_to_code
        res += files_to_code[section]
        res += "```\n"
        last_index = section_end + 1

    # Add the remaining of the file
    for line in lines[last_index:]:
        res += line
    return res


def output_result(file_to_embed_into: Path, result_str: str, inline: bool):
    if inline:
        with open(file_to_embed_into, 'w') as fp:
            fp.write(result_str)
    else:
        print(result_str)


def parse_arguments():
    import argparse
    parser = argparse.ArgumentParser(description='Embed code into readme')
    parser.add_argument("-f", "--file", type=Path)
    parser.add_argument('-i', '--inline', action='store_true')
    return parser.parse_args()


def run():
    args = parse_arguments()
    assert args.file.exists()
    sections_to_load = parse_markdown(args.file)
    files_to_code = load_code_sections(sections_to_load)
    new_file_str = embed_data_in_docs(args.file, files_to_code)
    output_result(args.file, new_file_str, args.inline)
    return 0


if __name__ == "__main__":
    exit(run())
