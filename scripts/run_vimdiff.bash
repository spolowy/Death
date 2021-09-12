#!/bin/bash

if [ $# != 2 ]; then
	echo "usage: $0 [target 1] [target 2]"
	exit 1
fi

target1_path="$1"
target2_path="$2"

# ---------------------------------------------------------------------------- #

function	diff
{
	local	target1=$(basename "$target1_path")
	local	target2=$(basename "$target2_path")

	xxd "$target1_path" > "$target1".x
	xxd "$target2_path" > "$target2".x
	vimdiff "$target1".x "$target2".x
	rm -f "$target1".x "$target2".x
}

# ---------------------------------------------------------------------------- #

function	start
{
	diff
}

start
