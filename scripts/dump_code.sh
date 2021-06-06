#!/bin/bash

gdb="gdb -x ~/peda/peda.py"
bin='ptest'
bin_path="./$bin"
tmp_file='/tmp/dump_code'

# --------------------------------- Process ---------------------------------- #

function	start_process() # (void)
{
	local	regex='End of info.'
	local	output=""

	rm $tmp_file
	exec 3>$tmp_file                               # open writing fd
	exec 4<$tmp_file                               # open reading fd

	./ptest >&3 &                                  # run as background
	bin_pid="$!"

	while ! [[ $line =~ $regex ]]; do
		read <&4 line;
		if [[ $line == "" ]]; then
			continue
		fi
		output+="$line\n"
	done

	exec 3>&-                                      # close writing fd
	exec 4<&-                                      # close reading fd
	bin_output=$(echo -e "$output")
}

# --------------------------------- Get info --------------------------------- #

function	get_info # (const char *output)
{
	local	output=$(echo "$1" | sed -e 's/\x1B\[[0-9;]\{1,\}[A-Za-z]//g')

	# retrieve buffer addresses
	local	found_ba="no"
	for item in $output; do
		if [[ $found_ba == "yes" ]]; then
			buffer_addrs+=("0x$item")
			found_ba="no"
		fi
		if [[ $item == "buffer_addr:" ]]; then
			found_ba="yes"
		fi
	done
	# retrieve buffer sizes
	local	found_os="no"
	for item in $output; do
		if [[ $found_os == "yes" ]]; then
			output_sizes+=($item)
			found_os="no"
		fi
		if [[ $item == "output_size:" ]]; then
			found_os="yes"
		fi
	done
}

# -------------------------------- Dump code --------------------------------- #

function	dump_code() # (void)
{
	local	length=${#buffer_addrs[@]};
	local	regex='0x[0-9a-f]*:\s*add\s*BYTE\s*PTR\s*\[rax\],al\s*0x[0-9a-f]*:\s*add\s*BYTE\s*PTR\s*\[rax\],al\s*0x[0-9a-f]*:\s*add\s*BYTE\s*PTR\s*\[rax\],al\s*0x[0-9a-f]*:\s*add\s*BYTE\s*PTR\s*\[rax\],al\s*0x[0-9a-f]*:\s*add\s*BYTE\s*PTR\s*\[rax\],al'
	local 	breakpoint='_breakpoint'

	for ((i=0;i<$length;i++)); do
		declare -i	ngen=$(($i+1))
		local		addr="i ${buffer_addrs[$i]}"
		declare -i	limit=$(echo "${output_sizes[$i]}" | bc)
		declare -i	increment=$(($limit/5+1))

		printf "\x1b[0;36m[ ------- generation $ngen: ${buffer_addrs[$i]} (${output_sizes[$i]} bytes): ------- ]\x1b[0m\n"

		for ((j=$increment;j<=$limit;j+=$increment)); do
			local	examine="x/$j"
			local	output=`$gdb -ex="attach $bin_pid" -ex="$examine$addr" -ex="quit" | grep -A200 'in _breakpoint ()'`

			printf "\x1b[0;33m[log]\x1b[0m: trying with \x1b[0;31m$j\x1b[0m instructions (max \x1b[0;31m$limit\x1b[0m)..\n"

			if [[ $output =~ $regex ]]; then
				echo "$output"
				break
			fi
		done
	done
}

# ---------------------------------- Start ----------------------------------- #

declare -a	buffer_addrs=()
declare -a	output_sizes=()
		bin_output=""
		bin_pid=""

start_process
get_info "$bin_output"

if [[ ${#buffer_addrs[@]} != ${#output_sizes[@]} ]]; then
	echo "error: number of buffer addresses and output sizes found differs."
	killall $bin
	exit 1
fi

dump_code
kill $bin_pid
