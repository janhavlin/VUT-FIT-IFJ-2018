/**
	file name:		parser.c
	project:		VUT-FIT-IFJ-2018
	created:		19.11.2018
	last modified:	05.12.2018
	
	created by: 	Jakub Karpíšek xkarpi06@stud.fit.vutbr.cz
	modifications:	Petr Bobčík    xbobci02@stud.fit.vutbr.cz
                    Jan Havlín     xhavli47@stud.fit.vutbr.cz
	
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
	TsymItem **mainLT;
	TsymItem **currLT;
	TsymItem **GT;
	TInstrList *instructions;
	unsigned whileCounter;
	unsigned ifCounter;
	unsigned psaCounter;
	int paramCounter;	//set to 0 before every function call and function def
	bool inWhile;
	bool inFunCall;			//flag for parameters to determine if we read them for definition or call, false means inFunDef
	bool inFunDef;
} TWrapper;

/**
 * error kinds for better explained error message
 */ 
typedef enum {
	KIND_UNDEF_VAR,
	KIND_UNDEF_FUN,
	KIND_REDEF,
	KIND_UNEXP_KWD,
	KIND_OTHER
} ERR_KIND;

//--------------FUNCTION PROTOTYPES----------------------

/* NON-TERMINALS */

int start(TToken **, TWrapper *);
int stat(TToken **, TWrapper *);
int fundef(TToken **, TWrapper *);
int stlist(TToken **, TWrapper *);
int plist(TToken **, TWrapper *, string);
int term(TToken **, TWrapper *, string);
int assorfun(TToken **, TWrapper *, string);
int assign(TToken **, TWrapper *, string, TsymData *);
int decideExprOrFunc(TToken **, TWrapper *, string, string, TsymData *);
int pbody(TToken **, TWrapper *, string);

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

int parserError(int errNbr, const char *whichFunction, int tokenType, int errKind, string data);
void initTWrapper(TWrapper *);
TsymData createDataForNewFunc(bool defined, int params);
TsymData createDataForNewVar(unsigned order);

//--------------PARSE-------------------------------------

