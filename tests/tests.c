#include <stdio.h>
#include <string.h>
#include "tests.h"

int count_tests = 0;
int count_group = 0;
int count_group_tests = 0;
int count_pass = 0;
int count_fail = 0;
char prev_test[128];


void cmp_group_names(char groupname[])
{
    if (!strcmp(groupname, prev_test))
    {
        printf("Test %d.%d: ", count_group, ++count_group_tests);
    }
    else
    {
        printf("=== %s:\n", groupname);
        printf("Test %d.%d: ", ++count_group, count_group_tests = 1);
        strcpy(prev_test, groupname);
    }
}

void TEST_EQ_INT(int a, int b, char groupname[], char testname[])
{
    cmp_group_names(groupname);
    count_tests++;

    if (a == b)
    {
        printf("%s: " KGRN "PASS\n" KNRM, testname);
        count_pass++;
    }
    else
    {
        printf("%s: " KRED "FAIL\n" KNRM, testname);
        printf(KRED "--" KNRM " A == B:\n");
        printf(KRED "--" KNRM " A: %d\n", a);
        printf(KRED "--" KNRM " B: %d\n", b);
        count_fail++;
    }
}

void TEST_PRINT_STATS()
{
    printf("==========\n");
    printf("Total amount of tests: %d\n", count_tests);
    printf("Passed tests: %d\n", count_pass);
    printf("Failed tests: %d\n", count_fail);

    if (count_fail > 0)
        printf(KRED "FIX YER CODE!!!\n" KNRM);
    else
        printf(KGRN "ALL OK\n" KNRM);
}
