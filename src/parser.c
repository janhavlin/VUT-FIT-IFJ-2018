/**
	file name:		parser.c
	project:		VUT-FIT-IFJ-2018
	created:		19.11.2018
	last modified:	01.12.2018
	
	created by: 	Jakub Karpíšek xkarpi06@stud.fit.vutbr.cz
	modifications:	
	
	description:	Recursive Descent parser				
*/

#include "parser.h"

//------------------WRAPPER STRUCTURE------------------------

/**
 * Instead of keeping global variables in parser.c
 * or passing million of arguments between functions
 * this wrapper structure carries all the data needed 
 * for semantic analysis.
 */ 
typedef struct {
	FILE *file;
	TsymItem *mainLT;
	TsymItem *currLT;
	TsymItem *GT;
	TInstrList *instructions;
	unsigned whileCounter;
	unsigned ifCounter;
	unsigned psaCounter;
	int paramCounter;	//set to 0 before every function call and function def
	bool inWhile;
	bool inFunCall;			//flag for parameters to determine if we read them for definition or call, false means inFunDef
} TWrapper;

//--------------FUNCTION PROTOTYPES----------------------

/* NON-TERMINALS */

int start(TToken **, TWrapper *);
int stat(TToken **, TWrapper *);
int fundef(TToken **, TWrapper *);
int stlist(TToken **, TWrapper *);
int plist(TToken **, TWrapper *);
int term(TToken **, TWrapper *);
int assorfun(TToken **, TWrapper *, string);
int assign(TToken **, TWrapper *, string);
int decideExprOrFunc(TToken **, TWrapper *, string, string);
int pbody(TToken **, TWrapper *);

/* TERMINALS */

int eol(TToken **, TWrapper *);
int then(TToken **, TWrapper *);
int telse(TToken **, TWrapper *);
int tdo(TToken **, TWrapper *);
int end(TToken **, TWrapper *);
int id(TToken **);
int lbr(TToken **, TWrapper *);
int rbr(TToken **, TWrapper *);

/* OTHER */

int parserError(int errNbr, const char *whichFunction, int tokenType);
void initTWrapper(TWrapper *);
TsymData createDataForNewFunc(bool defined, int params);
TsymData createDataForNewVar(unsigned order);

//--------------PARSE-------------------------------------

int parse(FILE *input, TsymItem *GT, TsymItem *LT, TInstrList *instructions) {
	if (DEBUG) printf("Function: %s\n", __func__);
	int result = PROGRAM_OK;
	TWrapper *globalInfo = (TWrapper *) malloc(sizeof(TWrapper));
	if (globalInfo == NULL) {
		return parserError(ERR_RUNTIME, "parse", 0);
	}
	initTWrapper(globalInfo);
	globalInfo->file = input;
	globalInfo->mainLT = LT;
	globalInfo->currLT = LT;
	globalInfo->GT = GT;
	globalInfo->instructions = instructions;

	TToken *token = (TToken *) malloc(sizeof(TToken));
	if (token == NULL) {
		return parserError(ERR_RUNTIME, "parse", 0);
	}
	TToken **tokenPP = &token;
	**tokenPP = getToken(globalInfo->file, globalInfo->GT);
	if (errflg == ERR_LEXICAL) return errflg;
	
	/*START OF PARSING*/
	result = start(tokenPP, globalInfo);
	/*END OF PARSING*/

	//check if all functions were defined
	if (!symTabCheckIfAllFunctionsWereDefined(globalInfo->GT)) {
		errflg = ERR_SEM_OTHER;
		ifjErrorPrint("ERROR %d SEMANTIC: calling undefined function in program!\n", errflg); 
		result = errflg;
	}

	free(globalInfo);
	free(token);
	return result;
}

//--------------RECURSIVE DESCENT - NON-TERMINALS--------------

/**
 * represents S non-terminal
 * 
 * assert *tokenPP != NULL
 */
int start(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("Function:%s Token:%d\n", __func__, (*tokenPP)->type);
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
						//genSTOP();
						printf("genSTOP()\n");
						genPrgEnd(globalInfo->instructions);
						return PROGRAM_OK;
		default:		break;						
	}
	return parserError(ERR_SYNTAX, __func__, type);
}

/**
 * represents STAT non-terminal
 */
