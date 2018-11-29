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
char lookInPrecedenceTable(TToken stackTopTok, TToken newTok) {
    char precedenceTable[NUMBER_OF_TOKENS][NUMBER_OF_TOKENS] = {
    //   +    -    *    /    <    >   <=    >=    ==    !=     (    )    i    $  
        {'r', 'r', 's', 's', 'r', 'r', 'r',  'r',  'r',  'r',  's', 'r', 's', 'r'},
        {'r', 'r', 's', 's', 'r', 'r', 'r',  'r',  'r',  'r',  's', 'r', 's', 'r'},
        {'r', 'r', 'r', 'r', 'r', 'r', 'r',  'r',  'r',  'r',  's', 'r', 's', 'r'},
        {'r', 'r', 'r', 'r', 'r', 'r', 'r',  'r',  'r',  'r',  's', 'r', 's', 'r'},
        {'s', 's', 's', 's', 'r', 'r', 'r',  'r',  'r',  'r',  's', 'r', 's', 'r'},
        {'s', 's', 's', 's', 'r', 'r', 'r',  'r',  'r',  'r',  's', 'r', 's', 'r'},
        {'s', 's', 's', 's', 'r', 'r', 'r',  'r',  'r',  'r',  's', 'r', 's', 'r'},
        {'s', 's', 's', 's', 'r', 'r', 'r',  'r',  'r',  'r',  's', 'r', 's', 'r'},
        {'s', 's', 's', 's', 's', 's', 's',  's',  'r',  'r',  's', 'r', 's', 'r'},
        {'s', 's', 's', 's', 's', 's', 's',  's',  'r',  'r',  's', 'r', 's', 'r'},
        {'s', 's', 's', 's', 's', 's', 's',  's',  's',  's',  's', 'e', 's', 'X'},
        {'r', 'r', 'r', 'r', 'r', 'r', 'r',  'r',  'r',  'r',  'X', 'r', 'X', 'r'},
        {'r', 'r', 'r', 'r', 'r', 'r', 'r',  'r',  'r',  'r',  'X', 'r', 'X', 'r'},
        {'s', 's', 's', 's', 's', 's', 's',  's',  's',  's',  's', 'X', 's', 'X'}
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

/*
 *  highestTerminal()
 * 		Find highest terminal in stack. Return string with its name.
 */
TToken highestTerminal( tStackLPtr stack ){
	tStackIPtr tmp = stack->top;
    TToken result;

    if(stack != NULL){
        
        while( !strcmp( tmp->IdName, "E") ){
            tmp=tmp->pred;
        }

        memcpy(result.data.s, tmp->IdName, strlen(tmp->IdName));
        result.type = TOK_ERR;
        return result;
    }
    
    result.data.s = "ERROR\0";
    result.type = TOK_ERR;
    return result;
}


/*
 *	FindRule()
 * 		While reducing items in struct, because of semantic analysiss,
 * 		it tries to find corresponding rule for that.
 */
int findRule( string readRule ){
    if(readRule != NULL){
	string help;
        for(unsigned int ruleNum = 0; ruleNum < AMOUNT_OF_RULES; ruleNum++){
		help = rules[ruleNum];         
            if( strcmp(readRule, help) == 0 ){    // if rule was found
                return ruleNum;    
            }
        }
    }
    return RULE_NOT_FOUND;
}


/*
 *  processExpression
 *  symTablePG == global symbol table
 *  symTablePL == local symbol table
 */ 
unsigned int processExpression(FILE * f, string followingToken, TsymItem *symTablePL, TsymItem *symTablePG){
    
    unsigned int Ecount  = 0;                               // counter of element E in stack when reduce
    static unsigned int psaCntr = 0;
    tStackLPtr s    = sLInit(followingToken, "TOK_KEY\0");  // followingToken == bottom of stack
    TToken get      = getToken(stdout, symTablePL);         // token got from scanner 
    char todo       = lookInPrecedenceTable( highestTerminal(s), get );     // reduce, shift, ...
    string toReduce;                                        // string to reduce
    TToken ter = highestTerminal(s);                        // highest terminal
    int ruleGet = 0;

    while((!strcmp(ter.data.s , followingToken)) && 
         (( get.type == TOK_KEY) && (!strcmp(highestTerminal(s).data.s, get.data.s)) &&
         (get.type == TOK_KEY))){    // key do, key then, eol

        todo = lookInPrecedenceTable(highestTerminal(s), get);
        if( get.type == TOK_ID ){
            
            //TODO:
            /*if(  ){
                kontrola, jestli je promenna v LT a ne GT
            }
            else{
                sLDelete(s);
                ifjErrorPrint("psa ERROR in processExpression: Variable %s was not defined. ERROR %d\n", get.data.s, ERR_SEM_DEFINE);
                return NO_E_NONTERM;
            }*/
        }

        switch(todo){
            
            // reduce
            case 'r':
                toReduce = sGetExprToReduce(s);
                
                if( (ruleGet = findRule(toReduce)) != RULE_NOT_FOUND){
                    while(s->top->IdName != "s"){
                        sLPop(s);
                    }

                    sLPop(s);
                    sLPush(s, "E", "NON_TERM\0");
                    Ecount++;

                    if(ruleGet == ADD_RULE){        // E+E
                       // genADD(psaCntr, Ecount, Ecount-1, Ecount-2);
                        printf("Generuji ADD s E%d = E%d + E%d",Ecount, Ecount-1, Ecount-2);
                    }

                    else if(ruleGet == SUB_RULE){   // E-E
                       // genSUB(psaCntr, Ecount, Ecount-1, Ecount-2);
                        printf("Generuji SUB s E%d = E%d - E%d",Ecount, Ecount-1, Ecount-2);
                    }

                    else if(ruleGet == MUL_RULE){   // E*E
                       // genMUL(psaCntr, Ecount, Ecount-1, Ecount-2);
                        printf("Generuji MUL s E%d = E%d * E%d",Ecount, Ecount-1, Ecount-2);
                    }

                    else if(ruleGet == DIV_RULE){   // E/E
                       // genDIV(psaCntr, Ecount, Ecount-1, Ecount-2);
                        printf("Generuji DIV s E%d = E%d / E%d",Ecount, Ecount-1, Ecount-2);
                    }


                }
                else{
                    sLDelete(s);
                    ifjErrorPrint("psa ERROR in processExpression: Can't find corresponding rule for %s. ERROR %d\n",toReduce, ERR_SYNTAX);
				    errflg = ERR_SYNTAX;
                    return NO_E_NONTERM;
                }
                break;

            // shift
            case 's':
                sLPush(s, "s", "NULL\0");
                sLPush(s, get.data.s, get.type);
                get = getToken(stdout, symTablePL);
                break;

            // equal
            case 'e':
                sLPush(s, get.data.s, get.type);
                get = getToken(stdout, symTablePL);
                break;            
            
            // nothing
            case 'X':
                if( (!strcmp(get.data.s, followingToken)) || (get.type == TOK_KEY) ){
                    sLDelete(s);
                    returnToken(get);
                    return psaCntr++;
                }
                else{
                    sLDelete(s);
                    ifjErrorPrint("psa ERROR in processExpression: Error has occurred. ERROR %d\n", ERR_SYNTAX);
				    errflg = ERR_SYNTAX;
                    return NO_E_NONTERM;                    
                }
                break;
        }
       
    }
}




/** end of psa.c */
