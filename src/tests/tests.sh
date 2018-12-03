#!/bin/sh

GREEN="\e[32m"
RED="\e[31m"
NORMAL="\e[39m"

TSTCNT=0
TSTPASS=0
TSTFAIL=0
BUFF=
PRG=../ifjcomp
INTERPRETER=./ic18int
TSTOUTFILE=tests.out
OUTSRC=ifjcode18.src

print_to_file()
{
    if [ $1 -eq 1 ]; then
        echo $2 > $TSTOUTFILE
        #echo $3 >> $TSTOUTFILE
    else
        echo $2 >> $TSTOUTFILE
        #echo $3 >> $TSTOUTFILE
    fi
}

check_output()
{
    if [ $1 -eq $2 ]; then
        echo "Test $TSTCNT: $GREEN""OK$NORMAL"
        print_to_file $TSTCNT "Test $TSTCNT: OK"
        TSTPASS=$((TSTPASS+1))
    else
        echo "Test $TSTCNT: $RED""FAIL$NORMAL before line $LINECNT in $3, return value: $1, expected: $2"
        print_to_file $TSTCNT "Test $TSTCNT: FAIL before line $LINECNT in $3, return value: $1, expected: $2"
        TSTFAIL=$((TSTFAIL+1))
    fi
}

interprete()
{
    #echo "interpreting"
    sed -n -E -e '/.IFJcode18/,$ p' $OUTSRC.tmp > $OUTSRC
    $INTERPRETER $OUTSRC
    check_output $? $1 $2

}

print_stats()
{
    if [ $TSTFAIL -eq 0 ]; then
        echo "$GREEN""Total amount of tests: $TSTCNT"", Passed: $TSTPASS"", Failed: $TSTFAIL$NORMAL"
    else
        echo "$RED""Total amount of tests: $TSTCNT"", Passed: $TSTPASS"", Failed: $TSTFAIL$NORMAL"
    fi

}

test_file()
{
    LINECNT=0
    echo "Testing file $1"
    while read -r LINE; do
        LINECNT=$((LINECNT+1))
        FIRSTCHAR=$(echo "$LINE" | cut -c1)

        if [ "$FIRSTCHAR" = "$" ] && [ "$BUFF" != "" ]; then
            TSTCNT=$((TSTCNT+1))
            RETVAL=$(echo $LINE | cut -c 3-)
            #echo "$GREEN$BUFF$NORMAL"
            echo "$BUFF" | $PRG > $OUTSRC.tmp
            COMPILERET=$?
            if [ $COMPILERET -eq 0 ]; then
                interprete $RETVAL $1
            else
                check_output $COMPILERET $RETVAL $1
            fi
            BUFF=""
        else
            if [ "$BUFF" = "" ]; then
                BUFF="$LINE"
            else
                BUFF="$BUFF\n$LINE"
            fi
        fi
    done < $1
}

test_file test_scanner
test_file test_parser
test_file test_type_comp

print_stats
