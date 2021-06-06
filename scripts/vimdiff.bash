
#!/bin/bash

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

function diff()
{
    cp -f /bin/pwd /tmp/test/pwd1
    cp -f /bin/pwd /tmp/test/pwd2
    ./death
    xxd /tmp/test/pwd1 > pwd1.x
    xxd /tmp/test/pwd2 > pwd2.x
    vimdiff pwd1.x pwd2.x
}

compile
diff
