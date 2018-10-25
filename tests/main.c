#include <stdio.h>
#include <stdlib.h>
#include "tests.h"
#include "../src/scanner.h"
#include "../src/dyn_arr.h"

void test_scanner(char *grp_name) {
    FILE *scanner_file = fopen("tests/test_scanner", "r");
    if (scanner_file == NULL)
    {
        fprintf(stderr, "Error opening test_scanner");
        exit(1);
    }

    TEST_EQ_INT(getToken(scanner_file), TOK_ID, grp_name, "Identifier test");
    TEST_EQ_INT(getToken(scanner_file), TOK_INT, grp_name, "Integer 0 test");
    TEST_EQ_INT(getToken(scanner_file), TOK_INT, grp_name, "Integer test");
    TEST_EQ_INT(getToken(scanner_file), TOK_FLOAT, grp_name, "Float 0.0 test");
    TEST_EQ_INT(getToken(scanner_file), TOK_FLOAT, grp_name, "Float test");
    TEST_EQ_INT(getToken(scanner_file), TOK_FLOAT, grp_name, "Float e test");
    TEST_EQ_INT(getToken(scanner_file), TOK_STRING, grp_name, "String test");
    TEST_EQ_INT(getToken(scanner_file), TOK_EOL, grp_name, "\'\\n\' test");
    TEST_EQ_INT(getToken(scanner_file), TOK_ADD, grp_name, "+ test");
    TEST_EQ_INT(getToken(scanner_file), TOK_SUB, grp_name, "- test");
    TEST_EQ_INT(getToken(scanner_file), TOK_MUL, grp_name, "* test");
    TEST_EQ_INT(getToken(scanner_file), TOK_DIV, grp_name, "/ test");
    TEST_EQ_INT(getToken(scanner_file), TOK_LESS, grp_name, "< test");
    TEST_EQ_INT(getToken(scanner_file), TOK_GREATER, grp_name, "> test");
    TEST_EQ_INT(getToken(scanner_file), TOK_LESSEQ, grp_name, "<= test");
    TEST_EQ_INT(getToken(scanner_file), TOK_GREATEREQ, grp_name, ">= test");
    TEST_EQ_INT(getToken(scanner_file), TOK_EQ, grp_name, "== test");
    TEST_EQ_INT(getToken(scanner_file), TOK_NEQ, grp_name, "!= test");
    TEST_EQ_INT(getToken(scanner_file), TOK_ASSIGN, grp_name, "= test");
    TEST_EQ_INT(getToken(scanner_file), TOK_LEFT_BRACKET, grp_name, "( test");
    TEST_EQ_INT(getToken(scanner_file), TOK_RIGHT_BRACKET, grp_name, ") test");
    TEST_EQ_INT(getToken(scanner_file), TOK_EOL, grp_name, "\'\\n\' test");


    //INT + FLOAT * ID != == = ID <= = STR > != / * ( - ) +
    TEST_EQ_INT(getToken(scanner_file), TOK_INT, grp_name, "Int tight test");
    TEST_EQ_INT(getToken(scanner_file), TOK_ADD, grp_name, "+ tight test");
    TEST_EQ_INT(getToken(scanner_file), TOK_FLOAT, grp_name, "Float tight test");
    TEST_EQ_INT(getToken(scanner_file), TOK_MUL, grp_name, "* tight test");
    TEST_EQ_INT(getToken(scanner_file), TOK_ID, grp_name, "Identifier tight test");
    TEST_EQ_INT(getToken(scanner_file), TOK_NEQ, grp_name, "!= tight test");
    TEST_EQ_INT(getToken(scanner_file), TOK_EQ, grp_name, "== tight test");
    TEST_EQ_INT(getToken(scanner_file), TOK_ASSIGN, grp_name, "= tight test");
    TEST_EQ_INT(getToken(scanner_file), TOK_ID, grp_name, "Identifier tight test");
    TEST_EQ_INT(getToken(scanner_file), TOK_LESSEQ, grp_name, "<= tight test");
    TEST_EQ_INT(getToken(scanner_file), TOK_ASSIGN, grp_name, "= tight test");
    TEST_EQ_INT(getToken(scanner_file), TOK_STRING, grp_name, "String tight test");
    TEST_EQ_INT(getToken(scanner_file), TOK_GREATER, grp_name, "> tight test");
    TEST_EQ_INT(getToken(scanner_file), TOK_NEQ, grp_name, "!= tight test");
    TEST_EQ_INT(getToken(scanner_file), TOK_DIV, grp_name, "/ tight test");
    TEST_EQ_INT(getToken(scanner_file), TOK_MUL, grp_name, "* tight test");
    TEST_EQ_INT(getToken(scanner_file), TOK_LEFT_BRACKET, grp_name, "( tight test");
    TEST_EQ_INT(getToken(scanner_file), TOK_SUB, grp_name, "- tight test");
    TEST_EQ_INT(getToken(scanner_file), TOK_RIGHT_BRACKET, grp_name, ") tight test");
    TEST_EQ_INT(getToken(scanner_file), TOK_ADD, grp_name, "+ tight test");

}

void test_string(char *grp_name) {
    unsigned err;
    string str = stringInit(&err);
    charPut(str, 'a', &err);
    TEST_EQ_INT(strcmp(str, "a"), 0, grp_name, "str == \"a\"");
}

int main()
{
    test_scanner("scanner.c tests");
    test_string("dyn_arr tests");

    TEST_PRINT_STATS();
}
