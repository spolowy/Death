#!/bin/bash

red='\033[31m'
green='\033[32m'
yellow='\033[33m'
none='\033[0m'

self="$0"

if [ $# -ne 2 ]; then
	printf "${yellow}usage${none}: $self [compile_mode] [bin_path]\n"
	exit 1
fi

compile_mode="$1"
bin_path="$2"
germ='../death'

function	go_to_script_directory
{
	parent_path=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
	cd "$parent_path"
}

function	compile
{
	rm -rf /tmp/test/*
	rm -rf /tmp/test2/*
	mkdir -p /tmp/test /tmp/test2
	make "$compile_mode" -C ..
}

function	diff
{
	bin_name=$(basename "$bin_path")
	cp -f "$bin_path" /tmp/test/"$bin_name"1
	$germ
	cp -f "$bin_path" /tmp/test/"$bin_name"2
	/tmp/test/"$bin_name"1
	xxd /tmp/test/"$bin_name"1 > "$bin_name"1.x
	xxd /tmp/test/"$bin_name"2 > "$bin_name"2.x
	vimdiff "$bin_name"1.x "$bin_name"2.x
}

function	cleanup
{
	rm -f "$bin_name"1.x "$bin_name"2.x
}

function	start
{
	go_to_script_directory

	if ! compile || ! diff || ! cleanup
	then
		printf "${red}error${none}: $self an error has occured.\n"
	fi
}

start
