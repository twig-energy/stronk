"""
compile_docs parses the code in the tests folder,
  gets the text in the tests and then injects them into all the .md files in this directory
"""

from asyncore import file_dispatcher
from dataclasses import dataclass
from io import TextIOWrapper
import itertools
from nis import cat
import sys
from pathlib import Path
from typing import Dict, List
import json


def remove_leading_spaces(code_string: str):
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
                    line_end=-1) -> str:
    code = ""
    for line in file_handle.readlines()[line_start:line_end]:
        code += line
    return remove_leading_spaces(code)


@dataclass(unsafe_hash=True)
class Section:
    language: str
    file: Path
    code_file_from: int
    code_file_to: int

    def is_valid_section(code_section_start_line: str):
        stripped = code_section_start_line.strip()
        stripped = stripped[stripped.find("```") + 3:]
        return len(stripped.split(":")) >= 2

    @staticmethod
    def parse_section(code_section_start_line: str):
        stripped = code_section_start_line.strip()
        stripped = stripped[stripped.find("```") + 3:]
        properties = stripped.split(":")
        return Section(
            language=properties[0],
            file=Path(properties[1]),
            code_file_from=properties[2] if len(properties) >= 3 else 0,
            code_file_to=properties[3] if len(properties) >= 4 else -1)


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
        try:
            section = Section.parse_section(lines[section_start])
            assert Path(section.file).exists(), section.file
            res.append(section)
        except Exception:
            print(f"Could not parse code section: {lines[section_start]}")
    return res


def load_code_sections(snippets: List[Section]) -> Dict[Section, str]:
    res = {}
    for snippet in snippets:
        with open(snippet.file) as fp:
            res[snippet] = parse_code_file(fp, snippet.code_file_from,
                                           snippet.code_file_to)
    return res


def embed_data_in_docs(files_to_embed_into: Path, files_to_code: Dict[Section,
                                                                      str]):
    with open(files_to_embed_into) as fp:
        lines = fp.readlines()

    res = ""
    last_index = 0
    for section_start, section_end in find_code_sections(lines):
        for line in lines[last_index:section_start + 1]:
            res += line
        section = Section.parse_section(lines[section_start])
        res += files_to_code[section]
        res += "```"
        last_index = section_end
    return res


def parse_arguments():
    import argparse
    parser = argparse.ArgumentParser(description='Embed code into readme')
    parser.add_argument("-f", "--file", type=Path, default=Path("./README.md"))
    return parser.parse_args()


def run():
    args = parse_arguments()
    file_to_embed_into = args.file
    sections_to_load = parse_markdown(file_to_embed_into)
    files_to_code = load_code_sections(sections_to_load)
    new_file = embed_data_in_docs(file_to_embed_into, files_to_code)
    print(new_file)
    return 0


if __name__ == "__main__":
    exit(run())
