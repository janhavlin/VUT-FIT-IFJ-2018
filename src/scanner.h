#ifndef IFJ18_SCANNER_H
#define IFJ18_SCANNER_H

#include <stdio.h>

typedef enum { TOK_ERR,             // Unidentified token type
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
} token_type;

// TODO: Entire parser just needs to know token type, in the future we will need to edit this function to return more parameters
token_type get_token(FILE *f);
#endif //IFJ18_SCANNER_H
