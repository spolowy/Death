#!/bin/bash

green='\033[32m'
red='\033[31m'
yellow='\033[33m'
none='\033[0m'

germ='../death'

# ---------------------------------------------------------------------------- #

function	go_to_script_directory
{
	local	parent_path=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )

	cd "$parent_path"
}

function	find_function_address
{
	local   regex_line='([0-9a-f]+)\s+[a-zA-Z]+\s+([a-zA-Z0-9_]+)'
	local	functions=$(nm -n "$germ")
	local	function="$1"
	local	ret=''

	let nth_line=1
	while [ true ] # get line loop
	do
		local   line=$(echo "$functions" | awk "NR==$nth_line{print}")

		if [[ "$line" == '' ]]
		then
			break
		elif [[ $line =~ $regex_line ]]
		then
			if [[ "${BASH_REMATCH[2]}" == "$function" ]]
			then
				ret="${BASH_REMATCH[1]}"
				break
			fi
		fi
		(( nth_line++ ))
	done
	ret=$(echo $ret | sed 's/^0*//')
	ret=$(( 16#$ret ))
	echo "$ret"
}

function	build_definitions
{
	local	loader_exit=$(find_function_address 'loader_exit')
	local	loader_entry=$(find_function_address 'loader_entry')
	local	virus=$(find_function_address 'virus')
	local	call_virus=$(find_function_address 'call_virus')
	local	virus_header=$(find_function_address 'virus_header')
	local	jump_back_to_client=$(find_function_address 'jump_back_to_client')

	local	loader_size=$(( loader_exit-loader_entry ))
	local	dist_virus_loader=$(( virus-loader_entry ))
	local	dist_vircall_loader=$(( call_virus-loader_entry ))
	local	dist_header_loader=$(( virus_header-loader_entry ))
	local	dist_jmpclient_loader=$(( jump_back_to_client-loader_entry ))

	local	definitions=''

	definitions+=" -DLOADER_SIZE=${loader_size}"
	definitions+=" -DDIST_VIRUS_LOADER=${dist_virus_loader}"
	definitions+=" -DDIST_VIRCALL_LOADER=${dist_vircall_loader}"
	definitions+=" -DDIST_HEADER_LOADER=${dist_header_loader}"
	definitions+=" -DDIST_CLIENT_LOADER=${dist_jmpclient_loader}"

	echo "$definitions"
}

# ---------------------------------- start ----------------------------------- #

function	start
{
	go_to_script_directory

	build_definitions "$functions"
}

start
