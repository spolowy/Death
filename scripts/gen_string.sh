#!/bin/bash

if [ $# != 1 ]; then
	echo "usage: $0 [string]"
	exit 1
fi

input="$1"
output=""

for (( i=0; i<${#input}; i++ )); do
	output+="'"
	output+="${input:$i:1}"
	output+="',"
done

echo "$output"
