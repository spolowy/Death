
#!/bin/bash

# checks if argument is passed
if [ $# != 1 ]; then
	echo "./run.bash folder"
	exit 1
fi
# checks if argument is a directory
if [[ ! -d $1 ]]; then
	echo "$1 is not a directory."
	exit 1
fi

red="\033[31m"
green="\033[32m"
yellow="\033[33m"
cyan="\033[36m"
none="\033[0m"

let n_item=0
let n_failed=0

function _not_infected()
{
	printf "$yellow[NOT INFECTED]$none\n"
}

function _ok()
{
	printf "$green[OK]$none\n"
}

function _ko()
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

# trap _ko SIGILL SIGABRT SIGBUS SIGFPE SIGSEGV
trap 'kill -INT -$pid && exit 1' SIGINT

let timeout=5
let exit_status_lower=128
let exit_status_upper=193
let ret=0
directory=$1
content=`ls $directory`

# loop through directory
for item in $content
do
	# test for ...
	printf "  [$item]  "
	file=$directory/$item
	# run as subprocess and store pid
	timeout $timeout $file > /dev/null 2>/dev/null &
	pid=$!
	wait $pid

	ret=$?
	if [[ $ret -gt $exit_status_lower ]] && [[ $ret -lt $exit_status_upper ]];
	then
	{
		((ret-=$exit_status_lower))
		_ko $ret
		((n_failed++))
	}
	else
	{
		signature=`strings $file | grep _UNICORNS_`
		if [[ $signature ]];
		then
			_ok
		else
			_not_infected
		fi
	}
	fi
	((n_item++))
done

printf "\n ---------- $red$n_failed$none failed out of $green$n_item$none --------------- \n"
