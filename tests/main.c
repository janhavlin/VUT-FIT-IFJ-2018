#include <stdio.h>
#include "tests.h"

int main()
{
    TEST_EQ_INT(0, 0, "Basic tests", "Pass test");
    TEST_EQ_INT(1, 0, "Basic tests", "Fail test");

    TEST_PRINT_STATS();
}
