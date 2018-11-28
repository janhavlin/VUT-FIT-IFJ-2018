/**
	file name:		parser.c
	project:		VUT-FIT-IFJ-2018
	created:		19.11.2018
	last modified:	26.11.2018
	
	created by: 	Jakub Karpíšek xkarpi06@stud.fit.vutbr.cz
	modifications:	
	
	description:	Recursive Descent parser				
*/

#include "parser.h"

//--------------GLOBAL VARIABLES-------------------------

/**
 * Keep track about main and current LT and GT
 */ 
TsymItem *mainLT;
TsymItem *currLT;
TsymItem *currGT;
/**
 * Keep track about main input file
 */
FILE *f; 

//--------------RECURSIVE DESCENT------------------------

int parse(FILE *input, TsymItem *GT, TsymItem *LT) {
	int result = PROGRAM_OK;
	mainLT = LT;
	currLT = LT;
	currGT = GT;
	f = input;
	bool success = false;
	TToken *token = (TToken *) malloc(sizeof(TToken));
	if (token == NULL) {
		errflg = ERR_RUNTIME;
		ifjErrorPrint("SYNTAX ERROR %d in parser.c in func. parse: allocation failed!\n", errflg);
		return errflg;
	}
	TToken **tokenPP = &token;
	
	**tokenPP = getToken(f, currGT);
	if (errflg == ERR_LEXICAL) return;
	
	/*START OF PARSING*/
	result = start(tokenPP);
	/*END OF PARSING*/

	//TODO: check if all functions are defined

	free(token);
	return result;
}

/**
	represents START non-terminal
	
	assert *tokenPP != NULL
*/
int start(TToken **tokenPP) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	switch (type) {
		case TOK_KEY:	if (strcmp(keyW, "def") == 0) {	
							//rule #2 START -> FUN-DEF eol S
							//process FUN-DEF
							if (fundef(tokenPP) != PROGRAM_OK) return errflg;
							//process eol
							if (eol(tokenPP) != PROGRAM_OK) return errflg;
							//process S
							if (start(tokenPP) != PROGRAM_OK) return errflg;
							return PROGRAM_OK;
						} else if (strcmp(keyW, "if") == 0 || strcmp(keyW, "while") == 0) {
							//continue to rule #1 
						} else
							break;	//error
		case TOK_EOL:
		case TOK_ID:	//rule #1 START -> STAT eol S
						//process STAT
						if (stat(tokenPP) != PROGRAM_OK) return errflg;
						//process eol
						if (eol(tokenPP) != PROGRAM_OK) return errflg;
						//process S
						if (start(tokenPP) != PROGRAM_OK) return errflg;
						return PROGRAM_OK;
		case TOK_EOF:	//rule #3 START -> eof
						genSTOP();
						return PROGRAM_OK;
		default:		break;						
	}
	errflg = ERR_SYNTAX;
	ifjErrorPrint("SYNTAX ERROR %d in parser.c in func. start: unexpected token n. %d!\n", errflg, type);
	return errflg;
}

/**
	represents STAT non-terminal
*/
int stat(TToken **tokenPP) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	switch (type) {
		case TOK_ID:	//rule #10 STAT -> id ASS-OR-FUN
						TToken savedIdOne = **tokenPP;	//save ID for later usage
						**tokenPP = getToken(f, currGT);	//get next token
						if (errflg != PROGRAM_OK) return errflg;	//check for lexical error from scanner
						//process ASS-OR-FUN
						if (assorfun(tokenPP, &savedIdOne) != PROGRAM_OK) return errflg;
						return PROGRAM_OK;
		case TOK_KEY:	if (strcmp(keyW, "if") == 0) {
							//rule #11 STAT -> if expr then eol ST-LIST else eol ST-LIST end
							//**tokenPP = getToken(f, currGT); //if is present, call next token  DONT GET NEXT TOKEN
							value = tif(tokenPP) && processExpression(f, currGT, currLT) && then(tokenPP) && eol(tokenPP) && 
									stlist(tokenPP) && telse(tokenPP) && eol(tokenPP) && stlist(tokenPP) && end(tokenPP);
						} else if (strcmp(keyW, "while") == 0) {
							//rule #12 STAT -> while expr do eol ST-LIST end
							//**tokenPP = getToken(f, currGT); //while is present, call next token DONT GET NEXT TOKEN
							value = tWhile(tokenPP) && processExpression(f, currGT, currLT) && tdo(tokenPP) && 
									eol(tokenPP) && stlist(tokenPP) && end(tokenPP);
						}
						break;
		case TOK_EOL:	//rule #13 STAT -> eps
						return true;
		default:		ifjErrorPrint("SYNTAX ERROR %d in parser.c in func. stat: unexpected token n. %d!\n", ERR_SYNTAX, type);
						errflg = ERR_SYNTAX;
						break;							
	}
	return value;
}

