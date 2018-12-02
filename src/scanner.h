/**
	file name:		scanner.h
	project:		VUT-FIT-IFJ-2018
	created:		24.10.2018
	last modified:	24.11.2018
	
	created by: 	Jan Havlín xhavli47@stud.fit.vutbr.cz
	modifications:	
	
	description:	Reads tokens from input stream
*/

#ifndef IFJ18_SCANNER_H
#define IFJ18_SCANNER_H

#include <stdio.h>
#include "dyn_arr.h"
#include "s_table.h"

typedef enum {
    TOK_ERR,             // Unidentified token type
    TOK_ID,              // Identifier
    TOK_KEY,             // Keyword
    TOK_INT,             // Integer
    TOK_FLOAT,           // Float
    TOK_STRING,          // String
    TOK_ADD,             // +
    TOK_SUB,             // -
    TOK_MUL,             // *
    TOK_DIV,             // /
    TOK_LT,              // <
    TOK_GT,              // >
    TOK_LEQ,             // <=
    TOK_GEQ,             // >=
    TOK_EQ,              // ==
    TOK_NEQ,             // !=
    TOK_ASSIGN,          // =
    TOK_LBR,             // (
    TOK_RBR,             // )
    TOK_COMMA,           // ,
    TOK_EOL,             // '\n'
	TOK_EOF
} TTokenType;

/*
struct t_token

contains type and data
*/
typedef struct {
	TTokenType type;
	union Data {
		int i;
		double f;
		string s; 
	} data;
} TToken;

typedef enum {
    S_START,
    S_ID,
    S_ID_END,
    S_INT,
    S_INT_ZERO,
    S_INT_BIN,
    S_INT_BIN_NUM,
    S_INT_OCT,
    S_INT_HEX,
    S_INT_HEX_NUM,
    S_FLOAT_DOT,
    S_FLOAT_DECIMAL,
    S_FLOAT_E,
    S_FLOAT_E_SIGN,
    S_FLOAT_E_NUM,
    S_STR,
    S_STR_ESC,
    S_STR_XH,
    S_STR_XHH,
    S_LESS,
    S_GREATER,
    S_NOT,
    S_ASSIGN,
    S_COMMENT,
    S_COMMENT_E,
    S_COMMENT_EB,
    S_COMMENT_EBE,
    S_COMMENT_EBEG,
    S_COMMENT_EBEGI,
    S_COMMENT_EBEGIN,
    S_COMMENT_BLOCK,
    S_COMMENT_BLOCK_EOL,
    S_COMMENT_BLOCK_E,
    S_COMMENT_BLOCK_EE,
    S_COMMENT_BLOCK_EEN,
    S_COMMENT_BLOCK_EEND,
    S_COMMENT_END_SPACE
} state_type;

// TODO: Entire parser just needs to know token type, in the future we will need to edit this function to return more parameters
void tokBuffInit();
void returnToken(TToken tok);
TToken getToken(FILE *f, TsymItem *symTable);
#endif //IFJ18_SCANNER_H