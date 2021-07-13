#!/bin/bash

if [ $# -lt 2 ]; then
	echo "usage: $0 [ngen] [target 1] [target 2] .."
	exit 1
fi

germ="../death"
gdb="/usr/bin/gdb"

declare -a	target_array=()
declare -a	byte_array=()
declare -a	swap_array=()
let		ratio=4

# ---------------------------------------------------------------------------- #

function 	go_to_script_directory
{
	local	parent_path=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )

	cd "$parent_path"
}

function	compile
{
	local	compile_mode='debug'

	rm -rf /tmp/test/* /tmp/test2/*
	mkdir -p /tmp/test /tmp/test2
	make "$compile_mode" -C ..
}

# ----------------------------- dump byte array ------------------------------ #

function	dump_byte_array
{
	echo -e "\n\n\n\n"
	for byte in ${byte_array[@]}
	do
		echo -n "$byte "
	done
	echo -e "\n\n\n\n"
}

# ----------------------------- target binaries ------------------------------ #

function	fill_target_array
{
	let	nbin=$(( $BASH_ARGC-1 ))

	for ((n=0; n<$nbin; n++))
	do
		target_array+=("${BASH_ARGV[$n]}")
	done
}

# -------------------------------- byte array -------------------------------- #

function	fill_byte_array
{
	local	gdb_output=$($gdb -ex='p/d _start-loader_entry' -ex='quit' $germ | grep  -A100 '$1 ')
	local	regex_length='^\$1 = ([0-9]+)'
	local	germ_size=''

	if [[ $gdb_output =~ $regex_length ]]
	then
		germ_size="${BASH_REMATCH[1]}"
	else
		echo "[error]: couldn't find germ size."
		return
	fi

	germ_size=$(( $germ_size/$ratio ))

	for ((i=0; i<$germ_size; i++))
	do
		byte_array+=("00")
	done
}

# ------------------------------- color array -------------------------------- #

function	colorize_byte_array
{
	local		byte_string=''

	for byte in ${byte_array[@]}
	do
		byte_string+="$byte "
	done

	byte_string=$(echo "$byte_string" | lolcat --force --freq 0.0004)
	byte_array=()

	for byte in $byte_string
	do
		byte_array+=("$byte")
	done
}

# --------------------------- loop through blocks ---------------------------- #

function	get_block_info
{
	local	infected_output="$1"
	local	nrow="$2"
	local	nth="$3"
	local	regex="$4"
	local	ret=''

	let	index=$(( $nrow+$nth ))

	local	item=$(echo "$infected_output" | awk "NR==$index{print}")

	if [[ $item =~ $regex ]]
	then
		ret="${BASH_REMATCH[1]}"
	fi
	echo "$ret"
}

function	shift_bytes
{
	let	size="$1"
	let	shift_amount="$2"
	let	offset="$3"

	let	byte_offset=$(( $offset ))
	let	swap_offset=$(( $offset+$shift_amount ))

	for ((i=0;i<$size;i++))
	do
		swap_array["$swap_offset"]="${byte_array[$byte_offset]}"
		(( swap_offset++ ))
		(( byte_offset++ ))
	done
}

function	update_byte_array
{
	for ((i=0;i<${#swap_array[@]};i++))
	do
		byte_array["$i"]="${swap_array[$i]}"
	done
}

function	loop_through_blocks
{
	local	infected_output="$1"
	local	regex_stop='#\s*General Info'
	local	regex_block='\[[-]+ block'
	local	row=''
	let	nrow=1
	let	offset=0

	swap_array=()

	while ! [[ $row =~ $regex_stop ]]
	do
		row=$(echo "$infected_output" | awk "NR==$nrow{print}")

		if [[ $row =~ $regex_block ]]
		then
			local	shift_amount=$(get_block_info "$infected_output" "$nrow" '8' 'shift_amount\s*:\s*([0-9\-]+)')
			local	size=$(get_block_info "$infected_output" "$nrow" '5' 'size\s*:\s*([0-9\-]+)')

			shift_bytes "$size" "$shift_amount" "$offset"

			(( offset+=$size ))
		fi
		(( nrow++ ))
	done
	update_byte_array
}

# ------------------------------ run infection ------------------------------- #

function 	get_infected_output
{
	local	infected_path="$1"
	local	regex_colors='\x1B\[([0-9]{1,3}(;[0-9]{1,2})?)?[mGK]'
	local	ret=$($infected_path | sed -r "s/$regex_colors//g")

	echo "$ret"
}

function	run_infection
{
	local	infected_path="$1"
	local	target_path="$2"

	local	infected_output=$(get_infected_output "$infected_path")

	loop_through_blocks "$infected_output"
	dump_byte_array
}

# ------------------------------- loop through ------------------------------- #

function	loop_through
{
	local	ngen="$1"
	local	array_size=${#target_array[@]}
	local	infected_path="$germ"

	for ((n=0;n<$ngen;n++))
	do
		# select a random binary from the list to target
		local	index=$(($RANDOM % $array_size))
		local	selected_path="${target_array[$index]}"
		# build sane binary path
		local	target_path="/tmp/test/$(basename "$selected_path")$n"
		# copy sane binary to destination path
		cp "$selected_path" "$target_path"
		# run infected binary
		if ! run_infection "$infected_path" "$target_path"; then
			break
		fi
		# set old sane binary as new infected
		infected_path="$target_path"
	done
}

# ---------------------------------- start ----------------------------------- #

function	start
{
	go_to_script_directory
	compile

	fill_target_array
	fill_byte_array
	colorize_byte_array

	dump_byte_array
	loop_through "$1"
}

start "$1"
