
#!/bin/bash

DARKRED="\033[2;31m"
DARKGREEN="\033[2;32m"
GREEN="\033[32m"
RED="\033[31m"
YELLOW="\033[33m"
CYAN="\033[36m"
DARKGREY="\033[90m"
NONE="\033[0m"

mode=$1

function clean_dirs()
{
	rm -rf /tmp/test/*
	rm -rf /tmp/test2/*
}

function compile()
{
	echo -e $DARKGREY"\nCompile and create directories"$NONE
	clean_dirs

	mkdir -p /tmp/test /tmp/test2
	if [ "$mode" == "debug" ];then
		make fclean debug -C ..
	else
		make re -C ..
	fi
	cp -f ../death .
}

function check_signature()
{
	strings $1 | grep __UNICORNS_OF_THE_APOCALYPSE__ 1>/dev/null
}

function test_infect_1()
{
	clean_dirs

	echo -e $YELLOW"\n\t\t-- infect one --\n"$NONE
	echo -e $DARKGREY"    copy sane binary"$NONE
	cp -f /bin/ls /tmp/test
	echo -e $DARKGREY"    run death"$NONE
	./death
	echo -e $DARKGREY"    run infected binary"$NONE
	/tmp/test/ls
	sleep 1

	if check_signature "/tmp/test/ls"; then
		echo -e $CYAN"    infect one without daemon: "$GREEN"[ OK ]"$NONE
	else
		echo -e $CYAN"    infect one without daemon: "$RED"[ KO ]"$NONE
	fi
}

function test_spread_2()
{
	clean_dirs

	echo -e $YELLOW"\n\t\t-- spread two --\n"$NONE
	echo -e $DARKGREY"    copy first sane binary"$NONE
	cp -f /bin/ls /tmp/test
	echo -e $DARKGREY"    run death"$NONE
	./death
	echo -e $DARKGREY"    copy second sane binary"$NONE
	cp -f /bin/pwd /tmp/test2
	echo -e $DARKGREY"    run first infected binary"$NONE
	/tmp/test/ls
	sleep 1
	echo -e $DARKGREY"    run second infected binary"$NONE
	/tmp/test2/pwd
	sleep 1

	if check_signature "/tmp/test/ls" && check_signature "/tmp/test2/pwd"; then
		echo -e $CYAN"    spread two without daemon: "$GREEN"[ OK ]"$NONE
	else
		echo -e $CYAN"    spread two without daemon: "$RED"[ KO ]"$NONE
	fi
}

function test_spread_3()
{
	clean_dirs

	echo -e $YELLOW"\n\t\t-- spread three --\n"$NONE
	echo -e $DARKGREY"    copy first sane binary"$NONE
	cp -f /bin/ls /tmp/test
	echo -e $DARKGREY"    run death"$NONE
	./death
	echo -e $DARKGREY"    copy second sane binary"$NONE
	cp -f /bin/pwd /tmp/test2
	echo -e $DARKGREY"    run first infected binary"$NONE
	/tmp/test/ls
	sleep 1
	echo -e $DARKGREY"    run second infected binary"$NONE
	/tmp/test2/pwd
	sleep 1

	if ! check_signature "/tmp/test/ls" || ! check_signature "/tmp/test2/pwd"; then
		echo -e $CYAN"    spread three without daemon: "$RED"[ KO ]"$NONE
		return
	fi

	echo -e $DARKGREY"    copy third sane binary"$NONE
	cp -f /bin/uname /tmp/test/uname
	echo -e $DARKGREY"    run second infected binary"$NONE
	/tmp/test2/pwd
	sleep 1
	echo -e $DARKGREY"    run third infected binary"$NONE
	/tmp/test/uname
	sleep 1

	if check_signature "/tmp/test2/pwd"; then
		echo -e $CYAN"    spread three without daemon: "$GREEN"[ OK ]"$NONE
	else
		echo -e $CYAN"    spread three without daemon: "$RED"[ KO ]"$NONE
	fi
}

compile

test_infect_1
test_spread_2
test_spread_3
