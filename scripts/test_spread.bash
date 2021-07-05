#!/bin/bash

# test_spread.bash
#
#     compile_mode:  specifies Makefile's compilation rule (re / debug).
#     bins:          comma separated list of binaries to select for spread test
#     ngen:          number of generation
#
# Select a random binary from the list <bins> and infect it with the previous
# infected binary.
# Continues to do so for <ngen> (n generations).

if [ $# != 3 ]; then
	echo "usage: $0 compile_mode bins ngen output"
	exit 1
fi

green='\033[32m'
red='\033[31m'
yellow='\033[33m'
none='\033[0m'

compile_mode="$1"
bins="$2"
ngen="$3"

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

function build_array
{
	bins="$1"
	IFS=',' read -ra bins_array <<< "$bins"
}

function define_handler
{
	# trap _ko SIGILL SIGABRT SIGBUS SIGFPE SIGSEGV
	trap 'kill -INT -$pid && exit 1' SIGINT
}

function _not_infected
{
	printf "$yellow[NOT INFECTED]$none\n"
}

function _ok
{
	printf "$green[OK]$none\n"
}

function _ko
{
	errmsg[1]="SIGHUP"
	errmsg[2]="SIGINT"
	errmsg[3]="SIGQUIT"
	errmsg[4]="SIGILL"
	errmsg[5]="SIGTRAP"
	errmsg[6]="SIGABRT"
	errmsg[7]="SIGBUS"
	errmsg[8]="SIGFPE"
	errmsg[9]="SIGKILL"
	errmsg[10]="SIGUSR1"
	errmsg[11]="SIGSEGV"
	errmsg[12]="SIGUSR2"
	errmsg[13]="SIGPIPE"
	errmsg[14]="SIGALRM"
	errmsg[15]="SIGTERM"
	errmsg[16]="SIGSTKFLT"
	errmsg[17]="SIGCHLD"
	errmsg[18]="SIGCONT"
	errmsg[19]="SIGSTOP"
	errmsg[20]="SIGTSTP"
	errmsg[21]="SIGTTIN"
	errmsg[22]="SIGTTOU"
	errmsg[23]="SIGURG"
	errmsg[24]="SIGXCPU"
	errmsg[25]="SIGXFSZ"
	errmsg[26]="SIGVTALRM"
	errmsg[27]="SIGPROF"
	errmsg[28]="SIGWINCH"
	errmsg[29]="SIGIO"
	errmsg[30]="SIGPWR"
	errmsg[31]="SIGSYS"
	errmsg[34]="SIGRTMIN"
	errmsg[35]="SIGRTMIN+1"
	errmsg[36]="SIGRTMIN+2"
	errmsg[37]="SIGRTMIN+3"
	errmsg[38]="SIGRTMIN+4"
	errmsg[39]="SIGRTMIN+5"
	errmsg[40]="SIGRTMIN+6"
	errmsg[41]="SIGRTMIN+7"
	errmsg[42]="SIGRTMIN+8"
	errmsg[43]="SIGRTMIN+9"
	errmsg[44]="SIGRTMIN+10"
	errmsg[45]="SIGRTMIN+11"
	errmsg[46]="SIGRTMIN+12"
	errmsg[47]="SIGRTMIN+13"
	errmsg[48]="SIGRTMIN+14"
	errmsg[49]="SIGRTMIN+15"
	errmsg[50]="SIGRTMAX-14"
	errmsg[51]="SIGRTMAX-13"
	errmsg[52]="SIGRTMAX-12"
	errmsg[53]="SIGRTMAX-11"
	errmsg[54]="SIGRTMAX-10"
	errmsg[55]="SIGRTMAX-9"
	errmsg[56]="SIGRTMAX-8"
	errmsg[57]="SIGRTMAX-7"
	errmsg[58]="SIGRTMAX-6"
	errmsg[59]="SIGRTMAX-5"
	errmsg[60]="SIGRTMAX-4"
	errmsg[61]="SIGRTMAX-3"
	errmsg[62]="SIGRTMAX-2"
	errmsg[63]="SIGRTMAX-1"
	errmsg[64]="SIGRTMAX"

	msg=${errmsg[$1]}
	printf "$red[$msg]$none\n"
}

function process_run
{
	let timeout=5
	let exit_status_lower=128
	let exit_status_upper=193
	let ret=0

	local	bin="$1"
	if [[ $compile_mode == "debug" ]]; then
		timeout $timeout "$bin" &
	else
		timeout $timeout "$bin" > /dev/null 2>/dev/null &
	fi
	pid=$!
	wait $pid

	ret=$?
	if [[ $ret -gt $exit_status_lower ]] && [[ $ret -lt $exit_status_upper ]];
	then
	{
		((ret-=$exit_status_lower))
		_ko $ret
		return 1
	}
	fi
}

function test_run
{
	infected_path="$1"
	sane_path="$2"

	# test for ...
	infected_name=$(basename "$infected_path")
	sane_name=$(basename "$sane_path")
	printf "  [$infected_name] -> [$sane_name]  "

	if process_run "$infected_path" && process_run "$sane_path"; then
		signature=$(strings $sane_path | grep '__UNICORNS_OF_THE_APOCALYPSE__')
		if [[ $signature ]];
		then
			_ok
		else
			_not_infected
			return 1
		fi
	else
		return 1
	fi
	return 0
}

function summary
{
	local	n="$1"
	local	ngen="$2"

	if [[ $n != $ngen ]]; then
		printf "\n ---------- $red$n$none infected out of $green$ngen$none --------------- \n"
	else
		printf "\n ---------- $green$n$none infected out of $green$ngen$none --------------- \n"
	fi

}

function loop_through
{
	local	array_size=${#bins_array[@]}
	local	infected_path='../death'
	local	sane_path=''

	define_handler

	for ((n=0;n<$ngen;n++))
	do
		# select a random binary from the list to target
		index=$(($RANDOM % $array_size))
		selected_path="${bins_array[$index]}"
		# build sane binary path
		sane_path="/tmp/test/$(basename "$selected_path")$n"
		# copy sane binary to destination path
		cp "$selected_path" "$sane_path"
		# run infected binary
		if ! test_run "$infected_path" "$sane_path"; then
			break
		fi
		# set old sane binary as new infected
		infected_path="$sane_path"
	done
	summary "$n" "$ngen"
}

declare -a bins_array

go_to_script_directory
compile
build_array "$bins"
loop_through
