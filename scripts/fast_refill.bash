#!/bin/bash

yellow='\033[33m'
none='\033[0m'

self="$0"

if [ $# -ne 1 ]; then
	printf "${yellow}usage${none}: $self [directory]\n"
	exit 1
fi

folder="$1"

rm -rf /tmp/test/*
rm -rf /tmp/test2/*

mkdir -p /tmp/test
mkdir -p /tmp/test2

cp "$folder"/* /tmp/test/
