#!/bin/bash

if [ $# -ne 1 ]; then
	echo "usage: $0 [source_directory]"
	exit 1
fi

folder="$1"

rm -rf /tmp/test/*
rm -rf /tmp/test2/*

mkdir -p /tmp/test
mkdir -p /tmp/test2

cp "$folder"/* /tmp/test/
