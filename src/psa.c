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
        return '-';
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
        case TOK_INT:   return 12;
        case TOK_FLOAT: return 12;
        case TOK_STRING:return 12;
        case TOK_KEY:   if(strcmp(token.data.s, "then") == 0 || strcmp(token.data.s, "do") == 0) {
                           return 13;//continue to case $
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
	TToken result;	
	if(stack != NULL){
		tStackIPtr tmp = stack->top;	
		
		if(tmp != NULL){
		    
		    while( !strcmp( tmp->IdName, "E") ){
		        tmp = tmp->pred;
		    }
            
			result.data.s = (string) calloc(strlen(tmp->IdName)+1, sizeof(char));
			
			if(result.data.s != NULL){
				memcpy(result.data.s, tmp->IdName, strlen(tmp->IdName));
				result.type = tmp->type;
				return result;
			}
			else{
				ifjErrorPrint("psa ERROR in highestTerminal: Can't allocate 'result.data.s'. ERROR %d\n", ERR_RUNTIME);
				errflg = ERR_RUNTIME;
			}
		}

    	result.type = 0;
    	return result;
    }
	result.type = 0;
    return result;
}

char rules[AMOUNT_OF_RULES][MAX_RULE_LENGTH] = {
	{"ETOK_ADDE"},		// rule 0 
	{"ETOK_SUBE"},		// rule 1
	{"TOK_ADDE"},		// rule 2
	{"TOK_SUBE"},		// rule 3
	{"ETOK_MULE"},		// rule 4
	{"ETOK_DIVE"},		// rule 5
	{"ETOK_LTE"},		// rule 6
	{"ETOK_GTE"},		// rule 7
	{"ETOK_LEQE"},		// rule 8
	{"ETOK_GEQE"},		// rule 9
	{"ETOK_EQE"},		// rule 10
	{"ETOK_NEQE"},		// rule 11
	{"TOK_LBRETOK_RBR"},// rule 12
	{"TOK_ID"},			// rule 13
	{"TOK_INT"},		// rule 14
	{"TOK_FLOAT"},		// rule 15
	{"TOK_STRING"}		// rule 16
};

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
 * idValGet
 *  Store name of id or const value into union, that needs generator.
 */

TAdr idValGet(TToken get){
	TAdr result;

	//result.type = get.type;
	if( get.type == TOK_ID ){
		result.val.s = get.data.s;
		result.type = ADRTYPE_VAR;
	}

	else if( get.type == TOK_INT ){
		result.val.i = get.data.i;
		result.type = ADRTYPE_INT;
	}

	else if( get.type == TOK_FLOAT ){
		result.val.f = get.data.f;
		result.type = ADRTYPE_FLOAT;
	}

	else if( get.type == TOK_STRING ){
		result.val.s = get.data.s;
		result.type = ADRTYPE_STRING;
	}

	return result;
}


/*
 *  processExpression
 *  symTablePG == global symbol table
 *  symTablePL == local symbol table
 */ 
