#!/bin/bash

red='\033[31m'
green='\033[32m'
yellow='\033[33m'
none='\033[0m'

self="$0"

if [ $# -ne 2 ]; then
	printf "${yellow}usage${none}: $self [compile_mode] [directory]\n"
	exit 1
fi
if [[ ! -d $2 ]]; then
	printf "${red}error${none}: $self $1 is not a directory.\n"
	exit 1
fi

compile_mode="$1"
directory="$2"
germ='../death'

# ----------------------------- define handlers ------------------------------ #

function	define_handlers
{
	trap 'ko' SIGILL SIGABRT SIGBUS SIGFPE SIGSEGV
	trap 'exit 0' SIGINT
}

# ---------------------------------------------------------------------------- #

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

# --------------------------------- summary ---------------------------------- #

function	summary
{
	local ninfected="$1"
	local nitem="$2"
	local tag="$3"

	if [[ "$ninfected" != "$nitem" ]]; then
		printf "\n ---------- ${tag}${ninfected}${none} infected out of ${green}${nitem}${none} --------------- \n"
	else
		printf "\n ---------- ${green}${ninfected}${none} infected out of ${green}${nitem}${none} --------------- \n"
	fi
}

function	not_infected
{
	local item="$1"

	printf "  ${yellow}%-12s${none}  $item\n" "NOT INFECTED"
}

function	ok
{
	local item="$1"

	printf "  ${green}%-12s${none}  $item\n" "OK"
}

function	ko
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

	local path="$1"
	local ret="$2"
	let signal=0

	if [[ $ret != 'ok' ]]
	then
		signal="$ret"
		path="${red}${path}${none}"
	fi

	local msg="${errmsg[$signal]}"

	printf "  ${red}%-12s${none}  $path\n" "${msg}"
}

# ------------------------------ run infection ------------------------------- #

function	process_run
{
	let	timeout=2

	local path="$1"

	timeout $timeout "$path" > /dev/null 2>/dev/null &
	wait $!
	return "$?"
}

function	check_signature
{
	local infected_path="$1"
	local signature=$(strings "$infected_path" | grep '__UNICORNS_OF_THE_APOCALYPSE__')

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
	local ret="$1"
	local path="$2"

	if [[ "$ret" == 'ok' ]]
	then
		if check_signature "$path"
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
	let nitem=0
	let ninfected=0
	local tag=''

	local dest='/tmp/test'
	cp "$directory"/* "$dest"
	"$germ"

	local content=$(ls "$dest")

	# loop through directory
	for item in $content
	do
		# test for ...
		local path="${dest}/${item}"
		# run as subprocess and store pid
		process_run "$path"
		local ret_process="$?"

		ret_process=$(check_process_return "$ret_process")
		local ret=$(check_return "$ret_process" "$path")

		if [[ "$ret" == 'ok' ]]
		then
			ok "$item"
			((ninfected++))
		elif [[ "$ret" == 'ko' ]]
		then
			ko "$item" "$ret_process"
			tag="${red}"
		elif [[ "$ret" == 'ni' ]]
		then
			not_infected "$item"
			tag="${yellow}"
		fi
		((nitem++))
	done
	summary "$ninfected" "$nitem" "$tag"
}

function	start
{
	define_handlers
	go_to_script_directory

	if ! compile || ! loop_through
	then
		printf "${red}error${none}: $self an error has occured.\n"
		exit 1
	fi
}

start
