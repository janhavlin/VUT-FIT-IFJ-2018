#include <stdio.h>
#include <stdlib.h>
#include "tests.h"
#include "scanner.h"

void test_scanner(char *grp_name) {
    FILE *scanner_file = fopen("tests/test_scanner", "r");
    if (scanner_file == NULL)
    {
        fprintf(stderr, "Error opening test_scanner");
        exit(1);
    }

    TEST_EQ_INT(get_token(scanner_file), TOK_ID, grp_name, "Identifier test");
    TEST_EQ_INT(get_token(scanner_file), TOK_INT, grp_name, "Integer test");
    TEST_EQ_INT(get_token(scanner_file), TOK_FLOAT, grp_name, "Float test");
    TEST_EQ_INT(get_token(scanner_file), TOK_STRING, grp_name, "String test");
    TEST_EQ_INT(get_token(scanner_file), TOK_ADD, grp_name, "+ test");
    TEST_EQ_INT(get_token(scanner_file), TOK_SUB, grp_name, "- test");
    TEST_EQ_INT(get_token(scanner_file), TOK_MUL, grp_name, "* test");
    TEST_EQ_INT(get_token(scanner_file), TOK_DIV, grp_name, "/ test");
    TEST_EQ_INT(get_token(scanner_file), TOK_LESS, grp_name, "< test");
    TEST_EQ_INT(get_token(scanner_file), TOK_GREATER, grp_name, "> test");
    TEST_EQ_INT(get_token(scanner_file), TOK_LESSEQ, grp_name, "<= test");
    TEST_EQ_INT(get_token(scanner_file), TOK_GREATEREQ, grp_name, ">= test");
    TEST_EQ_INT(get_token(scanner_file), TOK_EQ, grp_name, "== test");
    TEST_EQ_INT(get_token(scanner_file), TOK_NEQ, grp_name, "!= test");
    TEST_EQ_INT(get_token(scanner_file), TOK_ASSIGN, grp_name, "= test");
    TEST_EQ_INT(get_token(scanner_file), TOK_LEFT_BRACKET, grp_name, "( test");
    TEST_EQ_INT(get_token(scanner_file), TOK_RIGHT_BRACKET, grp_name, ") test");
}

int main()
{
    test_scanner("scanner.c tests");

    TEST_PRINT_STATS();
}