unsigned int processExpression(FILE *f, string followingToken, TsymItem *STG, TsymItem *STL, TInstrList *instrList, bool inWhile){

    unsigned int Ecount  = 0;   // counter of element E in stack when reduce
    int EFirst = -1;   // number of first E for generator
    int ESecond = -1;  // number of second E for gennerator
    static unsigned int psaCntr = 0;    // total amount of psa calls

    tStackLPtr s;
	
	if(strcmp(followingToken, "eol"))
		s = sLInit(TOK_KEY, followingToken);  // followingToken == bottom of stack
	else
		s = sLInit(TOK_EOL, followingToken);  // followingToken == bottom of stack

    TToken get      = getToken(f, STG);                // token got from scanner
    //printf("First token read in psa: %d\n", get.type); 
    TToken ter      = highestTerminal(s);   // highest terminal in stack
    char toDo       = lookInPrecedenceTable( ter, get );    //  get info what to do (reduce, shift,...)
    string toReduce;    // string that has to be reduced
    int ruleGet = 0;    // number

    TAdr IDKonst;
	while( 1 ){  
       if( get.type == TOK_ID ){
            if( symTabSearch(STG, get.data.s, NULL) && !symTabSearch(STL, get.data.s, NULL)){   // ID does not exist in Local ST, 
                  		                                                                                              // but exist in Global ST
                sLDelete(s);
				if(get.data.s != NULL)
					free(get.data.s);                
				ifjErrorPrint("psa ERROR in processExpression: Variable '%s' was not defined. ERROR %d\n", get.data.s, ERR_SEM_DEFINE);
                errflg = ERR_SEM_DEFINE;
				return 0;	// 0 means error == 0 E chars was found
            }
        }
        
		sPrintStack(s);
        switch(toDo){  
                 
			// reduce
            case 'r':
				toReduce = sGetExprToReduce(s);       
			 	if( (ruleGet = findRule(toReduce)) != RULE_NOT_FOUND){  // try to find specific rule for reducing
					EFirst  = -1;
					ESecond = -1;

					if( (s == NULL) || (sLEmpty(s)) || (s->top == NULL) ){
						sLDelete(s);
						ifjErrorPrint("psa ERROR in processExpression: Stack is empty. ERROR %d\n", ERR_RUNTIME);
						errflg = ERR_RUNTIME;
						return 0;	// 0 means error == 0 E chars was found
					}

					while( strcmp(s->top->IdName, "s") ){
						if(!strcmp(s->top->IdName, "E")){
							if( EFirst == -1 )
								EFirst = s->top->numberOfE;
							else   
								ESecond = s->top->numberOfE;
						}
						sLPop(s);
					}
					sLPop(s);   						// pop shift char s
					   

					sLPush(s, "E", 15);   				// push E element into stack
                    switch(ruleGet){
                        case ADD_RULE:          // +
                                genAdd(instrList, psaCntr, Ecount, ESecond, EFirst, inWhile);
					    	    printf("Generuji '+' s E%d = E%d + E%d \n",Ecount, ESecond, EFirst);
                            break;

                        case SUB_RULE:          // -
                                // genSUB(psaCntr, Ecount, Ecount-1, Ecount-2);
						        printf("Generuji '-' s E%d = E%d - E%d \n",Ecount, ESecond, EFirst);
                            break;

                        case MUL_RULE:          // *
                                // genMUL(psaCntr, Ecount, Ecount-1, Ecount-2);
						        printf("Generuji '*' s E%d = E%d * E%d \n",Ecount, ESecond, EFirst);
                            break;
                            
                        case DIV_RULE:          // :
                                //genDIV(psaCntr, Ecount, Ecount-1, Ecount-2);
						        printf("Generuji ':' s E%d = E%d / E%d \n",Ecount, ESecond, EFirst);
                            break;
                            
                        case LT_RULE:          // <
                                genLT(instrList, psaCntr, Ecount, ESecond, EFirst, inWhile);
						        printf("Generuji '<' s E%d = E%d < E%d \n",Ecount, ESecond, EFirst);
                            break;

                        case GT_RULE:          // >
                                genGT(instrList, psaCntr, Ecount, ESecond, EFirst, inWhile);
						        printf("Generuji '>' s E%d = E%d > E%d \n",Ecount, ESecond, EFirst);
                            break;

                        case LEQ_RULE:          // <=
                                genLEQ(instrList, psaCntr, Ecount, ESecond, EFirst, inWhile);
						        printf("Generuji '<=' s E%d = E%d <= E%d \n",Ecount, ESecond, EFirst);
                            break;

                        case GEQ_RULE:          // >=
                                genGEQ(instrList, psaCntr, Ecount, ESecond, EFirst, inWhile);
						        printf("Generuji '>=' s E%d = E%d >= E%d \n",Ecount, ESecond, EFirst);
                            break;

                        case EQ_RULE:          // ==
                                genEQ(instrList, psaCntr, Ecount, ESecond, EFirst, inWhile);
						        printf("Generuji '==' s E%d = E%d == E%d \n",Ecount, ESecond, EFirst);
                            break;                  

                        case NEQ_RULE:          // !=
                                genNEQ(instrList, psaCntr, Ecount, ESecond, EFirst, inWhile);
						        printf("Generuji '!=' s E%d = E%d != E%d \n",Ecount, ESecond, EFirst);
                            break; 

                        case ID_RULE:           // id
                                genE(instrList, psaCntr, Ecount, IDKonst, inWhile);
                                printf("Generuji ID %s s E%d\n", IDKonst.val.s, Ecount);
                            break;

                        case INT_RULE:          // int
                                printf("Generuji INT ID %d s E%d\n", IDKonst.val.i, Ecount);
                                genE(instrList, psaCntr, Ecount, IDKonst, inWhile);
                            break;

                        case FLOAT_RULE:        // float
                                printf("Generuji FLOAT ID %f s E%d\n", IDKonst.val.f, Ecount);
                                genE(instrList, psaCntr, Ecount, IDKonst, inWhile);
                            break;

                        case STRING_RULE:       // string
	                          	printf("Generuji STRING ID %s s E%d\n", IDKonst.val.s, Ecount);
	                    		genE(instrList, psaCntr, Ecount, IDKonst, inWhile);
                            break;
                    }

					(s->top)->numberOfE = Ecount++;     // add number of E to corresponding stack item

					toDo = lookInPrecedenceTable( highestTerminal(s), get );
					if(toReduce != NULL)
						free(toReduce);
					
				}
                else{
                    sLDelete(s);
                    ifjErrorPrint("psa ERROR in processExpression: Can't find corresponding rule for '%s'. ERROR %d\n",toReduce, ERR_SYNTAX);
					if(toReduce != NULL)
						free(toReduce);
				    errflg = ERR_SYNTAX;
					return 0;	// 0 means error == 0 E chars was found
                }
                break;



            // shift
            case 's':
                sPlaceShiftChar( s );
                IDKonst = idValGet(get);
				sLPush(s, tokToStr(get), get.type);
                get = getToken(f, STG);
				toDo = lookInPrecedenceTable( highestTerminal(s), get );
                break;



            // equal
            case 'e':
                IDKonst = idValGet(get);
                sLPush(s, tokToStr(get), get.type);
                get = getToken(f, STG);
				toDo = lookInPrecedenceTable( highestTerminal(s), get );
                break;            
            


            // nothing
            case 'X':
                if( ((get.type == TOK_KEY) && ( (!strcmp(get.data.s, "do")) || 
                    (!strcmp(get.data.s, "then")) )) || (get.type == TOK_EOL) ){   // end of expression was found
					sLDelete(s);
                    returnToken(get);
                    psaCntr++;
                    return Ecount-1;                      
                }
                else{          // an error was found
                    sLDelete(s);
                    ifjErrorPrint("psa ERROR in processExpression: Error has occurred. ERROR %d\n", ERR_SYNTAX);
				    errflg = ERR_SYNTAX;
                    return NO_E_NONTERM;                    
                }
                break;

            default:
                sLDelete(s);
                ifjErrorPrint("psa ERROR in processExpression: Error has occurred. ERROR %d\n", ERR_SYNTAX);
                errflg = ERR_SYNTAX;
                return NO_E_NONTERM;
        }
       
    }
    return Ecount;
}



/** end of psa.c */
