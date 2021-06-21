#!/bin/bash

# fast_refill.bash
#     source_directory: directory to copy content from.
#
# Cleanup test directories and refill one of them (/tmp/test) with the content
# of the specified <source_directory>.

if [ $# != 1 ]; then
	echo "usage: $0 source_directory"
	exit 1
fi

folder="$1"

rm -rf /tmp/test/*
rm -rf /tmp/test2/*

mkdir -p /tmp/test
mkdir -p /tmp/test2

cp "$folder"/* /tmp/test/
