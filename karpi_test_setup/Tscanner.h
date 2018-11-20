#include <stdio.h>

typedef char* string; 

typedef enum {
    TOK_ERR,             //0 Unidentified token type
    TOK_ID,              // Identifier
    TOK_KEY,             // Keyword
    TOK_INT,             // Integer
    TOK_FLOAT,           // Float
    TOK_STRING,          // String
	TOK_NIL,			 //5
    TOK_ADD,             // +
    TOK_SUB,             // -
    TOK_MUL,             // *
    TOK_DIV,             // /
    TOK_LT,              //10 <
    TOK_GT,              // >
    TOK_LEQ,             // <=
    TOK_GEQ,             // >=
    TOK_EQ,              // ==
    TOK_NEQ,             //15 !=
    TOK_ASSIGN,          // =
    TOK_LEFT_BRACKET,    // (
    TOK_RIGHT_BRACKET,   // )
    TOK_COMMA,           // ,
    TOK_EOL,             //20 '\n'
	TOK_EOF,
	TOK_EXPR			 // extra EXPR token
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

// TODO: Entire parser just needs to know token type, in the future we will need to edit this function to return more parameters
TToken * getToken(FILE *f);