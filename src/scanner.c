#include <ctype.h>
#include "scanner.h"
#include "dyn_arr.h"
#include <stdbool.h>

bool isOperator(char c)
{
    if (
        /*c == '+' ||
        c == '-' ||
        c == '*' ||
        c == '/' ||
        c == '<' ||
        c == '>' ||
        c == '!' ||
        c == '=' ||
        c == '(' ||
        c == ')'*/1        
    )
        return true;
    else
        return false;
}

token_type getToken(FILE *f)
{
    int c;
    int state = S_START;
    unsigned err;
    string buff = stringInit(&err);
    //TODO: Return err value to main if err

    while((c = getc(f)) != EOF)
    {
        //printf("CHAR READ: %c\n", c);
        switch(state)
        {
            case S_START:
                if (c >= 'a' && c <= 'z' || c == '_')
                {
                    charPut(buff, c, &err);
                    state = S_ID;
                }
                else if (c >= '1' && c <= '9')
                {
                    charPut(buff, c, &err);
                    state = S_INT;
                }
                else if (c == '0')
                {
                    charPut(buff, c, &err);
                    state = S_INT_ZERO;
                }
                else if (c == '\"')
                {
                    state = S_STR;
                }
                else if (c == '#')
                {
                    state = S_COMMENT;
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
                    charPut(buff, c, &err);
                }
                else if (c == '?' || c == '!')
                {
                    charPut(buff, c, &err);
                    state = S_START;
                    printf("Token read: %s\n", buff);
                    free(buff);
                    return TOK_ID;
                }
                else
                {
                    ungetc(c, f);
                    state = S_START;
                    printf("Token read: %s\n", buff);
                    free(buff);
                    return TOK_ID;
                }
                break;

            case S_INT:
                if (isdigit(c))
                {
                    charPut(buff, c, &err);
                }
                else if (c == '.')
                {
                    charPut(buff, c, &err);
                    state = S_FLOAT_DOT;
                }
                else if (isOperator(c) || isspace(c))
                {
                    /* TODO:
                        15=  15!=    15*     VALID INT
                        15a  15_     15"     INVALID INT
                        Think of all possible (in)valid suffixes of integer.
                        Create function isOperator()?                      */

                    ungetc(c, f);
                    state = S_START;
                    printf("Token read: %s\n", buff);
                    free(buff);
                    return TOK_INT;
                }
                else
                {
                    // TODO: Invalid token
                    state = S_START;
                    return TOK_ERR;
                }
                break;

            case S_INT_ZERO:
                if (c == '.')
                {
                    charPut(buff, c, &err);
                    state = S_FLOAT_DOT;
                }
                else if (isOperator(c) || isspace(c))
                {
                    // TODO: Same TODO as above
                    ungetc(c, f);
                    state = S_START;
                    printf("Token read: %s\n", buff);
                    free(buff);
                    return TOK_INT;
                }
                else
                {
                    // TODO: Invalid token
                    state = S_START;
                    return TOK_ERR;
                }
                // TODO: Implement BASE expansion here
                break;

            case S_FLOAT_DOT:
                if (isdigit(c))
                {
                    charPut(buff, c, &err);
                    state = S_FLOAT_DECIMAL;
                }
                else
                {
                    // TODO: Invalid float
                }
                break;

            case S_FLOAT_DECIMAL:
                if (isdigit(c))
                {
                    charPut(buff, c, &err);
                }
                else if (c == 'e' || c == 'E')
                {
                    charPut(buff, c, &err);
                    state = S_FLOAT_E;
                }
                else if (isOperator(c) || isspace(c))
                {
                    // TODO: if isOperator() -- VALID, else INVALID
                    ungetc(c, f);
                    state = S_START;
                    printf("Token read: %s\n", buff);
                    free(buff);
                    return TOK_FLOAT;
                }
                else
                {
                    // TODO: Invalid token
                    state = S_START;
                    return TOK_ERR;
                }                
                break;

            case S_FLOAT_E:
                if (c == '+' || c == '-')
                {
                    charPut(buff, c, &err);
                    state = S_FLOAT_E_SIGN;
                }
                else if (isdigit(c))
                {
                    charPut(buff, c, &err);
                    state = S_FLOAT_E_NUM;
                }
                else
                {
                    // TODO: Invalid float
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
                    charPut(buff, c, &err);
                }
                else if (isOperator(c) || isspace(c))
                {
                    // TODO: if isOperator() -- VALID, else INVALID
                    ungetc(c, f);
                    state = S_START;
                    printf("Token read: %s\n", buff);
                    free(buff);
                    return TOK_FLOAT;
                }
                else
                {
                    // TODO: Invalid token
                    state = S_START;
                    return TOK_ERR;
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
                    printf("Token read: %s\n", buff);
                    free(buff);
                    return TOK_STRING;
                }
                else if (c >= 32 && c <= 127)
                {
                    charPut(buff, c, &err);
                }
                break;

            case S_STR_ESC:
                if (c == 'x')
                {
                    state = S_STR_XH;
                }
                else if (c == '\"' || c == '\\' || c == 'n' || c == 't' || c == 's')
                {
                    switch (c)
                    {
                        case '\"': charPut(buff, '\"', &err); break;
                        case '\\': charPut(buff, '\\', &err); break;
                        case 'n': charPut(buff, '\n', &err); break;
                        case 't': charPut(buff, '\t', &err); break;
                        case 's': charPut(buff, ' ', &err); break;
                    }
                    state = S_STR;
                }
                break;

            case S_STR_XH:
                if (isdigit(c) || c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F')
                {
                    // TODO: Convert this number to a char
                    state = S_STR_XHH;
                }
                else
                {
                    // TODO: Invalid string
                }
                break;

            case S_STR_XHH:
                if (isdigit(c) || c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F')
                {
                    // TODO: Convert this number to a char
                    state = S_STR;
                }
                else
                {
                    ungetc(c, f);
                    state = S_STR;
                }
                break;
            
            case S_COMMENT:
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
                    ungetc(c, f);
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
                    ungetc(c, f);
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
                    ungetc(c, f);
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
                /*if (c == '=')
                {
                    //TODO: =begin comment case
                }
                else
                {*/
                    ungetc(c, f);
                    return TOK_EOL;
                /*}*/
                break;            
        } //switch
    } //while
}