/**
	represents FUN-DEF non-terminal
*/
bool fundef(TToken **tokenPP) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	
	if (type == TOK_KEY) {
		if (strcmp(keyW, "def") == 0) {
			//rule #6 FUN-DEF -> def id ( P-LIST ) eol ST-LIST end
			value = def(tokenPP) && id(tokenPP) && lbr(tokenPP) && plist(tokenPP) && rbr(tokenPP) &&
					eol(tokenPP) && stlist(tokenPP) && end(tokenPP);
		} 
	} else {
		ifjErrorPrint("SYNTAX ERROR %d in parser.c in func. fundef: unexpected token n. %d!\n", ERR_SYNTAX, type);
		errflg = ERR_SYNTAX;
	} 
	return value;
}

/**
	represents ST-LIST non-terminal
*/
bool stlist(TToken **tokenPP) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	switch (type) {
		case TOK_KEY:	if (strcmp(keyW, "else") == 0 || strcmp(keyW, "end") == 0) {
							//rule #5 ST-LIST -> eps
							return true;
						} else if (strcmp(keyW, "if") == 0 || strcmp(keyW, "while") == 0) {
							//continue to rule #4
						} else 
							break;
		case TOK_ID:
		case TOK_EOL:	//rule #4 ST-LIST -> STAT eol ST-LIST
						value = stat(tokenPP) && eol(tokenPP) && stlist(tokenPP);
						break;
		default:		ifjErrorPrint("SYNTAX ERROR %d in parser.c in func. stlist: unexpected token n. %d!\n", ERR_SYNTAX, type);
						errflg = ERR_SYNTAX;
						break;						
	}
	return value;
}

/**
	represents P-LIST non-terminal
*/
bool plist(TToken **tokenPP) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	switch (type) {
		case TOK_KEY:	if (strcmp(keyW, "nil") == 0) {
							//continue to rule #7
						} else 
							break;
		case TOK_ID:
		case TOK_STRING:
		case TOK_FLOAT:
		case TOK_INT:	//rule #7 P-LIST -> TERM P-LIST	
						value = term(tokenPP) && plist(tokenPP);
						break;
		case TOK_COMMA: //rule #8 P-LIST -> , TERM P-LIST
						value = comma(tokenPP) && term(tokenPP) && plist(tokenPP);
						break;
		case TOK_RBR:
		case TOK_EOL:	//rule #9 P-LIST -> eps
						return true;
		default:		ifjErrorPrint("SYNTAX ERROR %d in parser.c in func. plist: unexpected token n. %d!\n", ERR_SYNTAX, type);
						errflg = ERR_SYNTAX;
						break;						
	}
	return value;
}

/**
	represents TERM non-terminal
*/
bool term(TToken **tokenPP) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	switch (type) {
		case TOK_ID:	//rule #20 TERM -> id
						**tokenPP = getToken(f, currGT); //id is present, call next token
						return true;
		case TOK_STRING:
		case TOK_FLOAT:
		case TOK_INT:	//rule #21 TERM -> const
						return tconst(tokenPP);
		case TOK_KEY:	if (strcmp(keyW, "nil") == 0) {
							//rule #22 TERM -> nil
							return nil(tokenPP);
						} 
						break;
		default:		ifjErrorPrint("SYNTAX ERROR %d in parser.c in func. term: unexpected token n. %d!\n", ERR_SYNTAX, type);
						errflg = ERR_SYNTAX;
						break;						
	}
	return value;
}

/**
	represents ASS-OR-FUN non-terminal
*/
int assorfun(TToken **tokenPP, TToken *savedIdOne) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	switch (type) {
		case TOK_ASSIGN: //rule #14 ASS-OR-FUN -> = ASSIGN
						**tokenPP = getToken(f, currGT);	//= is present, call next token
						if (errflg != PROGRAM_OK) return errflg;	//check for lexical error in scanner
						//process Lvalue Var ID
						TsymData *data = NULL;
						if (!symTabSearch(currLT, savedIdOne->data->s, data)) {	//ID not present in current LT
							if (!symTabSearch(currGT, savedIdOne->data->s, data)) {	//ID not defined as function
								genDEFVAR(savedIdOne->data->s);
							} else {
								errflg = ERR_SEM_DEFINE;
								ifjErrorPrint("SEMANTICS ERROR %d in parser.c in func. assorfun. Identifier already taken.\n", errflg, type);
								return errflg;
							}
						} //else Variable already defined
						//process assign
						if (assign(tokenPP, savedIdOne) != PROGRAM_OK) return errflg;
						return PROGRAM_OK;
		case TOK_KEY:	if (strcmp(keyW, "nil") == 0) {
							//continue to rule #15
						} else 
							break;
		case TOK_COMMA:
		case TOK_LBR:
		case TOK_STRING:
		case TOK_FLOAT:
		case TOK_INT:
		case TOK_ID:
		case TOK_EOL:	//rule #15 ASS-OR-FUN -> P-BODY
						value = pbody(tokenPP);
						break;
		default:		ifjErrorPrint("SYNTAX ERROR %d in parser.c in func. assorfun: unexpected token n. %d!\n", ERR_SYNTAX, type);
						errflg = ERR_SYNTAX;
						break;						
	}
	return value;
}