int parse(FILE *input, TsymItem **GT, TsymItem **LT, TInstrList *instructions) {
	if (DEBUG) printf("Function: %s\n", __func__);
	int result = PROGRAM_OK;

	TWrapper *globalInfo = (TWrapper *) malloc(sizeof(TWrapper));
	if (globalInfo == NULL) {
		return parserError(ERR_RUNTIME, __func__, 0, KIND_OTHER, "");
	}
	TToken *token = (TToken *) malloc(sizeof(TToken));
	if (token == NULL) {
		free(globalInfo);
		return parserError(ERR_RUNTIME, "parse", 0, KIND_OTHER, "");
	}
	initTWrapper(globalInfo);
	globalInfo->file = input;
	globalInfo->mainLT = LT;
	globalInfo->currLT = LT;
	globalInfo->GT = GT;
	globalInfo->instructions = instructions;


	TToken **tokenPP = &token;
	**tokenPP = getToken(globalInfo->file, *(globalInfo->GT));
	if (errflg != PROGRAM_OK) {
		free(globalInfo);
		free(token);
		return errflg;
	}
	
	/*START OF PARSING*/
	result = start(tokenPP, globalInfo);
	/*END OF PARSING*/

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
						return PROGRAM_OK;
		default:		break;						
	}
	return parserError(ERR_SYNTAX, __func__, type, KIND_OTHER, "");
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
						**tokenPP = getToken(globalInfo->file, *(globalInfo->GT));	//get next token
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
							bool topLevelIf = !globalInfo->inWhile;
							globalInfo->inWhile = true;
							//process expr
							unsigned E = processExpression(globalInfo->file, "then", *(globalInfo->GT), *(globalInfo->currLT), globalInfo->instructions, globalInfo->inWhile, globalInfo->inFunDef);
							if (errflg != PROGRAM_OK) return errflg;	//check for errors in PSA
							genIfCond(globalInfo->instructions, ifNumber, globalInfo->psaCounter, E, globalInfo->inWhile, globalInfo->inFunDef);
							if (errflg != PROGRAM_OK) return errflg;	//checko for errors in generator
							(globalInfo->psaCounter)++;
							//process then
							//must load token after calling PSA
							**tokenPP = getToken(globalInfo->file, *(globalInfo->GT));
							if (errflg != PROGRAM_OK) return errflg;	//check for lexical error from scanner
							if (then(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
							//process eol
							if (eol(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
							//process ST-LIST
							if (stlist(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
							//process else
							if (telse(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
							genIfElse(globalInfo->instructions, ifNumber, globalInfo->inWhile, globalInfo->inFunDef);
							if (errflg != PROGRAM_OK) return errflg;	//checko for errors in generator
							//process eol
							if (eol(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
							//process ST-LIST
							if (stlist(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
							//process end
							if (end(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
							genIfEnd(globalInfo->instructions, ifNumber, globalInfo->inWhile, globalInfo->inFunDef);
							if (errflg != PROGRAM_OK) return errflg;	//checko for errors in generator
							//inWhile = false, but only if this if is not nested in another while or if
							globalInfo->inWhile = !topLevelIf;
							return PROGRAM_OK;
						} else if (strcmp(keyW, "while") == 0) {
							//rule #12 STAT -> while expr do eol ST-LIST end
							//process while
							//don't call getToken, leave it for PSA
							//information about while:
							unsigned whileNumber = (globalInfo->whileCounter)++;
							bool topLevelWhile = !globalInfo->inWhile;
							globalInfo->inWhile = true;
							genWhileBegin(globalInfo->instructions, whileNumber, globalInfo->inWhile, globalInfo->inFunDef);
							if (errflg != PROGRAM_OK) return errflg;	//checko for errors in generator
							//process expr
							unsigned E = processExpression(globalInfo->file, "do", *(globalInfo->GT), *(globalInfo->currLT), globalInfo->instructions, globalInfo->inWhile, globalInfo->inFunDef);
							if (errflg != PROGRAM_OK) return errflg;	//check for errors in PSA
							genWhileCond(globalInfo->instructions, whileNumber, globalInfo->psaCounter, E, globalInfo->inWhile, globalInfo->inFunDef);
							if (errflg != PROGRAM_OK) return errflg;	//checko for errors in generator
							(globalInfo->psaCounter)++;
							//process do
							//must load token after calling PSA
							**tokenPP = getToken(globalInfo->file, *(globalInfo->GT));
							if (errflg != PROGRAM_OK) return errflg;	//check for lexical error from scanner
							if (tdo(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
							//process eol
							if (eol(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
							//process ST-LIST
							if (stlist(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
							//process end
							if (end(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
							
							genWhileEnd(globalInfo->instructions, whileNumber, globalInfo->inWhile, globalInfo->inFunDef);
							if (errflg != PROGRAM_OK) return errflg;	//checko for errors in generator
							//inWhile = false, but only if this while is not nested in another while
							globalInfo->inWhile = !topLevelWhile;
							return PROGRAM_OK;
						}
						return parserError(ERR_SYNTAX, __func__, type, KIND_UNEXP_KWD, keyW);
		case TOK_EOL:	//rule #13 STAT -> eps
						return PROGRAM_OK;
		default:		break;
	}
	return parserError(ERR_SYNTAX, __func__, type, KIND_OTHER, "");
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
			TsymData *newFuncData = NULL;
			globalInfo->inFunDef = true;
			//process def
			**tokenPP = getToken(globalInfo->file, *(globalInfo->GT)); //def is present, call next token
			if (errflg != PROGRAM_OK) return errflg;	//check for lexical error in scanner
			//process id
			if (id(tokenPP) != PROGRAM_OK) return errflg;
			string functionId = (*tokenPP)->data.s;		//save name of ID
			
			if ((newFuncData = symTabSearch(globalInfo->currLT, functionId)) != NULL) {	//id taken by global variable
				return parserError(ERR_SEM_DEFINE, __func__, 0, KIND_REDEF, functionId);
			} else if ((newFuncData = symTabSearch(globalInfo->GT, functionId)) != NULL) {	//func already is in GT
				if (!newFuncData->defined) {	
					//func was not defined, just called, it is OK
				} else { //redefinition
					return parserError(ERR_SEM_DEFINE, __func__, 0, KIND_REDEF, functionId);
				}
			} else {	//new uncalled function definition
				funcData = createDataForNewFunc(true, 0);	//false means undefined
				newFuncData = symTabInsert(globalInfo->GT, functionId, funcData);
				if (newFuncData == NULL) return errflg;
			}
			genFunDefBegin(globalInfo->instructions, functionId, globalInfo->inWhile, globalInfo->inFunDef);
			if (errflg != PROGRAM_OK) return errflg;	//checko for errors in generator
			**tokenPP = getToken(globalInfo->file, *(globalInfo->GT)); //id is present, call next token
			if (errflg != PROGRAM_OK) return errflg;
			//process (
			if (lbr(tokenPP, globalInfo) != PROGRAM_OK) return errflg;
			//process P-LIST
			globalInfo->paramCounter = 0;
			globalInfo->inFunCall = false;	//in function definition here
			globalInfo->currLT = &(newFuncData->LT);
			if (plist(tokenPP, globalInfo, functionId) != PROGRAM_OK) return errflg;
			if (!newFuncData->defined) {	//fun was called before, it must be defined with same amount of parameters
				if (newFuncData->params != globalInfo->paramCounter) {
					return parserError(ERR_SEM_PARAM, __func__, 0, KIND_OTHER, functionId);
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

			genFunDefEnd(globalInfo->instructions, globalInfo->inWhile, globalInfo->inFunDef);
			if (errflg != PROGRAM_OK) return errflg;	//checko for errors in generator
			globalInfo->currLT = globalInfo->mainLT;
			globalInfo->inFunDef = false;
			return PROGRAM_OK;
		} 
	} else {
		//syntax error
	}
	return parserError(ERR_SYNTAX, __func__, type, KIND_OTHER, "");
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
	return parserError(ERR_SYNTAX, __func__, type, KIND_OTHER, "");
}

/**
 * represents P-LIST non-terminal
 */
int plist(TToken **tokenPP, TWrapper *globalInfo, string function) {
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
						if (term(tokenPP, globalInfo, function) != PROGRAM_OK) return errflg;
						**tokenPP = getToken(globalInfo->file, *(globalInfo->GT)); //term is present, call next token
						if (errflg != PROGRAM_OK) return errflg;	//check for lexical error in scanner
						if (plist(tokenPP, globalInfo, function) != PROGRAM_OK) return errflg;
						return PROGRAM_OK;
		case TOK_COMMA: //rule #8 P-LIST -> , TERM P-LIST
						//process comma
						**tokenPP = getToken(globalInfo->file, *(globalInfo->GT)); //comma is present, call next token
						if (errflg != PROGRAM_OK) return errflg;	//check for lexical error in scanner
						(globalInfo->paramCounter)++;
						//process term
						if (term(tokenPP, globalInfo, function) != PROGRAM_OK) return errflg;
						**tokenPP = getToken(globalInfo->file, *(globalInfo->GT)); //term is present, call next token
						if (errflg != PROGRAM_OK) return errflg;	//check for lexical error in scanner
						//process plist
						if (plist(tokenPP, globalInfo, function) != PROGRAM_OK) return errflg;
						return PROGRAM_OK;
		case TOK_RBR:
		case TOK_EOL:	//rule #9 P-LIST -> eps
						return PROGRAM_OK;
		default:		break;
	}
	return parserError(ERR_SYNTAX, __func__, type, KIND_OTHER, "");						
}

/**
 * represents TERM non-terminal	
 * checks if loaded token is correct and calls generator
*/
int term(TToken **tokenPP, TWrapper *globalInfo, string function) {
	if (DEBUG) printf("Function:%s Token:%d\n", __func__, (*tokenPP)->type);
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	TsymData *varPtr = NULL;
	TsymData idData;
	TAdr var;
	unsigned order_blank = 0;
	switch (type) {
		case TOK_ID:	//rule #20 TERM -> id 
						if (globalInfo->inFunCall) {	//passing parameter to function
							if ((varPtr = symTabSearch(globalInfo->currLT, keyW)) == NULL) {	//try to pass undefined variable to funcall
								return parserError(ERR_SEM_DEFINE, __func__, 0, KIND_UNDEF_VAR, keyW);
							} else {
								var.type = ADRTYPE_VAR;
								var.val.s = keyW;
								genFunCallPar(globalInfo->instructions, function, globalInfo->paramCounter, var, varPtr->order, globalInfo->inWhile, globalInfo->inFunDef);
								if (errflg != PROGRAM_OK) return errflg;	//checko for errors in generator
							}
						} else { //define variable in new function's LT
							if (symTabSearch(globalInfo->GT, keyW) != NULL) {
								//keyword is already used for function
								return parserError(ERR_SEM_DEFINE, __func__, 0, KIND_REDEF, keyW);
							} else {
								idData = createDataForNewVar(globalInfo->paramCounter);
								symTabInsert(globalInfo->currLT, keyW, idData);
							}
						}
						return PROGRAM_OK;
		case TOK_STRING: //rule #21 TERM -> const
						if (globalInfo->inFunCall) {
							var.type = ADRTYPE_STRING;
							var.val.s = (*tokenPP)->data.s;
							genFunCallPar(globalInfo->instructions, function, globalInfo->paramCounter, var, order_blank, globalInfo->inWhile, globalInfo->inFunDef);
							if (errflg != PROGRAM_OK) return errflg;	//check for errors in generator
						} else {	//cannot define function with constant
							return parserError(ERR_SEM_OTHER, __func__, 0, KIND_OTHER, function);
						}
						return PROGRAM_OK;
		case TOK_FLOAT: //rule #21 TERM -> const
						if (globalInfo->inFunCall) {
							var.type = ADRTYPE_FLOAT;
							var.val.f = (*tokenPP)->data.f;
							genFunCallPar(globalInfo->instructions, function, globalInfo->paramCounter, var, order_blank, globalInfo->inWhile, globalInfo->inFunDef);
							if (errflg != PROGRAM_OK) return errflg;	//check for errors in generator
						} else {	//cannot define function with constant
							return parserError(ERR_SEM_OTHER, __func__, 0, KIND_OTHER, function);
						}
						return PROGRAM_OK;
		case TOK_INT:	//rule #21 TERM -> const
						if (globalInfo->inFunCall) {
							var.type = ADRTYPE_INT;
							var.val.i = (*tokenPP)->data.i;
							genFunCallPar(globalInfo->instructions, function, globalInfo->paramCounter, var, order_blank, globalInfo->inWhile, globalInfo->inFunDef);
							if (errflg != PROGRAM_OK) return errflg;	//check for errors in generator
						} else {	//cannot define function with constant
							return parserError(ERR_SEM_OTHER, __func__, 0, KIND_OTHER, function);
						}
						return PROGRAM_OK;
		case TOK_KEY:	if (strcmp(keyW, "nil") == 0) {
							//rule #22 TERM -> nil
							if (globalInfo->inFunCall) {
								var.type = ADRTYPE_NIL;
								var.val.s = (*tokenPP)->data.s;
								genFunCallPar(globalInfo->instructions, function, globalInfo->paramCounter, var, order_blank, globalInfo->inWhile, globalInfo->inFunDef);
								if (errflg != PROGRAM_OK) return errflg;	//check for errors in generator
							} else {	//cannot define function with constant
								return parserError(ERR_SEM_OTHER, __func__, 0, KIND_OTHER, function);
							}
							return PROGRAM_OK;
						}
		default:		break;
	}
	return parserError(ERR_SYNTAX, __func__, type, KIND_OTHER, "");
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
	TsymData *newVarData = NULL;
	TsymData newFuncData;
	TsymData *funcData = NULL;
	switch (type) {
		case TOK_ASSIGN: //rule #14 ASS-OR-FUN -> = ASSIGN
						**tokenPP = getToken(globalInfo->file, *(globalInfo->GT));	//= is present, call next token
						if (errflg != PROGRAM_OK) return errflg;	//check for lexical error in scanner
						//process Lvalue Var ID
						if ((newVarData = symTabSearch(globalInfo->currLT, savedIdOne)) == NULL) {	//ID not present in current LT
							if (symTabSearch(globalInfo->GT, savedIdOne) == NULL) {	//ID not defined as function
								genDefVar(globalInfo->instructions, savedIdOne, globalInfo->inWhile, globalInfo->inFunDef);
								if (errflg != PROGRAM_OK) return errflg;	//check for errors in generator
								varData = createDataForNewVar(0);	//0 means not parameter, just variable
								symTabInsert(globalInfo->currLT, savedIdOne, varData);
								newVarData = &varData;
							} else {
								return parserError(ERR_SEM_DEFINE, __func__, 0, KIND_REDEF, savedIdOne);
							}
						} //else Variable already defined, which is OK
						//process assign
						if (assign(tokenPP, globalInfo, savedIdOne, newVarData) != PROGRAM_OK) return errflg;
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
						if ((funcData = symTabSearch(globalInfo->currLT, savedIdOne)) != NULL) {	//id name is already taken by local variable
							return parserError(ERR_SEM_DEFINE, __func__, 0, KIND_REDEF, savedIdOne);
						} else if ((funcData = symTabSearch(globalInfo->mainLT, savedIdOne)) != NULL) {	//id name is taken by global variable
							return parserError(ERR_SEM_DEFINE, __func__, 0, KIND_REDEF, savedIdOne);
						} else if ((funcData = symTabSearch(globalInfo->GT, savedIdOne)) != NULL) {
							//function is defined
						} else {
							//function call before definition
							return parserError(ERR_SEM_DEFINE, __func__, 0, KIND_UNDEF_FUN, savedIdOne);
						}
						globalInfo->paramCounter = 0;
						genFunCallBegin(globalInfo->instructions, savedIdOne, globalInfo->inWhile, globalInfo->inFunDef);
						if (errflg != PROGRAM_OK) return errflg;	//check for errors in generator
						globalInfo->inFunCall = true;
						//call pbody, store params and count them
						if (pbody(tokenPP, globalInfo, savedIdOne) != PROGRAM_OK) return errflg; 
						//if amount of parameters is wrong, semantic error (only for defined function)
						if (funcData->defined) {	//function was defined before, amount of parameters must equal
							if (funcData->params == UNLIMITED_P && globalInfo->paramCounter != 0) {
								//it is ok, function has variable amount of parameters
							} else if (globalInfo->paramCounter != funcData->params) {
								return parserError(ERR_SEM_PARAM, __func__, 0, KIND_OTHER, savedIdOne);
							} 
						} else if (funcData->called) {	//function was called before, amounts must match 
							if (funcData->params != globalInfo->paramCounter) {
								return parserError(ERR_SEM_PARAM, __func__, 0, KIND_OTHER, savedIdOne);
							}
						} else {	//first call of undefined function, store amount of parameters counted
							funcData->params = globalInfo->paramCounter;
							funcData->called = true;
						}
						//function call
						genFunCallEnd(globalInfo->instructions, savedIdOne, globalInfo->inWhile, globalInfo->inFunDef);
						if (errflg != PROGRAM_OK) return errflg;	//check for errors in generator
						globalInfo->inFunCall = false;
						return PROGRAM_OK;
		default:		break;
	}
	return parserError(ERR_SYNTAX, __func__, type, KIND_OTHER, "");
}

/**
 * represents ASSIGN non-terminal
 * @param savedIdOne is the name of the first ID in recently red sequence: id = 
 */
int assign(TToken **tokenPP, TWrapper *globalInfo, string savedIdOne, TsymData *varData) {
	if (DEBUG) printf("Function:%s Token:%d\n", __func__, (*tokenPP)->type);
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	string savedIdTwo;
	switch (type) {
		case TOK_EOL:	//rule #23 ASSIGN -> eps
						return PROGRAM_OK;
		case TOK_KEY:	if (strcmp(keyW, "nil") == 0) {
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
						//**tokenPP = getToken(globalInfo->file, *(globalInfo->GT)); //expr is present, call next token //DONT CALL NEXT TOKEN
						unsigned E = processExpression(globalInfo->file, "eol", *(globalInfo->GT), *(globalInfo->currLT), globalInfo->instructions, globalInfo->inWhile, globalInfo->inFunDef);
						if (errflg != PROGRAM_OK) return errflg;	//check for errors in PSA
						genAssign(globalInfo->instructions, savedIdOne, varData->order, globalInfo->psaCounter, E, globalInfo->inWhile, globalInfo->inFunDef);	//assign to savedIdOne
						if (errflg != PROGRAM_OK) return errflg;	//check for errors in generator
						(globalInfo->psaCounter)++;
						//must load token after calling PSA
						**tokenPP = getToken(globalInfo->file, *(globalInfo->GT));
						if (errflg != PROGRAM_OK) return errflg;	//check for lexical error from scanner
						return PROGRAM_OK;
		case TOK_ID:	savedIdTwo = (*tokenPP)->data.s;
						if (decideExprOrFunc(tokenPP, globalInfo, savedIdOne, savedIdTwo, varData) != PROGRAM_OK) return errflg;
						return PROGRAM_OK;
		default:		break;						
	}
	return parserError(ERR_SYNTAX, __func__, type, KIND_OTHER, "");
}

/**
 * decides if there is an expression or function
 * 
 * eventually passes program to PSA
 * @param savedIdOne is the name of the first ID in recently red sequence: id = id
 * @param savedIdTwo is the name of the second ID in recently red sequence: id = id
 */
int decideExprOrFunc(TToken **tokenPP, TWrapper *globalInfo, string savedIdOne, string savedIdTwo, TsymData *varData) {
	if (DEBUG) printf("Function:%s Token:%d\n", __func__, (*tokenPP)->type);
	TToken bufferToken = **tokenPP;
	**tokenPP = getToken(globalInfo->file, *(globalInfo->GT)); //id is present, but I need another token to decide
	if (errflg != PROGRAM_OK) return errflg;	//check for lexical error in scanner
	TsymData newFuncData;
	TsymData *funcData = NULL;
	switch ((*tokenPP)->type) {
		case TOK_EOL:	//rules #17 and #16 both work, it is a simple function id = fun or id = id expression;
						returnToken(**tokenPP); //must return the eol though, so stat can read it
						if (symTabSearch(globalInfo->GT, savedIdTwo) != NULL) {	//ID is function
							if (newFuncData.params == 0) {	
								// id = fun () // rule #16
								genFunCallBegin(globalInfo->instructions, savedIdTwo, globalInfo->inWhile, globalInfo->inFunDef);
								if (errflg != PROGRAM_OK) return errflg;	//check for errors in generator
								genFunCallEnd(globalInfo->instructions, savedIdTwo, globalInfo->inWhile, globalInfo->inFunDef);
								if (errflg != PROGRAM_OK) return errflg;	//check for errors in generator
								genAssignRetval(globalInfo->instructions, savedIdOne, varData->order, globalInfo->inWhile, globalInfo->inFunDef);	//to savedIdOne, from funcall
								if (errflg != PROGRAM_OK) return errflg;	//check for errors in generator
								//genASSIGN(savedIdOne, funcall);	//to savedIdOne, from funcall
								return PROGRAM_OK;
							} else {	//0 is wrong amount of parameters
								return parserError(ERR_SEM_PARAM, __func__, 0, KIND_OTHER, savedIdTwo);
							}
						} else if (symTabSearch(globalInfo->currLT, savedIdTwo) != NULL) {	//ID is variable
							// id = id // continue to rule #17
						} else {	//function call before definition
							return parserError(ERR_SEM_DEFINE, __func__, 0, KIND_UNDEF_FUN, savedIdTwo);
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
						unsigned E = processExpression(globalInfo->file, "eol", *(globalInfo->GT), *(globalInfo->currLT), globalInfo->instructions, globalInfo->inWhile, globalInfo->inFunDef);
						if (errflg != PROGRAM_OK) return errflg;	//check for errors in PSA
						genAssign(globalInfo->instructions, savedIdOne, varData->order, globalInfo->psaCounter, E, globalInfo->inWhile, globalInfo->inFunDef);	//to savedIdOne
						if (errflg != PROGRAM_OK) return errflg;	//check for errors in generator
						(globalInfo->psaCounter)++;
						//must load token after calling PSA
						**tokenPP = getToken(globalInfo->file, *(globalInfo->GT));
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
						if ((funcData = symTabSearch(globalInfo->currLT, savedIdTwo)) != NULL) { 
							//id name is already taken by local variable
							return parserError(ERR_SEM_DEFINE, __func__, 0, KIND_REDEF, savedIdTwo);
						} else if ((funcData = symTabSearch(globalInfo->mainLT, savedIdTwo)) != NULL) { 
							//id name is already taken by global variable
							return parserError(ERR_SEM_DEFINE, __func__, 0, KIND_REDEF, savedIdTwo);
						} else if ((funcData = symTabSearch(globalInfo->GT, savedIdTwo)) != NULL) {
							//function is defined
						} else {
							//function call before definition
							return parserError(ERR_SEM_DEFINE, __func__, 0, KIND_UNDEF_FUN, savedIdTwo);
						}
						globalInfo->paramCounter = 0;
						genFunCallBegin(globalInfo->instructions, savedIdTwo, globalInfo->inWhile, globalInfo->inFunDef);
						if (errflg != PROGRAM_OK) return errflg;	//check for errors in generator
						globalInfo->inFunCall = true;
						//call pbody, store params and count them
						if (pbody(tokenPP, globalInfo, savedIdTwo) != PROGRAM_OK) return errflg; 
						//if amount of parameters is wrong, semantic error (only for defined function)
						if (funcData->defined) {	//function was defined before, amount of parameters must equal
							if (funcData->params == UNLIMITED_P && globalInfo->paramCounter != 0) {
								//it is ok, function has variable amount of parameters
							} else if (globalInfo->paramCounter != funcData->params) {
								return parserError(ERR_SEM_PARAM, __func__, 0, KIND_OTHER, savedIdTwo);
							} 
						} else if (funcData->called) {	//function was called before, amounts must match 
							if (funcData->params != globalInfo->paramCounter) {
								return parserError(ERR_SEM_PARAM, __func__, 0, KIND_OTHER, savedIdTwo);
							}
						} else {	//first call of undefined function
							funcData->params = globalInfo->paramCounter;
							funcData->called = true;
						}
						//function call
						genFunCallEnd(globalInfo->instructions, savedIdTwo, globalInfo->inWhile, globalInfo->inFunDef);
						if (errflg != PROGRAM_OK) return errflg;	//check for errors in generator
						globalInfo->inFunCall = false;
						genAssignRetval(globalInfo->instructions, savedIdOne, varData->order, globalInfo->inWhile, globalInfo->inFunDef);	//to savedIdOne, from funcall
						if (errflg != PROGRAM_OK) return errflg;	//check for errors in generator
						return PROGRAM_OK;
		default:		break;
	}
	return parserError(ERR_SYNTAX, __func__, (*tokenPP)->type, KIND_OTHER, "");
}

/**
 * represents pbody non-terminal
 * pbody represents syntax for inserting parameters when calling a function like '(a, b, c)'
 */
int pbody(TToken **tokenPP, TWrapper *globalInfo, string function) {
	if (DEBUG) printf("Function:%s Token:%d\n", __func__, (*tokenPP)->type);
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	switch (type) {
		case TOK_LBR:	//rule #18 P-BODY -> ( P-LIST )
						//process (
						**tokenPP = getToken(globalInfo->file, *(globalInfo->GT)); //( is present, call next token
						if (errflg != PROGRAM_OK) return errflg;
						//process P-LIST
						if (plist(tokenPP, globalInfo, function) != PROGRAM_OK) return errflg;
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
						if (plist(tokenPP, globalInfo, function) != PROGRAM_OK) return errflg;
						return PROGRAM_OK;
		default:		break;						
	}
	return parserError(ERR_SYNTAX, __func__, type, KIND_OTHER, "");
}

//--------------RECURSIVE DESCENT - TERMINALS--------------

/**
 * eol terminal
 */
int eol(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("Function:%s Token:%d\n", __func__, (*tokenPP)->type);
	TTokenType type = (*tokenPP)->type;
	if (type == TOK_EOL) {
		**tokenPP = getToken(globalInfo->file, *(globalInfo->GT)); //eol is present, call next token
		if (errflg != PROGRAM_OK) return errflg;
		return PROGRAM_OK;
	} else {
		return parserError(ERR_SYNTAX, __func__, type, KIND_OTHER, "");
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
		**tokenPP = getToken(globalInfo->file, *(globalInfo->GT)); //then is present, call next token
		if (errflg != PROGRAM_OK) return errflg;	//check for lexical error in scanner
		return PROGRAM_OK;
	} else {
		return parserError(ERR_SYNTAX, __func__, type, KIND_UNEXP_KWD, keyW);
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
		**tokenPP = getToken(globalInfo->file, *(globalInfo->GT)); //else is present, call next token
		if (errflg != PROGRAM_OK) return errflg;
		return PROGRAM_OK;
	} else {
		return parserError(ERR_SYNTAX, __func__, type, KIND_UNEXP_KWD, keyW);
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
		**tokenPP = getToken(globalInfo->file, *(globalInfo->GT)); //do is present, call next token
		if (errflg != PROGRAM_OK) return errflg;	//check for lexical error in scanner
		return PROGRAM_OK;
	} else {
		return parserError(ERR_SYNTAX, __func__, type, KIND_UNEXP_KWD, keyW);
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
		**tokenPP = getToken(globalInfo->file, *(globalInfo->GT)); //end is present, call next token
		if (errflg != PROGRAM_OK) return errflg;
		return PROGRAM_OK;
	} else {
		return parserError(ERR_SYNTAX, __func__, type, KIND_UNEXP_KWD, keyW);
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
		return parserError(ERR_SYNTAX, __func__, type, KIND_OTHER, "");
	}
}

/**
 * left bracket terminal
 */
int lbr(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("Function:%s Token:%d\n", __func__, (*tokenPP)->type);
	TTokenType type = (*tokenPP)->type;
	if (type == TOK_LBR) {
		**tokenPP = getToken(globalInfo->file, *(globalInfo->GT)); //lbr is present, call next token
		if (errflg != PROGRAM_OK) return errflg;
		return PROGRAM_OK;
	} else {
		return parserError(ERR_SYNTAX, __func__, type, KIND_OTHER, "");
	}
}

/**
 * right bracket terminal
 */
int rbr(TToken **tokenPP, TWrapper *globalInfo) {
	if (DEBUG) printf("Function:%s Token:%d\n", __func__, (*tokenPP)->type);
	TTokenType type = (*tokenPP)->type;
	if (type == TOK_RBR) {
		**tokenPP = getToken(globalInfo->file, *(globalInfo->GT)); //rbr is present, call next token
		if (errflg != PROGRAM_OK) return errflg;
		return PROGRAM_OK;
	} else {
		return parserError(ERR_SYNTAX, __func__, type, KIND_OTHER, "");
	}
}

//--------------OTHER FUNCTIONS----------------------

/**
 * takes care about errors called in parser.c
 * @param errNbr is error number
 * @param where is name of parser.c function where the error occured
 * @tokenType is number of token that came (only used for syntax error)
 */ 
int parserError(int errNbr, const char *where, int tokenType, int errKind, string data) {
	if (DEBUG) printf("error in func. %s", where);
	errflg = errNbr;
	if (tokenType == TOK_EOL)
		linecnt--;
	switch(errNbr) {
		case ERR_SEM_DEFINE:	
			if (errKind == KIND_UNDEF_VAR)
				ifjErrorPrint("SEMANTIC ERROR %d: Undefined variable %s!\n", errflg, data);
			else if (errKind == KIND_UNDEF_FUN)
				ifjErrorPrint("SEMANTIC ERROR %d: Undefined function %s()!\n", errflg, data);
			else if (errKind == KIND_REDEF)
				ifjErrorPrint("SEMANTIC ERROR %d: Redefinition of %s!\n", errflg, data);
			break;
		case ERR_SEM_PARAM:
			ifjErrorPrint("SEMANTIC ERROR %d: Wrong amount of parameters for %s()!\n", errflg, data);
			break;
		case ERR_SEM_OTHER:		
			ifjErrorPrint("SEMANTIC ERROR %d: Try to define function with a constant in %s()!\n", errflg, data); 
			break;
		case ERR_SYNTAX:
			if (tokenType == TOK_KEY && errKind == KIND_UNEXP_KWD)
				ifjErrorPrint("SYNTAX ERROR %d: Unexpected keyword %s!\n", errflg, data);
			else
				ifjErrorPrint("SYNTAX ERROR %d: Unexpected token type n.%d!\n", errflg, tokenType);
			break;
		case ERR_RUNTIME: 		
			ifjErrorPrint("RUNTIME ERROR %d: Allocation failed!\n", errflg);
		default: break;
	}
	return errflg;
}
 
 /**
  * initializes TWrapper structure
  */ 
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
	info->inFunDef = false;
}

/**
 * creates data structure for new function
 * includes creating inner LT
 */ 
TsymData createDataForNewFunc(bool defined, int params) {
	TsymData data;
	data.type = TYPE_FUN;
	data.order = 0;				//never used
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
	data.defined = false;	//never used
	data.called = false;	//never used
	data.params = 0;		//never used
	data.LT = NULL;			//never used (in dispose there is check if NULL)
	return data;
}


/** end of parser.c */
