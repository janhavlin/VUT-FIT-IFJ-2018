/**
	file name:		psa.c
	project:		VUT-FIT-IFJ-2018
	created:		23.11.2018
	last modified:	23.11.2018
	
	created by: 	Jakub Karpíšek xkarpi06@stud.fit.vutbr.cz
	modifications:	
	
	description:	Precedence syntax analysis for expressions
*/
#include "psa.h"
#define NUMBER_OF_TOKENS 14

/**
 * returns symbol representing next action based on 2 tokens
 * @param stackTopTok is token currently at the top of stack
 * @param newToken is token received by scanner
 */
char * lookInPrecedenceTable(TToken stackTopTok, TToken newTok) {
    char precedenceTable[NUMBER_OF_TOKENS][NUMBER_OF_TOKENS] = {
    //   +    -    *    /    <    >   <=    >=    ==    !=     (    )    i    $  
        'r', 'r', 's', 's', 'r', 'r', 'r',  'r',  'r',  'r',  's', 'r', 's', 'r',
        'r', 'r', 's', 's', 'r', 'r', 'r',  'r',  'r',  'r',  's', 'r', 's', 'r',
        'r', 'r', 'r', 'r', 'r', 'r', 'r',  'r',  'r',  'r',  's', 'r', 's', 'r',
        'r', 'r', 'r', 'r', 'r', 'r', 'r',  'r',  'r',  'r',  's', 'r', 's', 'r',
        's', 's', 's', 's', 'r', 'r', 'r',  'r',  'r',  'r',  's', 'r', 's', 'r',
        's', 's', 's', 's', 'r', 'r', 'r',  'r',  'r',  'r',  's', 'r', 's', 'r',
        's', 's', 's', 's', 'r', 'r', 'r',  'r',  'r',  'r',  's', 'r', 's', 'r',
        's', 's', 's', 's', 'r', 'r', 'r',  'r',  'r',  'r',  's', 'r', 's', 'r',
        's', 's', 's', 's', 's', 's', 's',  's',  'r',  'r',  's', 'r', 's', 'r',
        's', 's', 's', 's', 's', 's', 's',  's',  'r',  'r',  's', 'r', 's', 'r',
        's', 's', 's', 's', 's', 's', 's',  's',  's',  's',  's', 'e', 's', 'X',
        'r', 'r', 'r', 'r', 'r', 'r', 'r',  'r',  'r',  'r',  'X', 'r', 'X', 'r',
        'r', 'r', 'r', 'r', 'r', 'r', 'r',  'r',  'r',  'r',  'E', 'r', 'E', 'r',
        's', 's', 's', 's', 's', 's', 's',  's',  's',  's',  's', 'E', 's', 'E'
    };
    int row = getIndex(stackTopTok);
    int col = getIndex(newTok);
    if(row >= 0 && row < NUMBER_OF_TOKENS && col >= 0 && col < NUMBER_OF_TOKENS)
        return precedenceTable[row][col];
    else {
        //index out of bounds
        return NULL;
    }
}

/**
 * returns correct index for each token it the precedence table
 */
int getIndex(TToken token) {
    switch (token.type) {
        case TOK_ADD:   return 0;
        case TOK_SUB:   return 1;
        case TOK_MUL:   return 2;
        case TOK_DIV:   return 3;
        case TOK_LT:    return 4;
        case TOK_GT:    return 5;
        case TOK_LEQ:   return 6;
        case TOK_GEQ:   return 7;
        case TOK_EQ:    return 8;
        case TOK_NEQ:   return 9;
        case TOK_LBR:   return 10;
        case TOK_RBR:   return 11;
        case TOK_ID:    return 12;
        case TOK_KEY:   if(strcmp(token.data.s, "then") == 0 || strcmp(token.data.s, "do") == 0) {
                           //continue to case $
                        } else 
                            break; 
        case TOK_EOL:   //$ symbol
                        return 13;
        default:        ifjErrorPrint("ERROR %d in psa.c in func. getIndex: unexpected token n. %d!\n", ERR_SYNTAX, token.type);
						errflg = ERR_SYNTAX;
                        break;
    }
    return -1;
}


/** end of psa.c */