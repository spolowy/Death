
#!/bin/bash

if [ $# != 1 ]; then
	echo "./refill.bash folder_to_copy"
	exit 1
fi

rm -rf /tmp/test/*
rm -rf /tmp/test2/*

mkdir -p /tmp/test
mkdir -p /tmp/test2

cp $1/* /tmp/test/