int stat(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("Function:%s Token:%d\n", __func__, (*tokenPP)->type);
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	string savedIdOne;
	switch (type) {
		case TOK_ID:	//rule #10 STAT -> id ASS-OR-FUN
						savedIdOne = (*tokenPP)->data.s;	//save ID for later call of generator
						**tokenPP = getToken(globalInfo->file, globalInfo->GT);	//get next token
						if (errflg != PROGRAM_OK) return errflg;	//check for lexical error from scanner
						//process ASS-OR-FUN
						if (assorfun(tokenPP, globalInfo, savedIdOne) != PROGRAM_OK) return errflg;
						return PROGRAM_OK;
		case TOK_KEY:	if (strcmp(keyW, "if") == 0) {
							//rule #11 STAT -> if expr then eol ST-LIST else eol ST-LIST end
							//process if
							//don't call getToken, leave it for PSA
							//information about if:
							unsigned ifNumber = (globalInfo->ifCounter)++;
							//process expr
							unsigned E = processExpression(globalInfo->file, "then", globalInfo->GT, globalInfo->currLT, globalInfo->instructions, globalInfo->inWhile);
							if (errflg != PROGRAM_OK) return errflg;	//check for errors in PSA
							genIfCond(&globalInfo->instructions, ifNumber, globalInfo->psaCounter, E, globalInfo->inWhile);
							printf("genIfCond(%d,%u)\n", ifNumber, E);
							(globalInfo->psaCounter)++;
							//process then
							//must load token after calling PSA
							**tokenPP = getToken(globalInfo->file, globalInfo->GT);
							if (errflg != PROGRAM_OK) return errflg;	//check for lexical error from scanner
							if (then(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
							//process eol
							if (eol(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
							//process ST-LIST
							if (stlist(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
							//process else
							if (telse(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
							genIfElse(&globalInfo->instructions, ifNumber, globalInfo->inWhile);
							printf("genIfElse(%d)\n", ifNumber);
							//process eol
							if (eol(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
							//process ST-LIST
							if (stlist(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
							//process end
							if (end(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
							genIfEnd(&globalInfo->instructions, ifNumber, globalInfo->inWhile);
							printf("genIfEnd(%d)\n", ifNumber);
							return PROGRAM_OK;
						} else if (strcmp(keyW, "while") == 0) {
							//rule #12 STAT -> while expr do eol ST-LIST end
							//process while
							//don't call getToken, leave it for PSA
							//information about while:
							unsigned whileNumber = (globalInfo->whileCounter)++;
							bool topLevelWhile = !globalInfo->inWhile;
							globalInfo->inWhile = true;
							genWhileBegin(globalInfo->instructions, whileNumber, globalInfo->inWhile);
							printf("genWhileBegin(%d)\n", whileNumber);
							//process expr
							unsigned E = processExpression(globalInfo->file, "do", globalInfo->GT, globalInfo->currLT, globalInfo->instructions, globalInfo->inWhile);
							if (errflg != PROGRAM_OK) return errflg;	//check for errors in PSA
																						// TODO: Figure out why is E-1 needed!!
							genWhileCond(globalInfo->instructions, whileNumber, globalInfo->psaCounter, E-1, globalInfo->inWhile);
							printf("genWhileCond(%u)\n", E);
							(globalInfo->psaCounter)++;
							//process do
							//must load token after calling PSA
							**tokenPP = getToken(globalInfo->file, globalInfo->GT);
							if (errflg != PROGRAM_OK) return errflg;	//check for lexical error from scanner
							if (tdo(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
							//process eol
							if (eol(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
							//process ST-LIST
							if (stlist(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
							//process end
							if (end(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
							
							genWhileEnd(globalInfo->instructions, whileNumber, globalInfo->inWhile);
							printf("genWhileEnd(%d)\n", whileNumber);
							//inWhile = false, but only if this while is not nested in another while
							globalInfo->inWhile = !topLevelWhile;
							return PROGRAM_OK;
						}
						break;
		case TOK_EOL:	//rule #13 STAT -> eps
						return PROGRAM_OK;
		default:		break;
	}
	return parserError(ERR_SYNTAX, __func__, type);
}

/**
 * represents FUN-DEF non-terminal
 */
int fundef(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("Function:%s Token:%d\n", __func__, (*tokenPP)->type);
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	if (type == TOK_KEY) {
		if (strcmp(keyW, "def") == 0) {
			//rule #6 FUN-DEF -> def id ( P-LIST ) eol ST-LIST end
			TsymData funcData;
			TsymData *newFuncData = &funcData;
			//process def
			**tokenPP = getToken(globalInfo->file, globalInfo->GT); //def is present, call next token
			if (errflg != PROGRAM_OK) return errflg;	//check for lexical error in scanner
			//process id
			if (id(tokenPP) != PROGRAM_OK) return errflg;
			string functionId = (*tokenPP)->data.s;		//save name of ID
			if (symTabSearch(globalInfo->currLT, functionId, newFuncData)) {	//id taken by global variable
				return parserError(ERR_SEM_DEFINE, __func__, 0);
			} else if (symTabSearch(globalInfo->GT, functionId, newFuncData)) {	//func already is in GT
				if (!newFuncData->defined) {	
					//func was not defined, just called, it is OK
				} else { //redefinition
					return parserError(ERR_SEM_DEFINE, __func__, 0);
				}
			} else {	//new uncalled function definition
				funcData = createDataForNewFunc(true, 0);	//false means undefined
				newFuncData = symTabInsert(&(globalInfo->GT), functionId, funcData);
				if (newFuncData == NULL) return errflg;
			}
			//genFunDefStart(functionId);
			printf("genFunDefStart(%s)\n", functionId);
			**tokenPP = getToken(globalInfo->file, globalInfo->GT); //id is present, call next token
			if (errflg != PROGRAM_OK) return errflg;
			//process (
			if (lbr(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
			//process P-LIST
			globalInfo->paramCounter = 0;
			globalInfo->inFunCall = false;	//in function definition here
			globalInfo->currLT = newFuncData->LT;
			if (plist(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
			if (!newFuncData->defined) {	//fun was called before, it must be defined with same amount of parameters
				if (newFuncData->params != globalInfo->paramCounter) {
					return parserError(ERR_SEM_PARAM, __func__, 0);
				} else {
					newFuncData->defined = true;
				}
			}
			newFuncData->params = globalInfo->paramCounter;
			//process )
			if (rbr(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
			//process eol
			if (eol(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
			//process ST-LIST
			if (stlist(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
			//process end
			if (end(tokenPP, globalInfo) != PROGRAM_OK) return errflg;

			//genFunDefEnd(functionId);
			printf("genFunDefEnd(%s)\n", functionId);
			globalInfo->currLT = globalInfo->mainLT;
			return PROGRAM_OK;
		} 
	} else {
		//syntax error
	}
	return parserError(ERR_SYNTAX, __func__, type);
}

/**
 * represents ST-LIST non-terminal
 */ 
int stlist(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("Function:%s Token:%d\n", __func__, (*tokenPP)->type);
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	switch (type) {
		case TOK_KEY:	if (strcmp(keyW, "else") == 0 || strcmp(keyW, "end") == 0) {
							//rule #5 ST-LIST -> eps
							return PROGRAM_OK;
						} else if (strcmp(keyW, "if") == 0 || strcmp(keyW, "while") == 0) {
							//continue to rule #4
						} else 
							break;
		case TOK_ID:
		case TOK_EOL:	//rule #4 ST-LIST -> STAT eol ST-LIST
						//process stat
						if (stat(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
						//process eol
						if (eol(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
						//process stlist
						if (stlist(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
						return PROGRAM_OK;
		default:		break;
	}
	return parserError(ERR_SYNTAX, __func__, type);
}

/**
 * represents P-LIST non-terminal
 */
int plist(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("Function:%s Token:%d\n", __func__, (*tokenPP)->type);
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
						**tokenPP = getToken(globalInfo->file, globalInfo->GT); //term is present, call next token
						if (errflg != PROGRAM_OK) return errflg;	//check for lexical error in scanner
						if (plist(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
						return PROGRAM_OK;
		case TOK_COMMA: //rule #8 P-LIST -> , TERM P-LIST
						//process comma
						**tokenPP = getToken(globalInfo->file, globalInfo->GT); //comma is present, call next token
						if (errflg != PROGRAM_OK) return errflg;	//check for lexical error in scanner
						(globalInfo->paramCounter)++;
						//process term
						if (term(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
						**tokenPP = getToken(globalInfo->file, globalInfo->GT); //term is present, call next token
						if (errflg != PROGRAM_OK) return errflg;	//check for lexical error in scanner
						//process plist
						if (plist(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
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
	if (DEBUG) printf("Function:%s Token:%d\n", __func__, (*tokenPP)->type);
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	TsymData idData;
	switch (type) {
		case TOK_ID:	//rule #20 TERM -> id 
						if (globalInfo->inFunCall) {	//passing parameter to function
							if (!symTabSearch(globalInfo->currLT, keyW, &idData)) {	//try to pass undefined variable to funcall
								return parserError(ERR_SEM_DEFINE, __func__, 0);
							} else {
								//genFunCallPar(type, (*tokenPP)->data.s, globalInfo->inWhile);
								printf("genFunCallPar(%d, %s, %d)\n", type, (*tokenPP)->data.s, globalInfo->inWhile);
							}
						} else { //define variable in new function's LT
							if (symTabSearch(globalInfo->GT, keyW, &idData)) {
								//keyword is already used for function
								return parserError(ERR_SEM_DEFINE, __func__, 0);
							} else {
								idData = createDataForNewVar(globalInfo->paramCounter);
								symTabInsert(&(globalInfo->currLT), keyW, idData);
							}
						}
						return PROGRAM_OK;
		case TOK_STRING: //rule #21 TERM -> const
						if (globalInfo->inFunCall) {
							//genFunCallPar(type, (*tokenPP)->data.s, globalInfo->inWhile);
							printf("genFunCallPar(%d, %s, %d)\n", type, (*tokenPP)->data.s, globalInfo->inWhile);
						} else {	//cannot define function with constant
							return parserError(ERR_SEM_OTHER, __func__, 0);
						}
						return PROGRAM_OK;
		case TOK_FLOAT: //rule #21 TERM -> const
						if (globalInfo->inFunCall) {
							//genFunCallPar(type, (*tokenPP)->data.f, globalInfo->inWhile);
							printf("genFunCallPar(%d, %f, %d)\n", type, (*tokenPP)->data.f, globalInfo->inWhile);
						} else {	//cannot define function with constant
							return parserError(ERR_SEM_OTHER, __func__, 0);
						}
						return PROGRAM_OK;
		case TOK_INT:	//rule #21 TERM -> const
						if (globalInfo->inFunCall) {
							//genFunCallPar(type, (*tokenPP)->data.i, globalInfo->inWhile);
							printf("genFunCallPar(%d, %i, %d)\n", type, (*tokenPP)->data.i, globalInfo->inWhile);
						} else {	//cannot define function with constant
							return parserError(ERR_SEM_OTHER, __func__, 0);
						}
						return PROGRAM_OK;
		case TOK_KEY:	if (strcmp(keyW, "nil") == 0) {
							//rule #22 TERM -> nil
							if (globalInfo->inFunCall) {
								//genFunCallPar(type, (*tokenPP)->data.s, globalInfo->inWhile);
								printf("genFunCallPar(%d, %s, %d)\n", type, (*tokenPP)->data.s, globalInfo->inWhile);
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
	if (DEBUG) printf("Function:%s Token:%d\n", __func__, (*tokenPP)->type);
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	TsymData varData;
	TsymData newFuncData;
	TsymData *funcData = &newFuncData;
	switch (type) {
		case TOK_ASSIGN: //rule #14 ASS-OR-FUN -> = ASSIGN
						**tokenPP = getToken(globalInfo->file, globalInfo->GT);	//= is present, call next token
						if (errflg != PROGRAM_OK) return errflg;	//check for lexical error in scanner
						//process Lvalue Var ID
						if (!symTabSearch(globalInfo->currLT, savedIdOne, &varData)) {	//ID not present in current LT
							if (!symTabSearch(globalInfo->GT, savedIdOne, &varData)) {	//ID not defined as function
								genDefVar(globalInfo->instructions, savedIdOne, globalInfo->inWhile);
								printf("genDefVar(%s)\n", savedIdOne);
								varData = createDataForNewVar(0);	//0 means not parameter, just variable
								symTabInsert(&(globalInfo->currLT), savedIdOne, varData);
							} else {
								return parserError(ERR_SEM_DEFINE, __func__, 0);
							}
						} //else Variable already defined, which is OK
						//process assign
						if (assign(tokenPP, globalInfo, savedIdOne) != PROGRAM_OK) return errflg;
						return PROGRAM_OK;
		case TOK_KEY:	if (strcmp(keyW, "nil") == 0) {
							//continue to rule #15
						} else 
							break;
		case TOK_COMMA:	//very suspicious to start P-BODY with comma, but it is in LL table
		case TOK_LBR:
		case TOK_STRING:
		case TOK_FLOAT:
		case TOK_INT:
		case TOK_ID:
		case TOK_EOL:	//rule #15 ASS-OR-FUN -> P-BODY
						if (symTabSearch(globalInfo->currLT, savedIdOne, funcData)) {	//id name is already taken by local variable
							return parserError(ERR_SEM_DEFINE, __func__, 0);
						} else if (symTabSearch(globalInfo->mainLT, savedIdOne, funcData)) {	//id name is taken by global variable
							return parserError(ERR_SEM_DEFINE, __func__, 0);
						} else if (symTabSearch(globalInfo->GT, savedIdOne, funcData)) {
							//function is defined
						} else {
							//new function
							newFuncData = createDataForNewFunc(false, 0);	//false means undefined
							funcData = symTabInsert(&(globalInfo->GT), savedIdOne, newFuncData);
							if (funcData == NULL) return errflg;
						}
						globalInfo->paramCounter = 0;
						//genFunCallBegin(globalInfo->inWhile);
						printf("genFunCallBegin(%d)\n", globalInfo->inWhile);
						globalInfo->inFunCall = true;
						//call pbody, store params and count them
						if (pbody(tokenPP, globalInfo) != PROGRAM_OK) return errflg; 
						//if amount of parameters is wrong, semantic error (only for defined function)
						if (funcData->defined) {	//function was defined before, amount of parameters must equal
							if (funcData->params == UNLIMITED_P && globalInfo->paramCounter != 0) {
								//it is ok, function has variable amount of parameters
							} else if (globalInfo->paramCounter != funcData->params) {
								return parserError(ERR_SEM_PARAM, __func__, 0);
							} 
						} else if (funcData->called) {	//function was called before, amounts must match 
							if (funcData->params != globalInfo->paramCounter) {
								return parserError(ERR_SEM_PARAM, __func__, 0);
							}
						} else {	//first call of undefined function, store amount of parameters counted
							funcData->params = globalInfo->paramCounter;
							funcData->called = true;
						}
						//function call
						//genFunCallEnd(savedIdOne, globalInfo->inWhile);
						printf("genFunCallEnd(%s, %d)\n", savedIdOne, globalInfo->inWhile);
						globalInfo->inFunCall = false;
						return PROGRAM_OK;
		default:		break;
	}
	return parserError(ERR_SYNTAX, __func__, type);
}

/**
 * represents ASSIGN non-terminal
 * @param savedIdOne is the name of the first ID in recently red sequence: id = 
 */
int assign(TToken **tokenPP, TWrapper *globalInfo, string savedIdOne) {
	if (DEBUG) printf("Function:%s Token:%d\n", __func__, (*tokenPP)->type);
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	string savedIdTwo;
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
						//**tokenPP = getToken(globalInfo->file, globalInfo->GT); //expr is present, call next token //DONT CALL NEXT TOKEN
						unsigned E = processExpression(globalInfo->file, "eol", globalInfo->GT, globalInfo->currLT, globalInfo->instructions, globalInfo->inWhile);
						if (errflg != PROGRAM_OK) return errflg;	//check for errors in PSA
						genAssign(globalInfo->instructions, savedIdOne, globalInfo->psaCounter, E, globalInfo->inWhile);	//assign to savedIdOne
						printf("genAssign(%s, %s)\n", savedIdOne, "E");
						(globalInfo->psaCounter)++;
						//must load token after calling PSA
						**tokenPP = getToken(globalInfo->file, globalInfo->GT);
						if (errflg != PROGRAM_OK) return errflg;	//check for lexical error from scanner
						return PROGRAM_OK;
		case TOK_ID:	savedIdTwo = (*tokenPP)->data.s;
						if (decideExprOrFunc(tokenPP, globalInfo, savedIdOne, savedIdTwo) != PROGRAM_OK) return errflg;
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
	if (DEBUG) printf("Function:%s Token:%d\n", __func__, (*tokenPP)->type);
	TToken bufferToken = **tokenPP;
	**tokenPP = getToken(globalInfo->file, globalInfo->GT); //id is present, but I need another token to decide
	if (errflg != PROGRAM_OK) return errflg;	//check for lexical error in scanner
	TsymData newFuncData;
	TsymData *funcData = &newFuncData;
	switch ((*tokenPP)->type) {
		case TOK_EOL:	//rules #17 and #16 both work, it is a simple function id = fun or id = id expression;
						returnToken(**tokenPP); //must return the eol though, so stat can read it
						if (symTabSearch(globalInfo->GT, savedIdTwo, &newFuncData)) {	//ID is function
							if (newFuncData.params == 0) {	
								//genFunCallBegin(globalInfo->inWhile);
								printf("genFunCallBegin(%d)\n", globalInfo->inWhile);
								//genFunCallEnd(savedIdTwo, globalInfo->inWhile);
								printf("genFunCallEnd(%s, %d)\n", savedIdTwo, globalInfo->inWhile);
								//genASSIGN(savedIdOne, funcall);	//to savedIdOne, from funcall
								printf("genAssign(%s, %s)\n", savedIdOne, "funcall");
							} else {	//0 is wrong amount of parameters
								return parserError(ERR_SEM_PARAM, __func__, 0);
							}
						} else if (symTabSearch(globalInfo->currLT, savedIdTwo, &newFuncData)) {	//ID is variable
							//genASSIGN(savedIdOne, savedIdTwo);	//to savedIdOne, from savedIdTwo
							printf("genAssign(%s, %s)\n", savedIdOne, savedIdTwo);
						} else {	//function call before definition
							newFuncData = createDataForNewFunc(false, 0);
							symTabInsert(&(globalInfo->GT), savedIdTwo, newFuncData);
							//genFunCallBegin(globalInfo->inWhile);
							printf("genFunCallBegin(%d)\n", globalInfo->inWhile);
							//genFunCallEnd(savedIdTwo, globalInfo->inWhile);
							printf("genFunCallEnd(%s, %d)\n", savedIdTwo, globalInfo->inWhile);
							//genASSIGN(savedIdOne, funcall);	//to savedIdOne, from funcall
							printf("genAssign(%s, %s)\n", savedIdOne, "funcall");
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
						returnToken(**tokenPP);
						returnToken(bufferToken);
						unsigned E = processExpression(globalInfo->file, "eol", globalInfo->GT, globalInfo->currLT, globalInfo->instructions, globalInfo->inWhile);
						if (errflg != PROGRAM_OK) return errflg;	//check for errors in PSA
						genAssign(globalInfo->instructions, savedIdOne, globalInfo->psaCounter, E, globalInfo->inWhile);	//to savedIdOne
						printf("genAssign(%s, %s)\n", savedIdOne, "E");
						(globalInfo->psaCounter)++;
						//must load token after calling PSA
						**tokenPP = getToken(globalInfo->file, globalInfo->GT);
						if (errflg != PROGRAM_OK) return errflg;	//check for lexical error from scanner
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
						if (symTabSearch(globalInfo->currLT, savedIdTwo, funcData)) { 
							//id name is already taken by local variable
							return parserError(ERR_SEM_DEFINE, __func__, 0);
						} else if (symTabSearch(globalInfo->mainLT, savedIdTwo, funcData)) { 
							//id name is already taken by global variable
							return parserError(ERR_SEM_DEFINE, __func__, 0);
						} else if (symTabSearch(globalInfo->GT, savedIdTwo, funcData)) {
							//function is defined
						} else {
							//new function
							newFuncData = createDataForNewFunc(false, 0);	//false means undefined
							funcData = symTabInsert(&(globalInfo->GT), savedIdTwo, newFuncData);
							if (funcData == NULL) return errflg;
						}
						globalInfo->paramCounter = 0;
						//genFunCallBegin(globalInfo->inWhile);
						printf("genFunCallBegin(%d)\n", globalInfo->inWhile);
						globalInfo->inFunCall = true;
						//call pbody, store params and count them
						if (pbody(tokenPP, globalInfo) != PROGRAM_OK) return errflg; 
						//if amount of parameters is wrong, semantic error (only for defined function)
						if (funcData->defined) {	//function was defined before, amount of parameters must equal
							if (funcData->params == UNLIMITED_P && globalInfo->paramCounter != 0) {
								//it is ok, function has variable amount of parameters
							} else if (globalInfo->paramCounter != funcData->params) {
								return parserError(ERR_SEM_PARAM, __func__, 0);
							} 
						} else if (funcData->called) {	//function was called before, amounts must match 
							if (funcData->params != globalInfo->paramCounter) {
								return parserError(ERR_SEM_PARAM, __func__, 0);
							}
						} else {	//first call of undefined function
							funcData->params = globalInfo->paramCounter;
							funcData->called = true;
						}
						//function call
						//genFunCallEnd(savedIdTwo, globalInfo->inWhile);
						printf("genFunCallEnd(%s, %d)\n", savedIdTwo, globalInfo->inWhile);
						globalInfo->inFunCall = false;
						//genASSIGN(savedIdOne, funcall);	//to savedIdOne, from funcall
						printf("genAssign(%s, %s)\n", savedIdOne, "funcall");
						return PROGRAM_OK;
		default:		break;
	}
	return parserError(ERR_SYNTAX, __func__, (*tokenPP)->type);
}

/**
 * represents pbody non-terminal
 * pbody represents syntax for inserting parameters when calling a function like '(a, b, c)'
 */
int pbody(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("Function:%s Token:%d\n", __func__, (*tokenPP)->type);
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	switch (type) {
		case TOK_LBR:	//rule #18 P-BODY -> ( P-LIST )
						//process (
						**tokenPP = getToken(globalInfo->file, globalInfo->GT); //( is present, call next token
						if (errflg != PROGRAM_OK) return errflg;
						//process P-LIST
						if (plist(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
						//process )
						if (rbr(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
						return PROGRAM_OK;
		case TOK_KEY:	if(strcmp(keyW, "nil") == 0) {
							//continue to rule#19
						} else 
							break;
		case TOK_COMMA:	//again very suspicious to start P-BODY with comma
		case TOK_STRING:
		case TOK_FLOAT:
		case TOK_INT:
		case TOK_ID:
		case TOK_EOL:	//rule #19 P-BODY -> P-LIST
						//process P-LIST
						if (plist(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
						return PROGRAM_OK;
		default:		break;						
	}
	return parserError(ERR_SYNTAX, __func__, type);
}

//--------------RECURSIVE DESCENT - TERMINALS--------------

/**
 * eol terminal
 */
int eol(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("Function:%s Token:%d\n", __func__, (*tokenPP)->type);
	TTokenType type = (*tokenPP)->type;
	if (type == TOK_EOL) {
		**tokenPP = getToken(globalInfo->file, globalInfo->GT); //eol is present, call next token
		if (errflg != PROGRAM_OK) return errflg;
		return PROGRAM_OK;
	} else {
		return parserError(ERR_SYNTAX, __func__, type);
	}
}

/**
 * then terminal
 */
int then(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("Function:%s Token:%d\n", __func__, (*tokenPP)->type);
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	if (type == TOK_KEY && (strcmp(keyW, "then") == 0)) {
		**tokenPP = getToken(globalInfo->file, globalInfo->GT); //then is present, call next token
		if (errflg != PROGRAM_OK) return errflg;	//check for lexical error in scanner
		return PROGRAM_OK;
	} else {
		return parserError(ERR_SYNTAX, __func__, type);
	}
}

/**
 * else terminal
 */
int telse(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("Function:%s Token:%d\n", __func__, (*tokenPP)->type);
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	if (type == TOK_KEY && (strcmp(keyW, "else") == 0)) {
		**tokenPP = getToken(globalInfo->file, globalInfo->GT); //else is present, call next token
		if (errflg != PROGRAM_OK) return errflg;
		return PROGRAM_OK;
	} else {
		return parserError(ERR_SYNTAX, __func__, type);
	}
}

/**
 *	do terminal
 */
int tdo(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("Function:%s Token:%d\n", __func__, (*tokenPP)->type);
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	if (type == TOK_KEY && (strcmp(keyW, "do") == 0)) {
		**tokenPP = getToken(globalInfo->file, globalInfo->GT); //do is present, call next token
		if (errflg != PROGRAM_OK) return errflg;	//check for lexical error in scanner
		return PROGRAM_OK;
	} else {
		return parserError(ERR_SYNTAX, __func__, type);
	}
}

/**
 * end terminal
 */
int end(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("Function:%s Token:%d\n", __func__, (*tokenPP)->type);
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	if (type == TOK_KEY && (strcmp(keyW, "end") == 0)) {
		**tokenPP = getToken(globalInfo->file, globalInfo->GT); //end is present, call next token
		if (errflg != PROGRAM_OK) return errflg;
		return PROGRAM_OK;
	} else {
		return parserError(ERR_SYNTAX, __func__, type);
	}
}


/**
 * id terminal
 */
int id(TToken **tokenPP) {
	if (DEBUG) printf("Function:%s Token:%d\n", __func__, (*tokenPP)->type);
	TTokenType type = (*tokenPP)->type;
	if (type == TOK_ID) {
		//no token read here, need to save id name first and then call next token
		return PROGRAM_OK;
	} else {
		return parserError(ERR_SYNTAX, __func__, type);
	}
}

/**
 * left bracket terminal
 */
int lbr(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("Function:%s Token:%d\n", __func__, (*tokenPP)->type);
	TTokenType type = (*tokenPP)->type;
	if (type == TOK_LBR) {
		**tokenPP = getToken(globalInfo->file, globalInfo->GT); //lbr is present, call next token
		if (errflg != PROGRAM_OK) return errflg;
		return PROGRAM_OK;
	} else {
		return parserError(ERR_SYNTAX, __func__, type);
	}
}

/**
 * right bracket terminal
 */
int rbr(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("Function:%s Token:%d\n", __func__, (*tokenPP)->type);
	TTokenType type = (*tokenPP)->type;
	if (type == TOK_RBR) {
		**tokenPP = getToken(globalInfo->file, globalInfo->GT); //rbr is present, call next token
		if (errflg != PROGRAM_OK) return errflg;
		return PROGRAM_OK;
	} else {
		return parserError(ERR_SYNTAX, __func__, type);
	}
}

//--------------OTHER FUNCTIONS----------------------

/**
 * takes care about errors called in parser.c
 * @param errNbr is error number
 * @param where is name of parser.c function where the error occured
 * @tokenType is number of token that came (only used for syntax error)
 */ 
int parserError(int errNbr, const char *where, int tokenType) {
	errflg = errNbr;
	switch(errNbr) {
		case ERR_SEM_DEFINE:	ifjErrorPrint("SEMANTIC ERROR %d in parser.c in func. %s: Undefined variable or redefinition!\n", errflg, where); 
								break;
		case ERR_SEM_PARAM:		ifjErrorPrint("SEMANTIC ERROR %d in parser.c in func. %s: wrong amount of parameters!\n", errflg, where);
								break;
		case ERR_SEM_OTHER:		ifjErrorPrint("SEMANTIC ERROR %d in parser.c in func. %s: can't define function with constant parameter!\n", errflg, where); 
								break;
		case ERR_SYNTAX:		if (tokenType != TOK_KEY)
									ifjErrorPrint("SYNTAX ERROR %d in parser.c in func. %s: unexpected token type n.%d!\n", errflg, where, tokenType);
								else
									ifjErrorPrint("SYNTAX ERROR %d in parser.c in func. %s: unexpected token type n.%d or unexpected keyword!\n", errflg, where, tokenType);
								break;
		case ERR_RUNTIME: 		ifjErrorPrint("RUNTIME ERROR %d in parser.c in func. %s: allocation failed!\n", errflg, where);
		default: break;
	}
	return errflg;
}
 
void initTWrapper(TWrapper *info) {
	info->file = NULL;
	info->mainLT = NULL;
	info->currLT = NULL;
	info->GT = NULL;
	info->instructions = NULL;
	info->whileCounter = 0;
	info->ifCounter = 0;
	info->psaCounter = 0;
	info->paramCounter = 0;
	info->inWhile = false;
	info->inFunCall = false;
}

/**
 * creates data structure for new function
 * includes creating inner LT
 */ 
TsymData createDataForNewFunc(bool defined, int params) {
	TsymData data;
	data.type = TYPE_FUN;
	//data.order = 0;				//never used
	data.defined = defined;
	data.called = false;
	data.params = params;
	data.LT = NULL;
	return data;
}

/**
 * creates data structure for new variable
 */ 
TsymData createDataForNewVar(unsigned order) {
	TsymData data;
	data.type = TYPE_VAR;
	data.order = order;		
	/* data.defined = false;	//never used
	data.called = false;	//never used
	data.params = 0;		//never used
	data.LT = NULL;			//never used */
	return data;
}


/** end of parser.c */