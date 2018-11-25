#include <stdio.h>
#include <stdlib.h>
#include "tests.h"
#include "../src/scanner.h"
#include "../src/dyn_arr.h"
#include "../src/type_conv.h"
#include "../src/ifj_error.h"
#include "../src/s_table.h"
#include "../src/parser.h"

int errflg;

void test_scanner(char *grp_name, int mode)
{
    FILE *scanner_file = fopen("tests/test_scanner", "r");
    if (scanner_file == NULL)
    {
        fprintf(stderr, "Error opening test_scanner\n");
        exit(1);
    }

    TsymItem *GST = NULL;
    symTabInit(&GST);
    fillTabWithKwds(&GST);

    TEST_SET_GROUP(grp_name);
    TToken tok = getToken(scanner_file, GST);
    TEST_EQ_INT(tok.type, TOK_ID, "Identifier test type", mode);
    TEST_EQ_INT(strcmp(tok.data.s, "identifier"), 0, "Identifier test value", mode);
    tok = getToken(scanner_file, GST);
    TEST_EQ_INT(tok.type, TOK_INT, "Integer 0 test type", mode);
    TEST_EQ_INT(tok.data.i, 0, "Integer 0 test value", mode);
    tok = getToken(scanner_file, GST);
    TEST_EQ_INT(tok.type, TOK_INT, "Integer test type", mode);
    TEST_EQ_INT(tok.data.i, 42, "Integer test value", mode);

    tokBuffInit();
    returnToken(tok);
    tok = getToken(scanner_file, GST);
    TEST_EQ_INT(tok.type, TOK_INT, "Integer return test type", mode);
    TEST_EQ_INT(tok.data.i, 42, "Integer return test value", mode);

    tok = getToken(scanner_file, GST);
    TEST_EQ_INT(tok.type, TOK_FLOAT, "Float 0.0 test", mode);
    tok = getToken(scanner_file, GST);
    TEST_EQ_INT(tok.type, TOK_FLOAT, "Float test", mode);
    tok = getToken(scanner_file, GST);
    TEST_EQ_INT(tok.type, TOK_FLOAT, "Float e test", mode);
    tok = getToken(scanner_file, GST);
    TEST_EQ_INT(tok.type, TOK_STRING, "String test", mode);
    TEST_EQ_INT(strcmp(tok.data.s, "Ahoj\n\"\tSve'te \\\x22\xfa\xf\x1a\xa1\x11\x2\x41\x48\x4F\x4a"), 0, "String test", mode);
           
    tok = getToken(scanner_file, GST);
    tok = getToken(scanner_file, GST);
    tok = getToken(scanner_file, GST);
    TEST_EQ_INT(tok.type, TOK_EOL, "\'\\n\' test", mode);

    tok = getToken(scanner_file, GST);
    TEST_EQ_INT(errflg, ERR_LEXICAL, "Invalid token test", mode);
    TEST_EQ_INT(tok.type, TOK_ERR, "Invalid token test", mode);
    tok = getToken(scanner_file, GST);
    TEST_EQ_INT(tok.type, TOK_ID, "Identifier test", mode);    
    tok = getToken(scanner_file, GST);
    TEST_EQ_INT(tok.type, TOK_EOL, "EOL test", mode);

    tok = getToken(scanner_file, GST);
    TEST_EQ_INT(tok.type, TOK_ADD, "+ test", mode);
    tok = getToken(scanner_file, GST);
    TEST_EQ_INT(tok.type, TOK_SUB, "- test", mode);
    tok = getToken(scanner_file, GST);
    TEST_EQ_INT(tok.type, TOK_MUL, "* test", mode);
    tok = getToken(scanner_file, GST);
    TEST_EQ_INT(tok.type, TOK_DIV, "/ test", mode);
    tok = getToken(scanner_file, GST);
    TEST_EQ_INT(tok.type, TOK_LT, "< test", mode);
    tok = getToken(scanner_file, GST);
    TEST_EQ_INT(tok.type, TOK_GT, "> test", mode);
    tok = getToken(scanner_file, GST);
    TEST_EQ_INT(tok.type, TOK_LEQ, "<= test", mode);
    tok = getToken(scanner_file, GST);
    TEST_EQ_INT(tok.type, TOK_GEQ, ">= test", mode);
    tok = getToken(scanner_file, GST);
    TEST_EQ_INT(tok.type, TOK_EQ, "== test", mode);
    tok = getToken(scanner_file, GST);
    TEST_EQ_INT(tok.type, TOK_NEQ, "!= test", mode);
    tok = getToken(scanner_file, GST);
    TEST_EQ_INT(tok.type, TOK_ASSIGN, "= test", mode);
    tok = getToken(scanner_file, GST);
    TEST_EQ_INT(tok.type, TOK_LBR, "( test", mode);
    tok = getToken(scanner_file, GST);
    TEST_EQ_INT(tok.type, TOK_RBR, ") test", mode);
    tok = getToken(scanner_file, GST);
    TEST_EQ_INT(tok.type, TOK_COMMA, ", test", mode);
    tok = getToken(scanner_file, GST);
    TEST_EQ_INT(tok.type, TOK_EOL, "\'\\n\' test", mode);
    tok = getToken(scanner_file, GST);    
}

void test_string(char *grp_name, int mode) {
    TEST_SET_GROUP(grp_name);
    unsigned err;
    string str = stringInit(&err);
    charPut(&str, 'a');
    TEST_EQ_INT(strcmp(str, "a"), 0, "str == \"a\"", mode);
}

void test_conv(char *grp_name, int mode)
{
    TEST_SET_GROUP(grp_name);
    TEST_EQ_INT(convStrToInt("12345"), 12345, "12345 test", mode);
    TEST_EQ_INT(convStrToInt("0"), 0, "0 test", mode);
    TEST_EQ_INT(convStrToInt("0b101010"), 42, "0b101010 test", mode);
    TEST_EQ_INT(convStrToInt("0b0"), 0, "0b0 test", mode);
    TEST_EQ_INT(convStrToInt("010"), 8, "010 test", mode);
    TEST_EQ_INT(convStrToInt("00"), 0, "00 test", mode);
    TEST_EQ_INT(convStrToInt("0xaB5"), 2741, "0xaB5 test", mode);
    TEST_EQ_INT(convStrToInt("0x0"), 0, "0x0 test", mode);
}

void test_parser(char *grp_name, int mode)
{
    errflg = 0;
    TEST_SET_GROUP(grp_name);
    FILE *f = fopen("tests/test_parser", "r");
    if (f == NULL)
    {
        fprintf(stderr, "Error opening test_parser\n");
        exit(1);
    }

    TsymItem *globalSymTable = NULL;
    TsymItem *localSymTable = NULL;
    symTabInit(&globalSymTable); //TODO: can throw error
    fillTabWithKwds(&globalSymTable); //TODO: can throw error
    symTabInit(&localSymTable); //TODO: can throw error
    parserStart(globalSymTable, localSymTable, f); //TODO: can throw error
    TEST_EQ_INT(errflg, 0, "Parsing test_parser file", mode);        
}

int main()
{
    errflg = 0;
    test_scanner("scanner.c tests", !COMPACT);
    test_string("dyn_arr.c tests", !COMPACT);
    test_conv("type_conv.c tests", !COMPACT);
    test_parser("parser.c tests", !COMPACT);
    TEST_PRINT_STATS();
}
