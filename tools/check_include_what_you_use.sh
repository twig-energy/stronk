#!/bin/bash

# find recursively which folder starting from build, contains the compile_commands.json file
export BUILD_FOLDER=$(find build -name compile_commands.json -printf "%h\n" | head -n 1)

git diff --diff-filter=d --name-only origin/main | grep -E "\.h$" | xargs -r -t -n 1 -P 8 -- clang-tidy-21 --fix --checks="-*,misc-include-cleaner" -p $BUILD_FOLDER
git diff --diff-filter=d --name-only origin/main | grep -E "\.hpp$" | xargs -r -t -n 1 -P 8 -- clang-tidy-21 --fix --checks="-*,misc-include-cleaner" -p $BUILD_FOLDER
git diff --diff-filter=d --name-only origin/main | grep -E "\.cpp$" | xargs -r -t -n 1 -P 8 -- clang-tidy-21 --fix --checks="-*,misc-include-cleaner" -p $BUILD_FOLDER

# Format after fixing
git diff --diff-filter=d --name-only origin/main | grep -E "\.h$" | xargs -I{} -t -n 1 -P 8 sh -c 'cmake -D FIX=YES -D PATTERNS="{}" -D FORMAT_COMMAND=clang-format-21 -P cmake/lint.cmake'
git diff --diff-filter=d --name-only origin/main | grep -E "\.hpp$" | xargs -I{} -t -n 1 -P 8 sh -c 'cmake -D FIX=YES -D PATTERNS="{}" -D FORMAT_COMMAND=clang-format-21 -P cmake/lint.cmake'
git diff --diff-filter=d --name-only origin/main | grep -E "\.cpp$" | xargs -I{} -t -n 1 -P 8 sh -c 'cmake -D FIX=YES -D PATTERNS="{}" -D FORMAT_COMMAND=clang-format-21 -P cmake/lint.cmake'

echo "Checking diff after clang-tidy-21"
git --no-pager diff --quiet || exit 1
