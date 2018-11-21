#include <ctype.h>
#include <stdbool.h>
#include "scanner.h"
#include "dyn_arr.h"
#include "type_conv.h"

struct {
    int count;
    TToken tok[2];
} tokBuff;

void tokBuffInit(){
    tokBuff.count = 0;
}

void returnToken(TToken tok){
    if (tokBuff.count < 2)
        tokBuff.tok[tokBuff.count++] = tok;
}

TToken getToken(FILE *f){
    if (tokBuff.count > 0)
        return tokBuff.tok[--tokBuff.count];

    int c;
    int state = S_START;
    int conv_int;
    int conv_esc;
    double conv_double;
    unsigned err;
    TToken tok;
    string buff = stringInit(&err);
    //TODO: Return err value to main if err

    while(c = getc(f)){
        /*DEBUG*///printf("CHAR READ: %c\n", c);
        switch(state){
            case S_START:
                if (c >= 'a' && c <= 'z' || c == '_'){
                    charPut(buff, c, &err);
                    state = S_ID;
                }
                else if (c >= '1' && c <= '9'){
                    charPut(buff, c, &err);
                    state = S_INT;
                }
                else if (c == '0'){
                    charPut(buff, c, &err);
                    state = S_INT_ZERO;
                }
                else if (c == '\"'){
                    state = S_STR;
                }
                else if (c == '#'){
                    state = S_COMMENT;
                }
                else if (c == '<'){
                    state = S_LESS;
                }
                else if (c == '>'){
                    state = S_GREATER;
                }
                else if (c == '='){
                    state = S_ASSIGN;
                }
                else if (c == '!'){
                    state = S_NOT;
                }
                else if (c == '\n'){
                    state = S_EOL;
                }
                else if (c == '+'){
                    tok.type = TOK_ADD;
                    return tok;
                }
                else if (c == '-'){
                    tok.type = TOK_SUB;
                    return tok;
                }
                else if (c == '*'){
                    tok.type = TOK_MUL;
                    return tok;
                }
                else if (c == '/'){
                    tok.type = TOK_DIV;
                    return tok;
                }
                else if (c == '('){
                    tok.type = TOK_LEFT_BRACKET;
                    return tok;
                }
                else if (c == ')'){
                    tok.type = TOK_RIGHT_BRACKET;
                    return tok;
                }
                else if (c == ','){
                    tok.type = TOK_COMMA;
                    return tok;
                }
                else if (c == EOF){
                    tok.type = TOK_EOF;
                    return tok;
                }
                else if (isspace(c)){
                    continue;
                }
                break;
            
            case S_ID:
                if (isalnum(c) || c == '_'){
                    charPut(buff, c, &err);
                }
                else if (c == '?' || c == '!'){
                    // This id can only be a function, not a variable
                    charPut(buff, c, &err);
                    state = S_START;
                    /*DEBUG*/printf("Token read: %s\n", buff);
                    tok.type = TOK_ID;
                    tok.data.s = buff;
                    //free(buff);
                    return tok;
                }
                else {
                    ungetc(c, f);
                    state = S_START;
                    /*DEBUG*/printf("Token read: %s\n", buff);
                    tok.type = TOK_ID;
                    tok.data.s = buff;
                    return tok;
                }
                break;

            case S_INT:
                if (isdigit(c)){
                    charPut(buff, c, &err);
                }
                else if (c == '.'){
                    charPut(buff, c, &err);
                    state = S_FLOAT_DOT;
                }
                else {
                    ungetc(c, f);
                    state = S_START;
                    tok.type = TOK_INT;
                    tok.data.i = convStrToInt(buff);
                    /*DEBUG*/printf("Token read: %s\n", buff);
                    /*DEBUG*/printf("Value: %d\n", tok.data.i);
                    free(buff);
                    return tok;
                }
                break;

            case S_INT_ZERO:
                if (c == '.'){
                    charPut(buff, c, &err);
                    state = S_FLOAT_DOT;
                }
                else if (c == 'b'){
                    charPut(buff, c, &err);
                    state = S_INT_BIN;
                }
                else if (c >= '0' && c <= '7'){
                    charPut(buff, c, &err);
                    state = S_INT_OCT;
                }
                else if (c == 'x'){
                    charPut(buff, c, &err);
                    state = S_INT_HEX;
                }
                else {
                    ungetc(c, f);
                    state = S_START;
                    tok.type = TOK_INT;
                    tok.data.i = 0;
                    /*DEBUG*/printf("Token read: %s\n", buff);
                    /*DEBUG*/printf("Value: %d\n", tok.data.i);
                    free(buff);
                    return tok;
                }
                break;

            case S_INT_BIN:
                if (c == '0' || c == '1'){
                    charPut(buff, c, &err);
                    state = S_INT_BIN_NUM;
                }
                else {
                    // TODO: Invalid token
                }
                break;
            case S_INT_BIN_NUM:
                if (c == '0' || c == '1'){
                    charPut(buff, c, &err);
                }
                else {
                    ungetc(c, f);
                    state = S_START;
                    tok.type = TOK_INT;
                    tok.data.i = convStrToInt(buff);
                    /*DEBUG*/printf("Token read: %s\n", buff);
                    /*DEBUG*/printf("Value: %d\n", tok.data.i);
                    free(buff);
                    return tok;
                }                
                break;

            case S_INT_OCT:
                if (c >= '0' && c <= '8'){
                    charPut(buff, c, &err);
                }
                else {
                    ungetc(c, f);
                    state = S_START;
                    tok.type = TOK_INT;
                    tok.data.i = convStrToInt(buff);
                    /*DEBUG*/printf("Token read: %s\n", buff);
                    /*DEBUG*/printf("Value: %d\n", tok.data.i);
                    free(buff);
                    return tok;
                }
                break;

            case S_INT_HEX:
                if (isdigit(c) || c >= 'A' && c <= 'F' || c >= 'a' && c <= 'f'){
                    charPut(buff, c, &err);
                    state = S_INT_HEX_NUM;
                }
                else {
                    // TODO: Invalid token
                }
                break;

            case S_INT_HEX_NUM:
                if (isdigit(c) || c >= 'A' && c <= 'F' || c >= 'a' && c <= 'f'){
                    charPut(buff, c, &err);
                    state = S_INT_HEX_NUM;
                }
                else {
                    ungetc(c, f);
                    state = S_START;
                    tok.type = TOK_INT;
                    tok.data.i = convStrToInt(buff);
                    /*DEBUG*/printf("Token read: %s\n", buff);
                    /*DEBUG*/printf("Value: %d\n", tok.data.i);
                    free(buff);
                    return tok;
                }
                break;

            case S_FLOAT_DOT:
                if (isdigit(c)){
                    charPut(buff, c, &err);
                    state = S_FLOAT_DECIMAL;
                }
                else {
                    // TODO: Invalid float
                }
                break;

            case S_FLOAT_DECIMAL:
                if (isdigit(c)){
                    charPut(buff, c, &err);
                }
                else if (c == 'e' || c == 'E'){
                    charPut(buff, c, &err);
                    state = S_FLOAT_E;
                }
                else {
                    // TODO: Is this correct?
                    ungetc(c, f);
                    state = S_START;
                    tok.type = TOK_FLOAT;
                    tok.data.f = convStrToDouble(buff);
                    /*DEBUG*/printf("Token read: %s\n", buff);
                    /*DEBUG*/printf("Value: %lf\n", tok.data.f);
                    free(buff);
                    return tok;
                }             
                break;

            case S_FLOAT_E:
                if (c == '+' || c == '-'){
                    charPut(buff, c, &err);
                    state = S_FLOAT_E_SIGN;
                }
                else if (isdigit(c)){
                    charPut(buff, c, &err);
                    state = S_FLOAT_E_NUM;
                }
                else {
                    // TODO: Invalid float
                }
                break;
            case S_FLOAT_E_SIGN:
                if (isdigit(c)){
                    state = S_FLOAT_E_NUM;
                }
                else {
                    // TODO: Invalid token
                }
                break;
            
            case S_FLOAT_E_NUM:
                if (isdigit(c)){
                    charPut(buff, c, &err);
                }
                else {
                    ungetc(c, f);
                    state = S_START;
                    tok.type = TOK_FLOAT;
                    tok.data.f = convStrToDouble(buff);
                    /*DEBUG*/printf("Token read: %s\n", buff);
                    /*DEBUG*/printf("Value: %lf\n", tok.data.f);
                    free(buff);
                    return tok;
                }               
                break;

            case S_STR:
                if (c == '\\'){
                    state = S_STR_ESC;
                }
                else if (c == '\"'){
                    state = S_START;                 
                    tok.type = TOK_STRING;
                    tok.data.s = buff;
                    /*DEBUG*/printf("Token read: %s\n", buff);
                    /*DEBUG*/printf("Value: %s\n", tok.data.s);
                    return tok;
                }
                else if (c >= 32 && c <= 127){
                    charPut(buff, c, &err);
                }
                else {
                    //TODO: Invalid token
                }
                break;

            case S_STR_ESC:
                if (c == 'x'){
                    state = S_STR_XH;
                }
                else if (c == '\"' || c == '\\' || c == 'n' || c == 't' || c == 's'){
                    switch (c){
                        case '\"': charPut(buff, '\"', &err); break;
                        case '\\': charPut(buff, '\\', &err); break;
                        case 'n': charPut(buff, '\n', &err); break;
                        case 't': charPut(buff, '\t', &err); break;
                        case 's': charPut(buff, ' ', &err); break;
                    }
                    state = S_STR;
                }
                else {
                    // TODO: Invalid token
                }
                break;

            case S_STR_XH:
                if (isdigit(c) || c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F'){
                    if (isdigit(c))
                        conv_esc = c - '0';
                    else
                        conv_esc = toupper(c) - 'A' + 10;
                    state = S_STR_XHH;
                }
                else {
                    // TODO: Invalid token
                }
                break;

            case S_STR_XHH:
                if (isdigit(c) || c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F'){
                    conv_esc *= 16;
                    if (isdigit(c))
                        conv_esc += c - '0';
                    else
                        conv_esc += toupper(c) - 'A' + 10;
                    charPut(buff, conv_esc, &err);
                    state = S_STR;
                }
                else {
                    charPut(buff, conv_esc, &err);
                    ungetc(c, f);
                    state = S_STR;
                }
                break;
            
            case S_COMMENT:
                if (c == '\n'){
                    state = S_START;
                }
                break;

            case S_LESS:
                if (c == '='){
                    state = S_START;
                    tok.type = TOK_LEQ;
                    return tok;
                }
                else {
                    ungetc(c, f);
                    state = S_START;
                    tok.type = TOK_LT;
                    return tok;
                }
                break;
            
            case S_GREATER:
                if (c == '='){
                    state = S_START;
                    tok.type = TOK_GEQ;
                    return tok;
                }
                else {
                    ungetc(c, f);
                    state = S_START;
                    tok.type = TOK_GT;
                    return tok;
                }
                break;
            
            case S_ASSIGN:
                if (c == '='){
                    state = S_START;
                    tok.type = TOK_EQ;
                    return tok;
                }
                else {
                    ungetc(c, f);
                    state = S_START;
                    tok.type = TOK_ASSIGN;
                    return tok;
                }

            case S_NOT:
                if (c == '='){
                    state = S_START;
                    tok.type = TOK_NEQ;
                    return tok;
                }
                break;

            case S_EOL:
                // TODO: MIGHT need to return EOL every time it's read, now it's ignored if block comment is being read
                if (c == '='){
                    //TODO: =begin comment case
                    state = S_COMMENT_E;
                }
                else {
                    ungetc(c, f);
                    tok.type = TOK_EOL;
                    return tok;
                }
                break;
            
            case S_COMMENT_E:           // Read: "\n="
                /*DEBUG*/printf("%d %c\n", state, c);
                if (c = 'b')
                    state = S_COMMENT_EB;
                else {
                    //TODO: Invalid token
                }
            break;
            
            case S_COMMENT_EB:          // Read: "\n=b"
                /*DEBUG*/printf("%d %c\n", state, c);
                if (c = 'e')
                    state = S_COMMENT_EBE;
                else {
                    //TODO: Invalid token
                }
            break;
            
            case S_COMMENT_EBE:         // Read: "\n=be"
                /*DEBUG*/printf("%d %c\n", state, c);
                if (c = 'g')
                    state = S_COMMENT_EBEG;
                else {
                    //TODO: Invalid token
                }
            break;
            
            case S_COMMENT_EBEG:        // Read: "\n=beg"
                /*DEBUG*/printf("%d %c\n", state, c);
                if (c = 'i')
                    state = S_COMMENT_EBEGI;
                else {
                    //TODO: Invalid token
                }
            break;
            
            case S_COMMENT_EBEGI:       // Read: "\n=begi"
                /*DEBUG*/printf("%d %c\n", state, c);
                if (c = 'n')
                    state = S_COMMENT_EBEGIN;
                else {
                    //TODO: Invalid token
                }
            break;
            
            case S_COMMENT_EBEGIN:      // Read: "\n=begin"
                /*DEBUG*/printf("%d %c\n", state, c);
                if (c == '\n')
                    state = S_COMMENT_BLOCK_EOL;
                else if (isspace(c))
                    state = S_COMMENT_BLOCK;
                else {
                    //TODO: Invalid token
                }
            break;
            
            case S_COMMENT_BLOCK:       // Skipping every char but '\n'
                /*DEBUG*/printf("%d %c\n", state, c);
                if (c == '\n')
                    state = S_COMMENT_BLOCK_EOL;
                else {
                    state = S_COMMENT_BLOCK;
                }
            break;
            
            case S_COMMENT_BLOCK_EOL:   // Read: "\n"
                /*DEBUG*/printf("%d %c\n", state, c);
                if (c == '\n')
                    state = S_COMMENT_BLOCK_EOL;
                else if (c == '=')
                    state = S_COMMENT_BLOCK_E;
                else {
                    state = S_COMMENT_BLOCK;
                }
            break;
            
            case S_COMMENT_BLOCK_E:     // Read: "\n="
                /*DEBUG*/printf("%d %c\n", state, c);
                if (c == '\n')
                    state = S_COMMENT_BLOCK_EOL;
                else if (c == 'e')
                    state = S_COMMENT_BLOCK_EE;
                else {
                    state = S_COMMENT_BLOCK;
                }
            break;
            
            case S_COMMENT_BLOCK_EE:    // Read: "\n=e"
                /*DEBUG*/printf("%d %c\n", state, c);
                if (c == '\n')
                    state = S_COMMENT_BLOCK_EOL;
                else if (c == 'n')
                    state = S_COMMENT_BLOCK_EEN;
                else {
                    state = S_COMMENT_BLOCK;
                }
            break;
            
            case S_COMMENT_BLOCK_EEN:   // Read: "\n=en"
                /*DEBUG*/printf("%d %c\n", state, c);
                if (c == '\n')
                    state = S_COMMENT_BLOCK_EOL;
                else if (c == 'd')
                    state = S_COMMENT_BLOCK_EEND;
                else {
                    state = S_COMMENT_BLOCK;
                }
            break;
            
            case S_COMMENT_BLOCK_EEND:  // Read: "\n=end"
                /*DEBUG*/printf("%d %c\n", state, c);
                if (c == '\n')
                    state = S_COMMENT_EOL_CHECK;
                else if (isspace(c))
                    state = S_COMMENT_END_SPACE;
                else
                    state = S_COMMENT_BLOCK;
            break;
            
            case S_COMMENT_END_SPACE:   // Read: "\n=end "
                /*DEBUG*/printf("%d %c\n", state, c);
                if (c == '\n')
                    state = S_COMMENT_EOL_CHECK;
                else
                    state = S_COMMENT_END_SPACE;
            break;

            case S_COMMENT_EOL_CHECK:   // Read: "\n=end \n"
                /*DEBUG*/printf("%d %c\n", state, c);
                if (c == '=')
                    state = S_COMMENT_E;
                else {
                    ungetc(c, f);
                    state = S_START;
                }
            break;
            

        } //switch
    } //while
}
