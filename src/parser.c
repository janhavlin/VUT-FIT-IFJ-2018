/**
	file name:		parser.c
	project:		VUT-FIT-IFJ-2018
	created:		19.11.2018
	last modified:	19.11.2018
	
	created by: 	Jakub Karpíšek xkarpi06@stud.fit.vutbr.cz
	modifications:	
	
	description:	Heart of the compiler, includes main
*/

#include "parser.h"

#define DEBUG 0

/** ##################### MAIN ############################ */

int main() {
	bool success = false;
	TToken *token = (TToken *) malloc(sizeof(TToken));
	if (token == NULL) {
		error("ERROR %d in parser.c in func. main: allocation failed!\n", ERR_RUNTIME);
		return ERR_RUNTIME;
	}
	TToken **tokenPP = &token;
	FILE *f = stdin;
	
	//TODO: assume that getToken(f) vrací token.
	**tokenPP = getToken(f);
	if (*tokenPP != NULL) {
		success = start(tokenPP, f);
	}
	if (success) 
		printf("Syntax OK!\n");
	else 
		printf("Syntax ERROR!!!\n");
	
	free(token);
	return 0;
}

/**
	represents START non-terminal
	
	assert *tokenPP != NULL
*/
bool start(TToken **tokenPP, FILE *f) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	switch (type) {
		case TOK_KEY:	if (strcmp(keyW, "def") == 0) {	
							//rule #2 START -> FUN-DEF eol S
							value = fundef(tokenPP, f) && eol(tokenPP, f) && start(tokenPP, f);
							break;
						} else if (strcmp(keyW, "if") == 0 || strcmp(keyW, "while") == 0) {
							//continue to rule #1 
						} else
							break;
		case TOK_EOL:
		case TOK_ID:	//rule #1 START -> STAT eol S
						value = stat(tokenPP, f) && eol(tokenPP, f) && start(tokenPP, f);
						break;
		case TOK_EOF:	//rule #3 START -> eof
						value = true;
						**tokenPP = getToken(f);
						break;
		default:		error("ERROR %d in parser.c in func. start: unexpected token n. %d!\n", ERR_SYNTAX, type);
						break;						
	}
	return value;
}

/**
	represents STAT non-terminal
*/
bool stat(TToken **tokenPP, FILE *f) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	switch (type) {
		case TOK_ID:	//rule #10 STAT -> id ASS-OR-FUN
						**tokenPP = getToken(f); //cover id
						value = assorfun(tokenPP, f);
						break;
		case TOK_KEY:	if (strcmp(keyW, "if") == 0) {
							//rule #11 STAT -> if expr then eol ST-LIST else eol ST-LIST end
							**tokenPP = getToken(f); //if is present, call next token
							value = processExpression(f, "then") && then(tokenPP, f) && eol(tokenPP, f) && stlist(tokenPP, f) &&
								    telse(tokenPP, f) && eol(tokenPP, f) && stlist(tokenPP, f) && end(tokenPP, f);
						} else if (strcmp(keyW, "while") == 0) {
							//rule #12 STAT -> while expr do eol ST-LIST end
							**tokenPP = getToken(f); //while is present, call next token
							value = processExpression(f, "do") && tdo(tokenPP, f) && eol(tokenPP, f) && stlist(tokenPP, f) && end(tokenPP, f);
						}
						break;
		case TOK_EOL:	//rule #13 STAT -> eps
						return true;
		default:		error("ERROR %d in parser.c in func. stat: unexpected token n. %d!\n", ERR_SYNTAX, type);
						break;							
	}
	return value;
}

/**
	represents FUN-DEF non-terminal
*/
bool fundef(TToken **tokenPP, FILE *f) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	
	if (type == TOK_KEY) {
		if (strcmp(keyW, "def") == 0) {
			//rule #6 FUN-DEF -> def id ( P-LIST ) eol ST-LIST end
			**tokenPP = getToken(f); //def is present, call next token
			value = id(tokenPP, f) && lbr(tokenPP, f) && plist(tokenPP, f) && rbr(tokenPP, f) &&
					 eol(tokenPP, f) && stlist(tokenPP, f) && end(tokenPP, f);
		} 
	} else {
		error("ERROR %d in parser.c in func. fundef: unexpected token n. %d!\n", ERR_SYNTAX, type);
	} 
	return value;
}

