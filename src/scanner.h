#ifndef IFJ18_SCANNER_H
#define IFJ18_SCANNER_H

#include <stdio.h>

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
    TOK_LESS,            // <
    TOK_GREATER,         // >
    TOK_LESSEQ,          // <=
    TOK_GREATEREQ,       // >=
    TOK_EQ,              // ==
    TOK_NEQ,             // !=
    TOK_ASSIGN,          // =
    TOK_LEFT_BRACKET,    // (
    TOK_RIGHT_BRACKET,   // )
    TOK_EOL,             // '\n'
} token_type;

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
    S_COMMENT,
    S_LESS,
    S_GREATER,
    S_NOT,
    S_ASSIGN,
    S_EOL,
} state_type;

// TODO: Entire parser just needs to know token type, in the future we will need to edit this function to return more parameters
token_type getToken(FILE *f);
#endif //IFJ18_SCANNER_H
