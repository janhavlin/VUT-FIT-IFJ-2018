/**
	file name:		parser.c
	project:		VUT-FIT-IFJ-2018
	created:		19.11.2018
	last modified:	29.11.2018
	
	created by: 	Jakub Karpíšek xkarpi06@stud.fit.vutbr.cz
	modifications:	
	
	description:	Recursive Descent parser				
*/

#include "parser.h"

//--------------RECURSIVE DESCENT------------------------

/**
 * takes care about errors called in parser.c
 * @param errNbr is error number
 * @param where is name of parser.c function where the error occured
 * @tokenType is number of token that came (only used for syntax error)
 */ 
int parserError(int errNbr, string where, int tokenType) {
	errflg = errNbr;
	switch(errNbr):
		case ERR_SEM_DEFINE:	ifjErrorPrint("SEMANTIC ERROR %d in parser.c in func. %s: Undefined variable or redefinition!\n", errflg, where); break;
		case ERR_SEM_PARAM:		ifjErrorPrint("SEMANTIC ERROR %d in parser.c in func. %s: wrong amount of parameters!\n", errflg, where); break;
		case ERR_SEM_OTHER:		ifjErrorPrint("SEMANTIC ERROR %d in parser.c in func. %s: can't define function with constant parameter!\n", errflg, where); break;
		case ERR_SYNTAX:		if (tokenType != TOK_KEY)
									ifjErrorPrint("SYNTAX ERROR %d in parser.c in func. %s: unexpected token type n.%d!\n", errflg, where, tokenType); 
								else
									ifjErrorPrint("SYNTAX ERROR %d in parser.c in func. %s: unexpected token type n.%d or unexpected keyword!\n", errflg, where, tokenType); break;
		case ERR_RUNTIME: 		ifjErrorPrint("RUNTIME ERROR %d in parser.c in func. %s: allocation failed!\n", errflg, where); break;
		default: break;
	return errflg;
}

void initTWrapper(TWrapper *info) {
	info->file = NULL;
	info->mainLT = NULL;
	info->currLT = NULL;
	info->currGT = NULL;
	info->whileCounter = 0;
	info->ifCounter = 0;
	info->psaCounter = 0;
	info->paramCounter = 0;
	info->inWhile = false;
	info->inFunCall = false;
}

int parse(FILE *input, TsymItem *GT, TsymItem *LT) {
	int result = PROGRAM_OK;
	TWrapper *globalInfo = (TWrapper *) malloc(sizeof(TWrapper));
	if (token == NULL) {
		return parserError(ERR_RUNTIME, "parse", 0);
	}
	initTWrapper(globalInfo);
	globalInfo->file = input;
	globalInfo->mainLT = LT;
	globalInfo->currLT = LT;
	globalInfo->currGT = GT;

	TToken *token = (TToken *) malloc(sizeof(TToken));
	if (token == NULL) {
		return parserError(ERR_RUNTIME, "parse", 0);
	}
	TToken **tokenPP = &token;
	**tokenPP = getToken(globalInfo->file, globalInfo->currGT);
	if (errflg == ERR_LEXICAL) return;
	
	/*START OF PARSING*/
	result = start(tokenPP, globalInfo);
	/*END OF PARSING*/

	//TODO: check if all functions are defined

	free(globalInfo);
	free(token);
	return result;
}

