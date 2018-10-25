#include <ctype.h>
#include "scanner.h"

token_type getToken(FILE *f)
{
    int c;
    int state = S_START;
    while((c = getc(f)) != EOF)
    {
        //printf("CHAR READ: %c\n", c);
        switch(state)
        {
            case S_START:
                if (c >= 'a' && c <= 'z' || c == '_')
                {
                    state = S_ID;
                }
                else if (c >= '1' && c <= '9')
                {
                    state = S_INT;
                }
                else if (c == '0')
                {
                    state = S_INT_ZERO;
                }
                else if (c == '\"')
                {
                    state = S_STR;
                }
                else if (c == '#')
                {
                    state = S_STR_COMMENT;
                }
                else if (c == '<')
                {
                    state = S_LESS;
                }
                else if (c == '>')
                {
                    state = S_GREATER;
                }
                else if (c == '=')
                {
                    state = S_ASSIGN;
                }
                else if (c == '!')
                {
                    state = S_NOT;
                }
                else if (c == '\n')
                {
                    state = S_EOL;
                }
                else if (c == '+')
                {
                    return TOK_ADD;
                }
                else if (c == '-')
                {
                    return TOK_SUB;
                }
                else if (c == '*')
                {
                    return TOK_MUL;
                }
                else if (c == '/')
                {
                    return TOK_DIV;
                }
                else if (c == '(')
                {
                    return TOK_LEFT_BRACKET;
                }
                else if (c == ')')
                {
                    return TOK_RIGHT_BRACKET;
                }
                else if (isspace(c))
                {
                    continue;
                }
                break;
            
            case S_ID:
                if (isalnum(c) || c == '_')
                {
                    
                }
                else if (c == '?' || c == '!')
                {
                    state = S_START;
                    return TOK_ID;
                }
                else
                {
                    state = S_START;
                    return TOK_ID;
                }
                break;

            case S_INT:
                if (isdigit(c))
                {

                }
                else if (c == '.')
                {
                    state = S_FLOAT_DOT;
                }
                else
                {
                    state = S_START;
                    return TOK_INT;
                }
                break;

            case S_INT_ZERO:
                if (c == '.')
                {
                    state = S_FLOAT_DOT;
                }
                else
                {
                    state = S_START;
                    return TOK_INT;
                }
                break;

            case S_FLOAT_DOT:
                if (isdigit(c))
                {
                    state = S_FLOAT_DECIMAL;
                }
                break;

            case S_FLOAT_DECIMAL:
                if (isdigit(c))
                {
                    
                }
                else if (c == 'e' || c == 'E')
                {
                    state = S_FLOAT_E;
                }
                else
                {
                    state = S_START;
                    return TOK_FLOAT;
                }
                break;

            case S_FLOAT_E:
                if (c == '+' || c == '-')
                {
                    state = S_FLOAT_E_SIGN;
                }
                else if (isdigit(c))
                {
                    state = S_FLOAT_E_NUM;
                }
                break;
            case S_FLOAT_E_SIGN:
                if (isdigit(c))
                {
                    state = S_FLOAT_E_NUM;
                }
                break;
            
            case S_FLOAT_E_NUM:
                if (isdigit(c))
                {

                }
                else
                {
                    state = S_START;
                    return TOK_FLOAT;
                }
                break;

            case S_STR:
                if (c == '\\')
                {
                    state = S_STR_ESC;
                }
                else if (c == '\"')
                {
                    state = S_START;
                    return TOK_STRING;
                }
                else if (c >= 32 && c <= 127)
                {

                }
                break;

            case S_STR_ESC:
                if (c == 'x')
                {
                    state = S_STR_XH;
                }
                else if (c == '\"' || c == '\\' || c == 'n' || c == 't' || c == 's')
                {
                    state = S_STR;
                }
                break;

            case S_STR_XH:
                if (isdigit(c) || c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F')
                {
                    state = S_STR_XHH;
                } 
                break;

            case S_STR_XHH:
                if (isdigit(c) || c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F')
                {
                    state = S_STR;
                }
                else
                {
                    state = S_STR;
                }
                break;
            
            case S_STR_COMMENT:
                if (c == '\n')
                {
                    state = S_START;
                }
                break;

            case S_LESS:
                if (c == '=')
                {
                    state = S_START;
                    return TOK_LESSEQ;
                }
                else
                {
                    state = S_START;
                    return TOK_LESS;
                }
                break;
            
            case S_GREATER:
                if (c == '=')
                {
                    state = S_START;
                    return TOK_GREATEREQ;
                }
                else
                {
                    state = S_START;
                    return TOK_GREATER;
                }
                break;
            
            case S_ASSIGN:
                if (c == '=')
                {
                    state = S_START;
                    return TOK_EQ;
                }
                else
                {
                    state = S_START;
                    return TOK_ASSIGN;
                }

            case S_NOT:
                if (c == '=')
                {
                    state = S_START;
                    return TOK_NEQ;
                }
                break;

            case S_EOL:
                if (c == '=')
                {
                    //TODO: =begin comment case
                }
                else
                {
                    ungetc(c, f);
                    return TOK_EOL;
                }
                break;            
        }
    }
}
