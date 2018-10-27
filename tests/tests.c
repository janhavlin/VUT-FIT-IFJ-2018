#include <stdio.h>
#include <string.h>
#include "tests.h"

int count_tests = 0;
int count_group = 0;
int count_group_tests = 0;
int count_group_pass = 0;
int count_group_fail = 0;
int count_pass = 0;
int count_fail = 0;
char groupname[128];

void TEST_SET_GROUP(char grp[])
{
    if (count_group_tests)
    {
        printf(count_group_fail ? KRED : KGRN);
        printf("===" KNRM " %s: Tests: %d, Passed: %d, Failed: %d\n", groupname, count_group_tests, count_group_pass, count_group_fail);
    }

    strcpy(groupname, grp);
    printf("====== %s:\n", grp);
    count_group_tests = count_group_pass = count_group_fail = 0;
    count_group++;
}

void TEST_EQ_INT(int a, int b, char testname[], int mode)
{
    count_tests++;

    if (a == b)
    {
        count_group_tests++;
        if (!mode)
            printf("Test %d.%d: %s: " KGRN "PASS\n" KNRM, count_group, count_group_tests, testname);
        count_pass++;
        count_group_pass++;
    }
    else
    {
        printf("Test %d.%d: %s: " KRED "FAIL\n" KNRM, count_group, ++count_group_tests, testname);
        printf(KRED "--" KNRM " A == B:\n");
        printf(KRED "--" KNRM " A: %d\n", a);
        printf(KRED "--" KNRM " B: %d\n", b);
        count_fail++;
        count_group_fail++;
    }
}

void TEST_PRINT_STATS()
{
    if (count_group_tests)
    {
        printf(count_group_fail ? KRED : KGRN);
        printf("===" KNRM " %s: Tests: %d, Passed: %d, Failed: %d\n", groupname, count_group_tests, count_group_pass, count_group_fail);
    }

    printf("==========\n");
    printf("Total amount of tests: %d\n", count_tests);
    printf("Passed tests: %d\n", count_pass);
    printf("Failed tests: %d\n", count_fail);

    if (count_fail > 0)
        printf(KRED "FIX YER CODE!!!\n" KNRM);
    else
        printf(KGRN "ALL OK\n" KNRM);
}