/**
	represents ST-LIST non-terminal
*/
bool stlist(TToken **tokenPP, FILE *f) {
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
						value = stat(tokenPP, f) && eol(tokenPP, f) && stlist(tokenPP, f);
						break;
		default:		error("ERROR %d in parser.c in func. stlist: unexpected token n. %d!\n", ERR_SYNTAX, type);
						break;						
	}
	return value;
}

/**
	represents P-LIST non-terminal
*/
bool plist(TToken **tokenPP, FILE *f) {
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
						value = term(tokenPP, f) && plist(tokenPP, f);
						break;
		case TOK_COMMA: //rule #8 P-LIST -> , TERM P-LIST
						**tokenPP = getToken(f); //comma is present, call next token
						value = term(tokenPP, f) && plist(tokenPP, f);
						break;
		case TOK_RIGHT_BRACKET:
		case TOK_EOL:	//rule #9 P-LIST -> eps
						return true;
		default:		error("ERROR %d in parser.c in func. plist: unexpected token n. %d!\n", ERR_SYNTAX, type);
						break;						
	}
	return value;
}

/**
	represents TERM non-terminal
*/
bool term(TToken **tokenPP, FILE *f) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	switch (type) {
		case TOK_ID:	//rule #20 TERM -> id
						**tokenPP = getToken(f); //id is present, call next token
						return true;
		case TOK_STRING:
		case TOK_FLOAT:
		case TOK_INT:	//rule #21 TERM -> const
						**tokenPP = getToken(f); //const is present, call next token
						return true;
		case TOK_KEY:	if (strcmp(keyW, "nil") == 0) {
							//rule #22 TERM -> nil
							**tokenPP = getToken(f); //nil is present, call next token
							return true;
						} 
						break;
		default:		error("ERROR %d in parser.c in func. term: unexpected token n. %d!\n", ERR_SYNTAX, type);
						break;						
	}
	return value;
}

/**
	represents ASS-OR-FUN non-terminal
*/
bool assorfun(TToken **tokenPP, FILE *f) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	switch (type) {
		case TOK_ASSIGN: //rule #14 ASS-OR-FUN -> = ASSIGN
						**tokenPP = getToken(f); //= is present, call next token
						value = assign(tokenPP, f);
						break;
		case TOK_KEY:	if (strcmp(keyW, "nil") == 0) {
							//continue to rule #15
						} else 
							break;
		case TOK_COMMA:
		case TOK_LEFT_BRACKET:
		case TOK_STRING:
		case TOK_FLOAT:
		case TOK_INT:
		case TOK_ID:
		case TOK_EOL:	//rule #15 ASS-OR-FUN -> P-BODY
						value = pbody(tokenPP, f);
						break;
		default:		error("ERROR %d in parser.c in func. assorfun: unexpected token n. %d!\n", ERR_SYNTAX, type);
						break;						
	}
	return value;
}

/**
	represents ASSIGN non-terminal
*/
bool assign(TToken **tokenPP, FILE *f) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	switch (type) {
		case TOK_KEY:	if (strcmp(keyW, "nil")) {
							//continue to rule#17
						} else 
							break;
		case TOK_LEFT_BRACKET:
		case TOK_STRING:
		case TOK_FLOAT:
		case TOK_INT:
		case TOK_ADD:
		case TOK_SUB:	//rule #17 ASSIGN -> expr 
						//case 1 - found after examining 1 token, therefore return 1 token to scanner
						returnToken(**tokenPP);
						**tokenPP = getToken(f); //expr is present, call next token
						value = processExpression(f, "eol");
						break;
		case TOK_ID:	value = decideExprOrFunc(tokenPP, f);
						break;
		default:		error("ERROR %d in parser.c in func. assign: unexpected token n. %d!\n", ERR_SYNTAX, type);
						break;						
	}
	return value;
}

/**
 * decides if there is an expression or not
 * 
 * eventually passes program to PSA
 */
bool decideExprOrFunc(TToken **tokenPP, FILE *f) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	TToken bufferToken = **tokenPP;
	**tokenPP = getToken(f); //id is present, but I need another token to decide
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
						return processExpression(f, "eol");
		case TOK_KEY:		if (strcmp((*tokenPP)->data.s, "nil")==0) {
							//continue to rule #18
						} else 
							break;
		case TOK_LEFT_BRACKET:
		case TOK_STRING:
		case TOK_FLOAT:
		case TOK_INT:
		case TOK_ID:	//rule #18 ASSIGN -> id P-BODY
						//actually no need of returning any tokens to scanner
						return plist(tokenPP, f); 
		default:		error("ERROR %d in parser.c in func. decideExprOrFunc: unexpected token n. %d!\n", ERR_SYNTAX, (*tokenPP)->type);
						break;
	}
	return false;
}

