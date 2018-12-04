/**
 *	file name:		psa.c
 *	project:		VUT-FIT-IFJ-2018
 *	created:		23.11.2018
 *	last modified:	23.11.2018
 *	
 *	created by: 	Petr Bobčík xbobci02@stud.fit.vutbr.cz
 *    
 *	modifications:	Jakub Karpíšek xkarpi06@stud.fit.vutbr.cz
 *	
 *	description:	Precedence syntax analysis for expressions
 */

#include "psa.h"

#define NUMBER_OF_TOKENS 14
#define NO_ID 0

// delete token if token is not key word, End Of Line or End Of File
#define DELETE_TOKEN(get, delete) { \
                            if((get.type != TOK_KEY) && (get.type != TOK_EOL) && (get.type != TOK_EOF) )\
                            free(delete); \
                          }

// check if we token is key word, identifier or string and if true, than store this token for future freeing
#define TOK_PREP_FOR_DELETE(get, delete) { \
                                if(((get.type == TOK_KEY) || (get.type == TOK_ID) || (get.type == TOK_STRING)) && get.data.s != NULL)\
                                    delete = get.data.s;\
                          }
                          
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
    //printf("ROW: %d, COL: %d\n\n", row, col);
    if(row >= 0 && row < NUMBER_OF_TOKENS && col >= 0 && col < NUMBER_OF_TOKENS){
    	if(stackTopTok.data.s != NULL){
    		free(stackTopTok.data.s);  
        }  
		return precedenceTable[row][col];
	}
    else {
    	if(stackTopTok.data.s != NULL)
    		free(stackTopTok.data.s); 
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
                        }
                        else if(strcmp(token.data.s, "nil") == 0)
							return 12;
                        else 
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
	
	else if( (get.type == TOK_KEY) && (!strcmp(get.data.s, "nil")) ){
		result.val.s = get.data.s;
		result.type = ADRTYPE_NIL;
	}
	
	return result;
}


/*
 *  processExpression
 *  symTablePG == global symbol table
 *  symTablePL == local symbol table
 */ 