/**
	represents ASSIGN non-terminal
*/
int assign(TToken **tokenPP, TToken *savedIdOne) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	switch (type) {
		case TOK_EOL:	//rule #23 ASSIGN -> eps
						return PROGRAM_OK;
		case TOK_KEY:	if (strcmp(keyW, "nil")) {
							//continue to rule#17
						} else 
							break;
		case TOK_LBR:
		case TOK_STRING:
		case TOK_FLOAT:
		case TOK_INT:
		case TOK_ADD:
		case TOK_SUB:	//rule #17 ASSIGN -> expr 
						//case 1 - found after examining 1 token, therefore return 1 token to scanner
						returnToken(**tokenPP);
						//**tokenPP = getToken(f, currGT); //expr is present, call next token //DONT CALL NEXT TOKEN
						int E = processExpression(f, currGT, currLT);
						if (errflg != PROGRAM_OK) return errflg;	//check for errors in PSA
						genASSIGN(savedIdOne->data->s, E);
						return PROGRAM_OK;
		case TOK_ID:	TToken savedIdTwo = **tokenPP;
						if (decideExprOrFunc(tokenPP) != PROGRAM_OK) return errflg;
						return PROGRAM_OK;
		default:		break;						
	}
	errflg = ERR_SYNTAX;
	ifjErrorPrint("SYNTAX ERROR %d in parser.c in func. assign: unexpected token n. %d!\n", errflg, type);
}

/**
 * decides if there is an expression or not
 * 
 * eventually passes program to PSA
 */
bool decideExprOrFunc(TToken **tokenPP) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	tokBuffInit();
	TToken bufferToken = **tokenPP;
	**tokenPP = getToken(f, currGT); //id is present, but I need another token to decide
	switch ((*tokenPP)->type) {
		case TOK_EOL:	//rules #17 and #16 both work, it is a simple function or id = id expression;
						returnToken(**tokenPP); //must return the eol though, so stat can read it
						return true;
		case TOK_ADD:             // +
		case TOK_SUB:             // -
		case TOK_MUL:             // *
		case TOK_DIV:             // /
		case TOK_LT:              // <
		case TOK_GT:              // >
		case TOK_LEQ:             // <=
		case TOK_GEQ:             // >=
		case TOK_EQ:              // ==
		case TOK_NEQ:   		  // !=		
						//rule #17 ASSIGN -> expr 
						//case 2 - found after examining 2 tokens, therefore return 2 tokens to scanner
						returnToken(bufferToken);
						returnToken(**tokenPP);
						return processExpression(f, currGT, currLT);
		case TOK_KEY:		if (strcmp((*tokenPP)->data.s, "nil")==0) {
							//continue to rule #18
						} else 
							break;
		case TOK_LBR:
		case TOK_STRING:
		case TOK_FLOAT:
		case TOK_INT:
		case TOK_ID:	//rule #18 ASSIGN -> id P-BODY
						//actually no need of returning any tokens to scanner
						return plist(tokenPP); 
		default:		ifjErrorPrint("SYNTAX ERROR %d in parser.c in func. decideExprOrFunc: unexpected token n. %d!\n", ERR_SYNTAX, (*tokenPP)->type);
						errflg = ERR_SYNTAX;
						break;
	}
	return false;
}

/**
	represents pbody non-terminal
	pbody represents syntax for inserting parameters when calling a function like '(a, b, c)'
*/
bool pbody(TToken **tokenPP) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	switch (type) {
		case TOK_LBR:	//rule #18 P-BODY -> ( P-LIST )
						**tokenPP = getToken(f, currGT); //( is present, call next token
						value = plist(tokenPP) && rbr(tokenPP);
						break;
		case TOK_KEY:	if(strcmp(keyW, "nil") == 0) {
							//continue to rule#19
						} else 
							break;
		case TOK_COMMA:
		case TOK_STRING:
		case TOK_FLOAT:
		case TOK_INT:
		case TOK_ID:
		case TOK_EOL:	//rule #19 P-BODY -> P-LIST
						value = plist(tokenPP);
						break;
		default:		ifjErrorPrint("SYNTAX ERROR %d in parser.c in func. pbody: unexpected token n. %d!\n", ERR_SYNTAX, type);
						errflg = ERR_SYNTAX;
						break;						
	}
	return value;
}

