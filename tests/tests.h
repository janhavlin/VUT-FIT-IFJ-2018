#ifndef GLOBALPROJECT_TESTS_H
#define GLOBALPROJECT_TESTS_H

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"


void cmp_group_names(char groupname[]);
void TEST_EQ_INT(int a, int b, char groupname[], char testname[]);
void TEST_PRINT_STATS();

#endif //GLOBALPROJECT_TESTS_H