unsigned int processExpression(FILE *f, string followingToken, TsymItem *STG, TsymItem *STL, TInstrList *instrList, bool inWhile, bool inFunDef){
    
    static unsigned int psaCntr = 0;    // total amount of psa calls
    unsigned int Ecount  = 0;           // counter of element E in stack when reduce
    int EFirst = -1;                    // number of first E for generator
    int ESecond = -1;                   // number of second E for gennerator
    int ruleGet = 0;                    // number of rule that has to be apply

    string delete;      // using for token deleting
    string toReduce;    // string that has to be reduced

    TsymData *IDData = NULL;
    tStackLPtr s;       // stack
    TAdr IDKonst;

	if(!strcmp(followingToken, "eol"))
		s = sLInit(TOK_EOL, followingToken);  // followingToken == bottom of stack
	
    else
		s = sLInit(TOK_KEY, followingToken);  // followingToken == bottom of stack

    TToken get      = getToken(f, STG);                     // token got from scanner
    TToken ter      = highestTerminal(s);                   // highest terminal in stack
    char toDo       = lookInPrecedenceTable( ter, get );    //  get info what to do (reduce, shift,...)
    

    
	while( !sLEmpty(s) ){  
       if( get.type == TOK_ID ){
            if( (IDData = symTabSearch(STL, get.data.s)) == NULL){   // ID does not exist in Local ST,  
                sLDelete(s);
                
				ifjErrorPrint("psa ERROR in processExpression: Variable '%s' was not defined. ERROR %d\n", get.data.s, ERR_SEM_DEFINE);
                if(get.data.s != NULL)
					free(get.data.s);
                errflg = ERR_SEM_DEFINE;
				return 0;	// 0 means error == 0 E chars was found
            }
        }
               	
		//sPrintStack(s);
        switch(toDo){  
                 
			/*
             *  Reducing rule
             */ 
            case 'r':
				toReduce = sGetExprToReduce(s);       
			 	if( (ruleGet = findRule(toReduce)) != RULE_NOT_FOUND){  // try to find specific rule for reducing
					EFirst  = -1;
					ESecond = -1;

					if( (s == NULL) || (sLEmpty(s)) || (s->top == NULL) ){
						sLDelete(s);
						ifjErrorPrint("psa ERROR in processExpression: Stack is empty. ERROR %d\n", ERR_RUNTIME);
						errflg = ERR_RUNTIME;
						return 0;
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
					sLPop(s);  // pop shift char s
					   

					sLPush(s, "E", 15); // push E element into stack
                    switch(ruleGet){
                        // rule 0.
                        case ADD_RULE:          // +
                                genAdd(instrList, psaCntr, Ecount, ESecond, EFirst, inWhile, inFunDef);
					    	    //printf("Generuji '+' s E%d = E%d + E%d \n",Ecount, ESecond, EFirst);
                            break;

                        // rule 1.
                        case SUB_RULE:          // -
                                genSub(instrList, psaCntr, Ecount, ESecond, EFirst, inWhile, inFunDef);
						        //printf("Generuji '-' s E%d = E%d - E%d \n",Ecount, ESecond, EFirst);
                            break;

                        // rule 2.
                        case PLUSE_RULE:        // +E
                                genPos(instrList, psaCntr, Ecount, IDKonst, inWhile, inFunDef);
                               // printf("Generuji '+E' %s s E%d\n", IDKonst.val.s, Ecount);
                            break;

                        // rule 3.
                        case NEG_RULE:          // -E
                                genNeg(instrList, psaCntr, Ecount, IDKonst, inWhile, inFunDef);
                                //printf("Generuji '-E' %s s E%d\n", IDKonst.val.s, Ecount);
                            break;
                        
                        // rule 4.
                        case MUL_RULE:          // *
                                genMul(instrList, psaCntr, Ecount, ESecond, EFirst, inWhile, inFunDef);
						        //printf("Generuji '*' s E%d = E%d * E%d \n",Ecount, ESecond, EFirst);
                            break;

                        // rule 5.    
                        case DIV_RULE:          // :
                                genDiv(instrList, psaCntr, Ecount, ESecond, EFirst, inWhile, inFunDef);
						        //printf("Generuji ':' s E%d = E%d / E%d \n",Ecount, ESecond, EFirst);
                            break;

                        // rule 6.    
                        case LT_RULE:           // <
                                genLT(instrList, psaCntr, Ecount, ESecond, EFirst, inWhile, inFunDef);
						        //printf("Generuji '<' s E%d = E%d < E%d \n",Ecount, ESecond, EFirst);
                            break;

                        // rule 7. 
                        case GT_RULE:           // >
                                genGT(instrList, psaCntr, Ecount, ESecond, EFirst, inWhile, inFunDef);
						        //printf("Generuji '>' s E%d = E%d > E%d \n",Ecount, ESecond, EFirst);
                            break;

                        // rule 8.
                        case LEQ_RULE:          // <=
                                genLEQ(instrList, psaCntr, Ecount, ESecond, EFirst, inWhile, inFunDef);
						        //printf("Generuji '<=' s E%d = E%d <= E%d \n",Ecount, ESecond, EFirst);
                            break;

                        // rule 9.
                        case GEQ_RULE:          // >=
                                genGEQ(instrList, psaCntr, Ecount, ESecond, EFirst, inWhile, inFunDef);
						        //printf("Generuji '>=' s E%d = E%d >= E%d \n",Ecount, ESecond, EFirst);
                            break;

                        // rule 10.
                        case EQ_RULE:           // ==
                                genEQ(instrList, psaCntr, Ecount, ESecond, EFirst, inWhile, inFunDef);
						        //printf("Generuji '==' s E%d = E%d == E%d \n",Ecount, ESecond, EFirst);
                            break;                  

                        // rule 11.
                        case NEQ_RULE:          // !=
                                genNEQ(instrList, psaCntr, Ecount, ESecond, EFirst, inWhile, inFunDef);
						       //printf("Generuji '!=' s E%d = E%d != E%d \n",Ecount, ESecond, EFirst);
                            break; 

                        // rule 12.
                        case ID_RULE:           // ID
                                genE(instrList, psaCntr, Ecount, IDKonst, IDData->order, inWhile, inFunDef);
                                //printf("Generuji ID %s s E%d\n", IDKonst.val.s, Ecount);
                            break;

                        // rule 13.                    
                        case INT_RULE:          // int
                                //printf("Generuji INT ID %d s E%d\n", IDKonst.val.i, Ecount);
                                genE(instrList, psaCntr, Ecount, IDKonst, NO_ID, inWhile, inFunDef);
                            break;

                        // rule 14.
                        case FLOAT_RULE:        // float
                                //printf("Generuji FLOAT ID %f s E%d\n", IDKonst.val.f, Ecount);
                                genE(instrList, psaCntr, Ecount, IDKonst, NO_ID, inWhile, inFunDef);
                            break;

                        // rule 15.
                        case STRING_RULE:       // string
	                            //printf("Generuji STRING ID %s s E%d\n", IDKonst.val.s, Ecount);
	                    	    genE(instrList, psaCntr, Ecount, IDKonst, NO_ID, inWhile, inFunDef);
                            break;
                    }

					if( (ruleGet == LBR_E_RBR_RULE) ){
                        (s->top)->numberOfE = Ecount-1;     // add number of E to corresponding stack item
                    }
                    else
                        (s->top)->numberOfE = Ecount++;

					toDo = lookInPrecedenceTable( highestTerminal(s), get );
					if(toReduce != NULL)
						free(toReduce);					
				}

                else{
                    sLDelete(s);
                    ifjErrorPrint("psa ERROR in processExpression: Can't find corresponding rule for '%s'. ERROR %d\n",toReduce, ERR_LEXICAL);
					if(toReduce != NULL)
						free(toReduce);
				    errflg = ERR_SYNTAX;
					return 0;	
                }
                break;



            /*
             *  Shifting rule
             */
            case 's':
                sPlaceShiftChar( s );
                IDKonst = idValGet(get);
                if( (get.type == TOK_KEY) && (!strcmp(get.data.s, "nil")) )
                	get.type = TOK_STRING;
				sLPush(s, tokToStr(get), get.type);
                
                //TOK_PREP_FOR_DELETE(get, delete);   // check if token can be deleted 
                
                get = getToken(f, STG);
               
                //DELETE_TOKEN(get, delete);          // if it is possible to delete token, it deletes it
                
                toDo = lookInPrecedenceTable( highestTerminal(s), get );
                break;



            /*
             *  Equal rule
             */
            case 'e':
                IDKonst = idValGet(get);
                if( (get.type == TOK_KEY) && (!strcmp(get.data.s, "nil")) )
                	get.type = TOK_STRING;
                sLPush(s, tokToStr(get), get.type);

                //TOK_PREP_FOR_DELETE(get, delete);

                get = getToken(f, STG);

                //DELETE_TOKEN(get, delete);           

                toDo = lookInPrecedenceTable( highestTerminal(s), get );
                break;            
            


            /*
             *  Nothing/Empty rule
             */
            case 'X':
                if( ((get.type == TOK_KEY) && ( (!strcmp(get.data.s, "do")) || 
                    (!strcmp(get.data.s, "then"))) ) || (get.type == TOK_EOL) ){   // end of expression was found

					if(s->top == s->first->next){
		                returnToken(get);
		                psaCntr++;
		                sLPop(s);
		                sLPop(s);
		                break;             
                	}
                }

                /*if(((get.type == TOK_KEY) || (get.type == TOK_ID) || (get.type == TOK_STRING)) && get.data.s != NULL)
                    free(get.data.s);*/
              
                sLDelete(s);
                ifjErrorPrint("psa ERROR in processExpression: Error has occurred. ERROR %d\n", ERR_LEXICAL);
			    errflg = ERR_SYNTAX;
                return NO_E_NONTERM;  // // an error was found                   

            default:
                /*if(((get.type == TOK_KEY) || (get.type == TOK_ID) || (get.type == TOK_STRING)) &&get.data.s != NULL)
                    free(delete);*/
                sLDelete(s);
                ifjErrorPrint("psa ERROR in processExpression: Error has occurred. ERROR %d\n", ERR_LEXICAL);
                errflg = ERR_SYNTAX;
                return NO_E_NONTERM;
        }
       
    }

    sLDelete(s);
	return Ecount-1;
}



/** end of psa.c */
