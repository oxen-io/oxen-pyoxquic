#!/usr/bin/env bash

CLANG_FORMAT_DESIRED_VERSION=15

binary=$(command -v clang-format-$CLANG_FORMAT_DESIRED_VERSION 2>/dev/null)
if [ $? -ne 0 ]; then
    binary=$(command -v clang-format-mp-$CLANG_FORMAT_DESIRED_VERSION 2>/dev/null)
fi
if [ $? -ne 0 ]; then
    binary=$(command -v clang-format 2>/dev/null)
    if [ $? -ne 0 ]; then
        echo "Please install clang-format version $CLANG_FORMAT_DESIRED_VERSION and re-run this script."
        exit 1
    fi
    version=$(clang-format --version)
    if [[ ! $version == *"clang-format version $CLANG_FORMAT_DESIRED_VERSION"* ]]; then
        echo "Please install clang-format version $CLANG_FORMAT_DESIRED_VERSION and re-run this script."
        exit 1
    fi
fi

cd "$(dirname $0)/../"
readarray -t sources < <(find src | grep -E '\.([hc](pp)?)$' | grep -v '\#\|Catch2\|CLI11')
if [ "$1" = "verify" ] ; then
    if [ $($binary --output-replacements-xml "${sources[@]}"  | grep '</replacement>' | wc -l) -ne 0 ] ; then
        exit 2
    fi
else
    $binary -i "${sources[@]}" &> /dev/null
fi
