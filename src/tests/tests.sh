#!/bin/sh

GREEN="\e[32m"
RED="\e[31m"
NORMAL="\e[39m"

TSTCNT=0
BUFF=
PRG=../../ifj18
OUTFILE=tests.out

print_to_file()
{
    if [ $1 -eq 1 ]; then
        echo $2 > $OUTFILE
    else
        echo $2 >> $OUTFILE
    fi
}

check_output()
{
    if [ $1 -eq $2 ]; then
        echo "Test $5: $GREEN""OK$NORMAL"
        print_to_file $5 "Test $5: OK"
    else
        echo "Test $5: $RED""FAIL$NORMAL before line $3 in $4, return value: $1, expected: $2"
        print_to_file $5 "Test $5: FAIL before line $3 in $4, return value: $1, expected: $2"
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
        echo "$GREEN$BUFF$NORMAL"
        echo "$BUFF" | $PRG
        check_output $? $RETVAL $LINECNT $1 $TSTCNT
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