/**
	represents pbody non-terminal
	pbody represents syntax for inserting parameters when calling a function like '(a, b, c)'
*/
bool pbody(TToken **tokenPP, FILE *f) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	switch (type) {
		case TOK_LEFT_BRACKET: //rule #18 P-BODY -> ( P-LIST )
						**tokenPP = getToken(f); //( is present, call next token
						value = plist(tokenPP, f) && rbr(tokenPP, f);
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
						value = plist(tokenPP, f);
						break;
		default:		error("ERROR %d in parser.c in func. pbody: unexpected token n. %d!\n", ERR_SYNTAX, type);
						break;						
	}
	return value;
}

/**
	represents eol terminal processing
*/
bool eol(TToken **tokenPP, FILE *f) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	if (type == TOK_EOL) {
		**tokenPP = getToken(f); //eol is present, call next token
		value = true;
	} else 
		error("ERROR %d in parser.c in func. eol: unexpected token n. %d!\n", ERR_SYNTAX, type);
	return value;
}

/**
	represents then terminal processing
*/
bool then(TToken **tokenPP, FILE *f) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	if (type == TOK_KEY && (strcmp(keyW, "then") == 0)) {
		**tokenPP = getToken(f); //then is present, call next token
		value = true;
	} else 
		error("ERROR %d in parser.c in func. then: unexpected token n. %d or keyword %s!\n", ERR_SYNTAX, type, keyW);
	return value;
}

/**
	represents else terminal processing
*/
bool telse(TToken **tokenPP, FILE *f) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	if (type == TOK_KEY && (strcmp(keyW, "else") == 0)) {
		**tokenPP = getToken(f); //else is present, call next token
		value = true;
	} else 
		error("ERROR %d in parser.c in func. telse: unexpected token n. %d or keyword %s!\n", ERR_SYNTAX, type, keyW);
	return value;
}


/**
	represents end terminal processing
*/
bool end(TToken **tokenPP, FILE *f) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	if (type == TOK_KEY && (strcmp(keyW, "end") == 0)) {
		**tokenPP = getToken(f); //end is present, call next token
		value = true;
	} else 
		error("ERROR %d in parser.c in func. end: unexpected token n. %d or keyword %s!\n", ERR_SYNTAX, type, keyW);
	return value;
}

/**
	represents do terminal processing
*/
bool tdo(TToken **tokenPP, FILE *f) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	string keyW = (*tokenPP)->data.s;
	if (type == TOK_KEY && (strcmp(keyW, "do") == 0)) {
		**tokenPP = getToken(f); //do is present, call next token
		value = true;
	} else 
		error("ERROR %d in parser.c in func. tdo: unexpected token n. %d or keyword %s!\n", ERR_SYNTAX, type, keyW);
	return value;
}

/**
	represents id terminal processing
*/
bool id(TToken **tokenPP, FILE *f) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	if (type == TOK_ID) {
		**tokenPP = getToken(f); //id is present, call next token
		value = true;
	} else 
		error("ERROR %d in parser.c in func. id: unexpected token n. %d!\n", ERR_SYNTAX, type);
	return value;
}

/**
	represents left bracket terminal processing
*/
bool lbr(TToken **tokenPP, FILE *f) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	if (type == TOK_LEFT_BRACKET) {
		**tokenPP = getToken(f); //lbr is present, call next token
		value = true;
	} else 
		error("ERROR %d in parser.c in func. lbr: unexpected token n. %d!\n", ERR_SYNTAX, type);
	return value;
}

/**
	represents right bracket terminal processing
*/
bool rbr(TToken **tokenPP, FILE *f) {
	if (DEBUG) printf("TOKEN: '%s' FUNCTION: %s\n", (*tokenPP)->data.s, __func__);
	bool value = false;
	TTokenType type = (*tokenPP)->type;
	if (type == TOK_RIGHT_BRACKET) {
		**tokenPP = getToken(f); //rbr is present, call next token
		value = true;
	} else 
		error("ERROR %d in parser.c in func. rbr: unexpected token n. %d!\n", ERR_SYNTAX, type);
	return value;
}

/** end of parser.c */