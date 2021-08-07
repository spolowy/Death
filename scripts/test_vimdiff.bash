#!/bin/bash

# test_vimdiff.bash
#
#     compile_mode: specifies Makefile's compilation rule (re / debug).
#     bin_path:     binary path to perform vimdiff on.
#
# Compile virus as <compile_mode> (no default), makes 1 copy of <bin_path>
# in the testing directory (/tmp/test), runs the virus, makes a seconds copy of
# <bin_path>, runs the first infected copy, performs and hexdump of both copies
# and vimdiff against eachother.
# Cleanup hexdump files after quitting vimdiff.

if [ $# != 2 ]; then
	echo "usage: $0 compile_mode bin_path"
	exit 1
fi

compile_mode="$1"
bin_path="$2"

function go_to_script_directory
{
	parent_path=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
	cd "$parent_path"
}

function compile
{
	rm -rf /tmp/test/*
	rm -rf /tmp/test2/*
	mkdir -p /tmp/test /tmp/test2
	make "$compile_mode" -C ..
}

function diff
{
	bin_name=$(basename "$bin_path")
	cp -f "$bin_path" /tmp/test/"$bin_name"1
	../death
	cp -f "$bin_path" /tmp/test/"$bin_name"2
	/tmp/test/"$bin_name"1
	xxd /tmp/test/"$bin_name"1 > "$bin_name"1.x
	xxd /tmp/test/"$bin_name"2 > "$bin_name"2.x
	vimdiff "$bin_name"1.x "$bin_name"2.x
}

function cleanup
{
	rm -f "$bin_name"1.x "$bin_name"2.x
}

go_to_script_directory
compile
diff
cleanup
