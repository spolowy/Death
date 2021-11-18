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

green='\033[32m'
red='\033[31m'
yellow='\033[33m'
none='\033[0m'

argv="$@"
argc="$#"
self="$0"

if [ $argc -lt 3 ]; then
	printf "${yellow}usage${none}: $self [compile_mode] [ngen] [target 1] [target 2] ..\n"
	exit 1
fi

compile_mode="$1"
ngen="$2"
germ='../death'

declare -a target_array

# ----------------------------- define handlers ------------------------------ #

function	define_handlers
{
	trap 'ko' SIGILL SIGABRT SIGBUS SIGFPE SIGSEGV
	trap 'exit 0' SIGINT
}

# ---------------------------------------------------------------------------- #
#
function	go_to_script_directory
{
	local parent_path=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )

	cd "$parent_path"
}

function	compile
{
	rm -rf /tmp/test/* /tmp/test2/*
	mkdir -p /tmp/test /tmp/test2
	make "$compile_mode" -C ..
}

# ------------------------------- target array ------------------------------- #

function	fill_target_array
{
	let nbin=$(( $argc ))
	targets=( $argv )

	for ((n=2; n<$nbin; n++))
	do
		target_array+=( "${targets[$n]}" )
	done
}

# --------------------------------- summary ---------------------------------- #

function	summary
{
	local ninfected="$1"
	local ngen="$2"
	local tag="$3"

	if [[ "$ninfected" != "$ngen" ]]; then
		printf "\n ---------- ${tag}${ninfected}${none} infected out of ${green}${ngen}${none} --------------- \n"
	else
		printf "\n ---------- ${green}${ninfected}${none} infected out of ${green}${ngen}${none} --------------- \n"
	fi

}

function	not_infected
{
	local infected_name="$1"
	local target_name="$2"

	printf "  [${yellow}NOT INFECTED${none}]  [$infected_name] -> [${yellow}${target_name}${none}]\n"
}

function	ok
{
	local infected_name="$1"
	local target_name="$2"

	printf "  [${green}OK${none}]  [$infected_name] -> [$target_name]\n"
}

function ko
{
	errmsg[1]="SIGHUP"       ; errmsg[2]="SIGINT"
	errmsg[3]="SIGQUIT"      ; errmsg[4]="SIGILL"
	errmsg[5]="SIGTRAP"      ; errmsg[6]="SIGABRT"
	errmsg[7]="SIGBUS"       ; errmsg[8]="SIGFPE"
	errmsg[9]="SIGKILL"      ; errmsg[10]="SIGUSR1"
	errmsg[11]="SIGSEGV"     ; errmsg[12]="SIGUSR2"
	errmsg[13]="SIGPIPE"     ; errmsg[14]="SIGALRM"
	errmsg[15]="SIGTERM"     ; errmsg[16]="SIGSTKFLT"
	errmsg[17]="SIGCHLD"     ; errmsg[18]="SIGCONT"
	errmsg[19]="SIGSTOP"     ; errmsg[20]="SIGTSTP"
	errmsg[21]="SIGTTIN"     ; errmsg[22]="SIGTTOU"
	errmsg[23]="SIGURG"      ; errmsg[24]="SIGXCPU"
	errmsg[25]="SIGXFSZ"     ; errmsg[26]="SIGVTALRM"
	errmsg[27]="SIGPROF"     ; errmsg[28]="SIGWINCH"
	errmsg[29]="SIGIO"       ; errmsg[30]="SIGPWR"
	errmsg[31]="SIGSYS"      ;
	errmsg[34]="SIGRTMIN"    ; errmsg[35]="SIGRTMIN+1"
	errmsg[36]="SIGRTMIN+2"  ; errmsg[37]="SIGRTMIN+3"
	errmsg[38]="SIGRTMIN+4"  ; errmsg[39]="SIGRTMIN+5"
	errmsg[40]="SIGRTMIN+6"  ; errmsg[41]="SIGRTMIN+7"
	errmsg[42]="SIGRTMIN+8"  ; errmsg[43]="SIGRTMIN+9"
	errmsg[44]="SIGRTMIN+10" ; errmsg[45]="SIGRTMIN+11"
	errmsg[46]="SIGRTMIN+12" ; errmsg[47]="SIGRTMIN+13"
	errmsg[48]="SIGRTMIN+14" ; errmsg[49]="SIGRTMIN+15"
	errmsg[50]="SIGRTMAX-14" ; errmsg[51]="SIGRTMAX-13"
	errmsg[52]="SIGRTMAX-12" ; errmsg[53]="SIGRTMAX-11"
	errmsg[54]="SIGRTMAX-10" ; errmsg[55]="SIGRTMAX-9"
	errmsg[56]="SIGRTMAX-8"  ; errmsg[57]="SIGRTMAX-7"
	errmsg[58]="SIGRTMAX-6"  ; errmsg[59]="SIGRTMAX-5"
	errmsg[60]="SIGRTMAX-4"  ; errmsg[61]="SIGRTMAX-3"
	errmsg[62]="SIGRTMAX-2"  ; errmsg[63]="SIGRTMAX-1"
	errmsg[64]="SIGRTMAX"    ;

	local infected_name="$1"
	local ret_infected="$2"
	local target_name="$3"
	local ret_target="$4"
	let signal=0

	if [[ $ret_infected != 'ok' ]]
	then
		signal="$ret_infected"
		infected_name="${red}${infected_name}${none}"
	elif [[ $ret_target != 'ok' ]]
	then
		signal="$ret_target"
		target_name="${red}${target_name}${none}"
	fi

	local msg="${errmsg[$signal]}"

	printf "  [${red}${msg}${none}]  [$infected_name] -> [$target_name]\n"
}

function	process_run
{
	let	timeout=2

	local name="$1"
	local path="$2"
	if [[ $compile_mode == "debug" ]]; then
		timeout $timeout "$path" &
	else
		timeout $timeout "$path" > /dev/null 2>/dev/null &
	fi

	wait $!
	return "$?"
}

# ------------------------------ run infection ------------------------------- #

function	check_signature
{
	local target_path="$1"
	local signature=$(strings "$target_path" | grep '__UNICORNS_OF_THE_APOCALYPSE__')

	if [[ $signature ]];
	then
		return 0
	else
		return 1
	fi
}

# ------------------------------- loop through ------------------------------- #

function	check_process_return
{
	local ret="$1"
	let exit_status_lower=128
	let exit_status_upper=193

	if [[ $ret -gt $exit_status_lower ]] && [[ $ret -lt $exit_status_upper ]];
	then
		(( ret-=$exit_status_lower ))
		echo "$ret"
		return
	fi
	echo 'ok'
}

function	check_return
{
	local ret_infected="$1"
	local ret_target="$2"
	local target_path="$3"

	if [[ "$ret_infected" == 'ok' ]] && [[ "$ret_target" == 'ok' ]]
	then
		if check_signature "$target_path"
		then
			echo 'ok'
		else
			echo 'ni'
		fi
	else
		echo 'ko'
	fi
}

function	loop_through
{
	local ntarget="${#target_array[@]}"
	local infected_path="$germ"
	local tag=''
	let ninfected=0

	for ((n=0;n<$ngen;n++))
	do
		local index=$(($RANDOM % $ntarget))
		local selected_path="${target_array[$index]}"
		local target_path="/tmp/test/$(basename "$selected_path")$n"

		local infected_name=$(basename "$infected_path")
		local target_name=$(basename "$target_path")

		cp "$selected_path" "$target_path"

		process_run "$infected_name" "$infected_path"
		local ret_infected="$?"

		process_run "$target_name" "$target_path"
		local ret_target="$?"

		ret_infected=$(check_process_return "$ret_infected")
		ret_target=$(check_process_return "$ret_target")

		local ret=$(check_return "$ret_infected" "$ret_target" "$target_path")

		if [[ "$ret" == 'ok' ]]
		then
			ok "$infected_name" "$target_name"
			infected_path="$target_path"
			(( ninfected++ ))
		elif [[ "$ret" == 'ko' ]]
		then
			ko "$infected_name" "$ret_infected" "$target_name" "$ret_target"
			tag="${red}"
			break
		elif [[ "$ret" == 'ni' ]]
		then
			not_infected "$infected_name" "$target_name"
			tag="${yellow}"
		fi
	done
	summary "$ninfected" "$ngen" "$tag"
}

# ---------------------------------- start ----------------------------------- #

function	start
{
	define_handlers
	go_to_script_directory

	if ! compile || ! fill_target_array || ! loop_through
	then
		printf "${red}error${none}: $self an error has occured.\n"
	fi
}

start