/**
	eol terminal
*/
int eol(TToken **tokenPP) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	TTokenType type = (*tokenPP)->type;
	if (type == TOK_EOL) {
		**tokenPP = getToken(f, currGT); //eol is present, call next token
		if (errflg != PROGRAM_OK) return errflg;
		return PROGRAM_OK;
	} else {
		errflg = ERR_SYNTAX;
		ifjErrorPrint("SYNTAX ERROR %d in parser.c in func. eol: unexpected token n. %d!\n", errflg, type);
		return errflg;
	}
}

/**
	if terminal
*/
bool tif(TToken **tokenPP) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	//----SEMANTICS----
	
	//----SEMANTICS----
	return true;
}

/**
	then terminal
*/
bool then(TToken **tokenPP) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	if (type == TOK_KEY && (strcmp(keyW, "then") == 0)) {
		**tokenPP = getToken(f, currGT); //then is present, call next token
		value = true;
	} else {
		ifjErrorPrint("SYNTAX ERROR %d in parser.c in func. then: unexpected token n. %d or keyword %s!\n", ERR_SYNTAX, type, keyW);
		errflg = ERR_SYNTAX;
	}
	return value;
}

/**
	else terminal
*/
bool telse(TToken **tokenPP) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	if (type == TOK_KEY && (strcmp(keyW, "else") == 0)) {
		**tokenPP = getToken(f, currGT); //else is present, call next token
		value = true;
	} else {
		ifjErrorPrint("SYNTAX ERROR %d in parser.c in func. telse: unexpected token n. %d or keyword %s!\n", ERR_SYNTAX, type, keyW);
		errflg = ERR_SYNTAX;
	}
	return value;
}

/**
 * while terminal
 */ 
bool tWhile(TToken **tokenPP) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	return true;
}

/**
 *	do terminal
 */
bool tdo(TToken **tokenPP) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	if (type == TOK_KEY && (strcmp(keyW, "do") == 0)) {
		**tokenPP = getToken(f, currGT); //do is present, call next token
		value = true;
	} else {
		ifjErrorPrint("SYNTAX ERROR %d in parser.c in func. tdo: unexpected token n. %d or keyword %s!\n", ERR_SYNTAX, type, keyW);
		errflg = ERR_SYNTAX;
	}
	return value;
}

/**
 * def terminal
 */ 
bool def(TToken **tokenPP) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	**tokenPP = getToken(f, currGT); //def is present, call next token
	return true;
}

/**
	end terminal
*/
bool end(TToken **tokenPP) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	if (type == TOK_KEY && (strcmp(keyW, "end") == 0)) {
		**tokenPP = getToken(f, currGT); //end is present, call next token
		value = true;
	} else {
		ifjErrorPrint("SYNTAX ERROR %d in parser.c in func. end: unexpected token n. %d or keyword %s!\n", ERR_SYNTAX, type, keyW);
		errflg = ERR_SYNTAX;
	}
	return value;
}


/**
	id terminal
*/
bool id(TToken **tokenPP) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	if (type == TOK_ID) {
		//----SEMANTICS----
			
		//----SEMANTICS----
		**tokenPP = getToken(f, currGT); //id is present, call next token
		value = true;
	} else {
		ifjErrorPrint("SYNTAX ERROR %d in parser.c in func. id: unexpected token n. %d!\n", ERR_SYNTAX, type);
		errflg = ERR_SYNTAX;
	}
	return value;
}

/**
 * const terminal
 */ 
bool tconst(TToken **tokenPP) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	**tokenPP = getToken(f, currGT); //const is present, call next token
	return true;
}

/**
 * nil terminal
 */ 
bool tconst(TToken **tokenPP) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	**tokenPP = getToken(f, currGT); //nil is present, call next token
	return true;
}

/**
	left bracket terminal
*/
bool lbr(TToken **tokenPP) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	if (type == TOK_LBR) {
		**tokenPP = getToken(f, currGT); //lbr is present, call next token
		value = true;
	} else {
		ifjErrorPrint("SYNTAX ERROR %d in parser.c in func. lbr: unexpected token n. %d!\n", ERR_SYNTAX, type);
		errflg = ERR_SYNTAX;
	}
	return value;
}

/**
 * comma terminal
 */ 
bool comma(TToken **tokenPP) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	**tokenPP = getToken(f, currGT); //comma is present, call next token
	return true;
}

/**
	right bracket terminal
*/
bool rbr(TToken **tokenPP) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	if (type == TOK_RBR) {
		**tokenPP = getToken(f, currGT); //rbr is present, call next token
		value = true;
	} else {
		ifjErrorPrint("SYNTAX ERROR %d in parser.c in func. rbr: unexpected token n. %d!\n", ERR_SYNTAX, type);
		errflg = ERR_SYNTAX;
	}
	return value;
}

/** end of parser.c */