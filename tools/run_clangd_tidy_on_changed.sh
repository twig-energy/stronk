#!/bin/bash

# find recursively which folder starting from build, contains the compile_commands.json file
export BUILD_FOLDER=$(find build -name compile_commands.json -printf "%h\n" | head -n 1)

# you can add --enable-check-profile to see which checks are taking the most time

# if given argument "all" run on all files, otherwise only on diffs
if [ "$1" != "all" ]; then
    git diff --diff-filter=d --name-only origin/main | grep -E "\.(h|hpp|cpp)$" | xargs -r -t -- clangd-tidy -p $BUILD_FOLDER --clangd-executable=clangd-21 --github --tqdm
else
    find include source test example -type f | grep -E "\.(h|hpp|cpp)$" | xargs -r -t -- clangd-tidy -p $BUILD_FOLDER --clangd-executable=clangd-21 --github --tqdm
fi
