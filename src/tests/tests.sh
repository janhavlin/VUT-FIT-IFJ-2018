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

PRINTED=0
CHECKPRINT=0

print_to_file()
{
    if [ $1 -eq 1 ]; then
        echo $2 > $TSTOUTFILE
    else
        echo $2 >> $TSTOUTFILE
    fi
}

check_output()
{
    if [ $2 -eq $3 ] && [ "$4" = "$5" ]; then
        echo "Test $TSTCNT: $GREEN""OK$NORMAL"
        print_to_file $TSTCNT "Test $TSTCNT: OK"
        TSTPASS=$((TSTPASS+1))
    else
        echo "Test $TSTCNT: $RED""FAIL$NORMAL before line $LINECNT in $1, return value: $2, expected: $3; print: '$4', expected: '$5'"
        print_to_file $TSTCNT "Test $TSTCNT: FAIL before line $LINECNT in $1, return value: $2, expected: $3; print: '$4', expected: '$5'"
        TSTFAIL=$((TSTFAIL+1))
    fi

    PRINTED=0
    EXPECTEDPRINT=0
}

interprete()
{
    #echo "interpreting"
    sed -n -E -e '/.IFJcode18/,$ p' $OUTSRC.tmp > $OUTSRC
    PRINTED=$($INTERPRETER $OUTSRC)
    INTERPRETERRETVAL=$?
    #echo "INTERPRETER PRINT $PRINTED""-"
    check_output $1 $INTERPRETERRETVAL $RETVAL $PRINTED $EXPECTEDPRINT
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
            RETVAL=$(echo $LINE | cut -c 3-4)
            EXPECTEDPRINT=$(echo $LINE | cut -c 5-)
            #echo "EXPECTED RET $RETVAL""a"
            #echo "EXPECTED PRINT $EXPECTEDPRINT""b"
            #echo "$GREEN$BUFF$NORMAL"
            echo "$BUFF" | $PRG > $OUTSRC.tmp
            COMPILERETVAL=$?
            if [ $COMPILERETVAL -eq 0 ]; then
                interprete $1
            else
                check_output $1
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

#test_file test_scanner
#test_file test_parser
test_file test_type_comp

print_stats
