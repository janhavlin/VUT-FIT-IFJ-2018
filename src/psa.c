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
unsigned int processExpression(FILE * f, string followingToken, TsymItem *STG, TsymItem *STL){
    
    unsigned int Ecount  = 0;   // counter of element E in stack when reduce
    unsigned int EFirst = -1;   // number of first E for generator
    unsigned int ESecond = -1;  // number of second E for gennerator
    static unsigned int psaCntr = 0;    // total amount of psa calls
    tStackLPtr s    = sLInit(followingToken, "TOK_KEY\0");  // followingToken == bottom of stack
    TToken get      = getToken(stdout, STL);                // token got from scanner 
    TToken ter      = highestTerminal(s);   // highest terminal in stack
    char toDo       = lookInPrecedenceTable( ter, get );    //  get info what to do (reduce, shift,...)
    string toReduce;    // string that has to be reduced
    int ruleGet = 0;    // number

    
    while(s->first != s->top){    

        toDo = lookInPrecedenceTable(highestTerminal(s), get);
        if( get.type == TOK_ID ){
            
            if( ( symTableSearch(STG, get.data.s) != NULL ) && ( symTableSearch(STL, get.data.s) == NULL ) ){   // ID does not exist in Local ST, 
                                                                                                                // but exist in Global ST
                sLDelete(s);
                ifjErrorPrint("psa ERROR in processExpression: Variable %s was not defined. ERROR %d\n", get.data.s, ERR_SEM_DEFINE);
                errflg = ERR_SEM_DEFINE;
                return NO_E_NONTERM;
            }
        }

        switch(toDo){
            
            // reduce
            case 'r':
                toReduce = sGetExprToReduce(s);
                
                if( (ruleGet = findRule(toReduce)) != RULE_NOT_FOUND){  // try to find specific rule for reducing
                    EFirst  = -1;
                    ESecond = -1;
                   
                    while(s->top->IdName != "s"){
                        if(s->top->IdName != "E"){
                            if( EFirst == -1 )
                                EFirst = s->top->numberOfE;
                            else   
                                ESecond = s->top->numberOfE;
                        }
                        sLPop(s);
                    }
                    sLPop(s);   // pop shift char s
                    sLPush(s, "E", "NON_TERM\0");   // push E element into stack
                    (s->top)->numberOfE = Ecount++;     // add number of E to corresponding stack item

                    if(ruleGet == ADD_RULE){        // E+E
                       // genADD(psaCntr, Ecount, ESecond, EFirst);
                        printf("Generuji ADD s E%d = E%d + E%d \n",Ecount, ESecond, EFirst);
                    }

                    else if(ruleGet == SUB_RULE){   // E-E
                       // genSUB(psaCntr, Ecount, Ecount-1, Ecount-2);
                        printf("Generuji SUB s E%d = E%d - E%d \n",Ecount, ESecond, EFirst);
                    }

                    else if(ruleGet == MUL_RULE){   // E*E
                       // genMUL(psaCntr, Ecount, Ecount-1, Ecount-2);
                        printf("Generuji MUL s E%d = E%d * E%d \n",Ecount, ESecond, EFirst);
                    }

                    else if(ruleGet == DIV_RULE){   // E/E
                       // genDIV(psaCntr, Ecount, Ecount-1, Ecount-2);
                        printf("Generuji DIV s E%d = E%d / E%d \n",Ecount, ESecond, EFirst);
                    }

                    else if(ruleGet == ID_RULE){   // E from ID
                        printf("Generuji ID s E%d\n", Ecount);
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
                void sPlaceShiftChar( tStackLPtr s );
                sLPush(s, get.data.s, get.type);
                get = getToken(stdout, STL);
                break;


            // equal
            case 'e':
                sLPush(s, get.data.s, get.type);
                get = getToken(stdout, STL);
                break;            
            

            // nothing
            case 'X':
                if( (get.type == TOK_KEY) && ( (!strcmp(get.data.s, "do")) || 
                    (!strcmp(get.data.s, "then")) || (!strcmp(get.data.s, "eol")) ) ){   // end of expression was found
                    sLDelete(s);
                    returnToken(get);
                    return Ecount;                      
                }
                else{          // an error was found
                    sLDelete(s);
                    ifjErrorPrint("psa ERROR in processExpression: Error has occurred. ERROR %d\n", ERR_SYNTAX);
				    errflg = ERR_SYNTAX;
                    return NO_E_NONTERM;                    
                }
                break;
        }
       
    }
    return Ecount;
}




/** end of psa.c */