/**
	represents START non-terminal
	
	assert *tokenPP != NULL
*/
int start(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	switch (type) {
		case TOK_KEY:	if (strcmp(keyW, "def") == 0) {	
							//rule #2 START -> FUN-DEF eol S
							//process FUN-DEF
							if (fundef(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
							//process eol
							if (eol(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
							//process S
							if (start(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
							return PROGRAM_OK;
						} else if (strcmp(keyW, "if") == 0 || strcmp(keyW, "while") == 0) {
							//continue to rule #1 
						} else
							break;	//error
		case TOK_EOL:
		case TOK_ID:	//rule #1 START -> STAT eol S
						//process STAT
						if (stat(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
						//process eol
						if (eol(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
						//process S
						if (start(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
						return PROGRAM_OK;
		case TOK_EOF:	//rule #3 START -> eof
						genSTOP();
						return PROGRAM_OK;
		default:		break;						
	}
	return parserError(ERR_SYNTAX, __func__, type);
}

/**
	represents STAT non-terminal
*/
int stat(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	switch (type) {
		case TOK_ID:	//rule #10 STAT -> id ASS-OR-FUN
						string savedIdOne = (*tokenPP)->data.s;	//save ID for later call of generator
						**tokenPP = getToken(globalInfo->file, globalInfo->currGT);	//get next token
						if (errflg != PROGRAM_OK) return errflg;	//check for lexical error from scanner
						//process ASS-OR-FUN
						if (assorfun(tokenPP, globalInfo, savedIdOne) != PROGRAM_OK) return errflg;
						return PROGRAM_OK;
		case TOK_KEY:	if (strcmp(keyW, "if") == 0) {
							//rule #11 STAT -> if expr then eol ST-LIST else eol ST-LIST end
							//**tokenPP = getToken(globalInfo->file, globalInfo->currGT); //if is present, call next token  DONT GET NEXT TOKEN
							value = tif(tokenPP) && processExpression(f, globalInfo->currGT, globalInfo->currLT) && then(tokenPP) && eol(tokenPP) && 
									stlist(tokenPP) && telse(tokenPP) && eol(tokenPP) && stlist(tokenPP) && end(tokenPP);
						} else if (strcmp(keyW, "while") == 0) {
							//rule #12 STAT -> while expr do eol ST-LIST end
							//**tokenPP = getToken(globalInfo->file, globalInfo->currGT); //while is present, call next token DONT GET NEXT TOKEN
							value = tWhile(tokenPP) && processExpression(f, globalInfo->currGT, globalInfo->currLT) && tdo(tokenPP) && 
									eol(tokenPP) && stlist(tokenPP) && end(tokenPP);
						}
						break;
		case TOK_EOL:	//rule #13 STAT -> eps
						return true;
		default:		return parserError(ERR_SYNTAX, __func__, type);							
	}
	return value;
}

/**
	represents FUN-DEF non-terminal
*/
bool fundef(TToken **tokenPP, TWrapper *globalInfo) {
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
		return parserError(ERR_SYNTAX, __func__, type);
	} 
	return value;
}

/**
	represents ST-LIST non-terminal
*/
bool stlist(TToken **tokenPP, TWrapper *globalInfo) {
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
		default:		return parserError(ERR_SYNTAX, __func__, type);					
	}
	return value;
}

/**
	represents P-LIST non-terminal
*/
int plist(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
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
						(globalInfo->paramCounter)++;
						//process term
						if (term(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
						**tokenPP = getToken(globalInfo->file, globalInfo->currGT); //term is present, call next token
						if (errflg != PROGRAM_OK) return errflg;	//check for lexical error in scanner
						if (plist(tokenPP) != PROGRAM_OK) return errflg;
						return PROGRAM_OK;
		case TOK_COMMA: //rule #8 P-LIST -> , TERM P-LIST
						//process comma
						**tokenPP = getToken(globalInfo->file, globalInfo->currGT); //comma is present, call next token
						if (errflg != PROGRAM_OK) return errflg;	//check for lexical error in scanner
						(globalInfo->paramCounter)++;
						//process term
						if (term(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
						**tokenPP = getToken(globalInfo->file, globalInfo->currGT); //term is present, call next token
						if (errflg != PROGRAM_OK) return errflg;	//check for lexical error in scanner
						//process plist
						if (plist(tokenPP) != PROGRAM_OK) return errflg;
						return PROGRAM_OK;
		case TOK_RBR:
		case TOK_EOL:	//rule #9 P-LIST -> eps
						return PROGRAM_OK;
		default:		break;
	}
	return parserError(ERR_SYNTAX, __func__, type);						
}

/**
 * represents TERM non-terminal	
 * checks if loaded token is correct and calls generator
*/
int term(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	switch (type) {
		case TOK_ID:	//rule #20 TERM -> id 
						Data idData;
						if (globalInfo->inFunCall) {	//passing parameter to function
							if (!symTabSearch(globalInfo->currLT, keyW, &idData)) {	//try to pass undefined variable to funcall
								return parserError(ERR_SEM_DEFINE, __func__, 0);
							} else {
								genFunCallPar(type, (*token)->data.s, globalInfo->inWhile);
							}
						} else { //define variable in new function's LT
							Data idData = createDataForNewVar(globalInfo->paramCounter);
							symTabInsert(&(globalInfo->currLT), keyW, idData);
						}
						return PROGRAM_OK;
		case TOK_STRING://rule #21 TERM -> const
						if (globalInfo->inFunCall) {
							genFunCallPar(type, (*token)->data.s, globalInfo->inWhile);
						} else {	//cannot define function with constant
							return parserError(ERR_SEM_OTHER, __func__, 0);
						}
						return PROGRAM_OK;
		case TOK_FLOAT: //rule #21 TERM -> const
						if (globalInfo->inFunCall) {
							genFunCallPar(type, (*token)->data.f, globalInfo->inWhile);
						} else {	//cannot define function with constant
							return parserError(ERR_SEM_OTHER, __func__, 0);
						}
						return PROGRAM_OK;
		case TOK_INT:	//rule #21 TERM -> const
						if (globalInfo->inFunCall) {
							genFunCallPar(type, (*token)->data.i, globalInfo->inWhile);
						} else {	//cannot define function with constant
							return parserError(ERR_SEM_OTHER, __func__, 0);
						}
						return PROGRAM_OK;
		case TOK_KEY:	if (strcmp(keyW, "nil") == 0) {
							//rule #22 TERM -> nil
							if (globalInfo->inFunCall) {
								genFunCallPar(type, (*token)->data.s, globalInfo->inWhile);
							} else {	//cannot define function with constant
								return parserError(ERR_SEM_OTHER, __func__, 0);
							}
							return PROGRAM_OK;
						}
		default:		break;
	}
	return parserError(ERR_SYNTAX, __func__, type);
}

/**
 * represents ASS-OR-FUN non-terminal
 * @param savedIdOne is the name of the recently red id 
 */
int assorfun(TToken **tokenPP, TWrapper *globalInfo, string savedIdOne) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	switch (type) {
		case TOK_ASSIGN: //rule #14 ASS-OR-FUN -> = ASSIGN
						**tokenPP = getToken(globalInfo->file, globalInfo->currGT);	//= is present, call next token
						if (errflg != PROGRAM_OK) return errflg;	//check for lexical error in scanner
						//process Lvalue Var ID
						TsymData *data = NULL;
						if (!symTabSearch(globalInfo->currLT, savedIdOne, data)) {	//ID not present in current LT
							if (!symTabSearch(globalInfo->currGT, savedIdOne, data)) {	//ID not defined as function
								genDEFVAR(savedIdOne);
							} else {
								return parserError(ERR_SEM_DEFINE, __func__, 0);
							}
						} //else Variable already defined
						//process assign
						if (assign(tokenPP, globalInfo, savedIdOne) != PROGRAM_OK) return errflg;
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
		default:		return parserError(ERR_SYNTAX, __func__, type);						
	}
	return value;
}

/**
 * represents ASSIGN non-terminal
 * @param savedIdOne is the name of the first ID in recently red sequence: id = 
 */
int assign(TToken **tokenPP, TWrapper *globalInfo, string savedIdOne) {
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
						//**tokenPP = getToken(globalInfo->file, globalInfo->currGT); //expr is present, call next token //DONT CALL NEXT TOKEN
						int E = processExpression(f, globalInfo->currGT, globalInfo->currLT);
						if (errflg != PROGRAM_OK) return errflg;	//check for errors in PSA
						(globalInfo->psaCounter)++;
						genASSIGN(to savedIdOne, E);
						return PROGRAM_OK;
		case TOK_ID:	string savedIdTwo = (*tokenPP)->data.s;
						if (decideExprOrFunc(tokenPP, savedIdOne, savedIdTwo) != PROGRAM_OK) return errflg;
						return PROGRAM_OK;
		default:		break;						
	}
	return parserError(ERR_SYNTAX, __func__, type);
}

/**
 * decides if there is an expression or function
 * 
 * eventually passes program to PSA
 * @param savedIdOne is the name of the first ID in recently red sequence: id = id
 * @param savedIdTwo is the name of the second ID in recently red sequence: id = id
 */
int decideExprOrFunc(TToken **tokenPP, TWrapper *globalInfo, string savedIdOne, string savedIdTwo) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	TToken bufferToken = **tokenPP;
	**tokenPP = getToken(globalInfo->file, globalInfo->currGT); //id is present, but I need another token to decide
	if (errflg != PROGRAM_OK) return errflg;	//check for lexical error in scanner
	switch ((*tokenPP)->type) {
		case TOK_EOL:	//rules #17 and #16 both work, it is a simple function id = fun or id = id expression;
						returnToken(**tokenPP); //must return the eol though, so stat can read it
						TsymData data;
						if (symTabSearch(globalInfo->currGT, savedIdTwo, &data)) {	//ID is function
							if (data.params == 0) {	
								genFunCallBegin(globalInfo->inWhile);
								genFunCallEnd(savedIdOne, globalInfo->inWhile);
								genASSIGN(to savedIdOne, from funcall);
							} else {	//0 is wrong amount of parameters
								return parserError(ERR_SEM_PARAM, __func__, 0);
							}
						} else if (symTabSearch(globalInfo->currLT, savedIdTwo, &data)) {	//ID is variable
							genASSIGN(to savedIdOne, from savedIdTwo);
						} else {	//function call before definition
							data = createDataForNewFunc(false, 0);
							symTabInsert(&(globalInfo->currGT), savedIdTwo, TsymData *data);
							genFunCallBegin(globalInfo->inWhile);
							genFunCallEnd(savedIdOne, globalInfo->inWhile);
							genASSIGN(to savedIdOne, from funcall);
						}
						return PROGRAM_OK;
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
						int E = processExpression(f, globalInfo->currGT, globalInfo->currLT);
						if (errflg != PROGRAM_OK) return errflg;	//check for errors in PSA
						(globalInfo->psaCounter)++;
						genASSIGN(to savedIdOne, E);
						return PROGRAM_OK;
		case TOK_KEY:	if (strcmp((*tokenPP)->data.s, "nil")==0) {
							//continue to rule #16
						} else 
							break;
		case TOK_LBR:
		case TOK_STRING:
		case TOK_FLOAT:
		case TOK_INT:
		case TOK_ID:	//rule #16 ASSIGN -> id P-BODY
						//actually no need of returning any tokens to scanner
						Data funcData;
						if (symTabSearch(globalInfo->currLT, savedIdTwo, &data)) {
							return parserError(ERR_SEM_DEFINE, __func__, 0);
						} else if (symTabSearch(globalInfo->currGT, savedIdTwo, &data)) {
							//function is defined
						} else {
							//new function
							data = createDataForNewFunc(false, 0);	//false means undefined
							symTabInsert(&(globalInfo->currGT), savedIdTwo, TsymData *data);
						}
						globalInfo->paramCounter = 0;
						genFunCallBegin(globalInfo->inWhile);
						globalInfo->inFunCall = true;
						//call plist, store params and count them
						if (plist(tokenPP, globalInfo) != PROGRAM_OK) return errflg; 
						//if amount of parameters is wrong, semantic error (only for defined function)
						if (data.defined) {	//function was defined before, amount of parameters must equal
							if (globalInfo->paramCounter != data.params) {
								return parserError(ERR_SEM_PARAM, __func__, 0);
							}
						} else if (data.called) {	//function was called before, amounts must match 
							if (data.params != globalInfo->paramCounter) {
								return parserError(ERR_SEM_PARAM, __func__, 0);
							}
						} else {	//first call of undefined function
							data.params = globalInfo->paramCounter;
							data.called = true;
						}
						//function call
						genFunCallEnd(savedIdTwo, globalInfo->inWhile);
						globalInfo->inFunCall = false;
						genASSIGN(to savedIdOne, from funcall);
						return PROGRAM_OK;
		default:		break;
	}
	return parserError(ERR_SYNTAX, __func__, (*tokenPP)->type);
}

/**
 * creates data structure for new function
 * includes creating inner LT
 */ 
TsymData createDataForNewFunc(bool defined, int params) {
	TsymItem *newLT = NULL;    // local symbol table
	symTabInit(&newLT);
	TsymData data;
	data.type = TYPE_FUN;
	data.defined = defined;
	data.called = false;
	data.params = params;
	data.LT = newLT;
	return data;
}

/**
 * creates data structure for new variable
 */ 
TsymData createDataForNewVar(unsigned order) {
	TsymData data;
	data.type = TYPE_VAR;
	data.order = order;
	return data;
}

/**
	represents pbody non-terminal
	pbody represents syntax for inserting parameters when calling a function like '(a, b, c)'
*/
bool pbody(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	switch (type) {
		case TOK_LBR:	//rule #18 P-BODY -> ( P-LIST )
						**tokenPP = getToken(globalInfo->file, globalInfo->currGT); //( is present, call next token
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
		default:		break;						
	}
	return parserError(ERR_SYNTAX, __func__, type);
}

/**
	eol terminal
*/
int eol(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	TTokenType type = (*tokenPP)->type;
	if (type == TOK_EOL) {
		**tokenPP = getToken(globalInfo->file, globalInfo->currGT); //eol is present, call next token
		if (errflg != PROGRAM_OK) return errflg;
		return PROGRAM_OK;
	} else {
		return parserError(ERR_SYNTAX, __func__, type);
	}
}

/**
	if terminal
*/
bool tif(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	//----SEMANTICS----
	
	//----SEMANTICS----
	return true;
}

/**
	then terminal
*/
bool then(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	if (type == TOK_KEY && (strcmp(keyW, "then") == 0)) {
		**tokenPP = getToken(globalInfo->file, globalInfo->currGT); //then is present, call next token
		value = true;
	} else {
		return parserError(ERR_SYNTAX, __func__, type);
	}
	return value;
}

/**
	else terminal
*/
bool telse(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	if (type == TOK_KEY && (strcmp(keyW, "else") == 0)) {
		**tokenPP = getToken(globalInfo->file, globalInfo->currGT); //else is present, call next token
		value = true;
	} else {
		return parserError(ERR_SYNTAX, __func__, type);
	}
	return value;
}

/**
 * while terminal
 */ 
bool tWhile(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	return true;
}

/**
 *	do terminal
 */
bool tdo(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	if (type == TOK_KEY && (strcmp(keyW, "do") == 0)) {
		**tokenPP = getToken(globalInfo->file, globalInfo->currGT); //do is present, call next token
		value = true;
	} else {
		return parserError(ERR_SYNTAX, __func__, type);
	}
	return value;
}

/**
 * def terminal
 */ 
bool def(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	**tokenPP = getToken(globalInfo->file, globalInfo->currGT); //def is present, call next token
	return true;
}

/**
	end terminal
*/
bool end(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	if (type == TOK_KEY && (strcmp(keyW, "end") == 0)) {
		**tokenPP = getToken(globalInfo->file, globalInfo->currGT); //end is present, call next token
		value = true;
	} else {
		return parserError(ERR_SYNTAX, __func__, type);
	}
	return value;
}


/**
	id terminal
*/
bool id(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	if (type == TOK_ID) {
		//----SEMANTICS----
			
		//----SEMANTICS----
		**tokenPP = getToken(globalInfo->file, globalInfo->currGT); //id is present, call next token
		value = true;
	} else {
		return parserError(ERR_SYNTAX, __func__, type);
	}
	return value;
}

/**
	left bracket terminal
*/
bool lbr(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	if (type == TOK_LBR) {
		**tokenPP = getToken(globalInfo->file, globalInfo->currGT); //lbr is present, call next token
		value = true;
	} else {
		return parserError(ERR_SYNTAX, __func__, type);
	}
	return value;
}

/**
	right bracket terminal
*/
bool rbr(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	if (type == TOK_RBR) {
		**tokenPP = getToken(globalInfo->file, globalInfo->currGT); //rbr is present, call next token
		value = true;
	} else {
		return parserError(ERR_SYNTAX, __func__, type);
	}
	return value;
}

/** end of parser.c */