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

int main(int argc, char *argv[]) {
	bool success = false;
	TToken *token = (TToken *) malloc(sizeof(TToken));
	if (token == NULL) {
		error("ERROR %d in parser.c in func. main: allocation failed!\n", ERR_RUNTIME);
		return ERR_RUNTIME;
	}
	TToken **tokenPtr = &token;
	FILE *f = fopen(stdin, r);
	
	//TODO: assume that getToken(f) vrací token.
	*tokenPtr = getToken(f);
	if (*tokenPtr != NULL) {
		success = start(tokenPtr, f);
	}
	return 0;
}

/**
	represents START non-terminal
	
	assert *tokenPtr != NULL
*/
bool start(TToken **tokenPtr, FILE *f) {
	bool start = false;
	TTokenType type = (*tokenPtr)->type;
	string keyW = (*tokenPtr)->data.s;
	switch (type) {
		case TOK_KEY:	if (strcmp(keyW, "def") == 0) {	
							//rule #2 START -> FUN-DEF eol S
							start = fundef(tokenPtr, f) && eol(tokenPtr, f) && start(tokenPtr, f);
							break;
						} else if (strcmp(keyW, "if") == 0 || strcmp(keyW, "while") == 0) {
							//continue to rule #1 
						} else
							break;
		case TOK_EOL:
		case TOK_ID:	//rule #1 START -> STAT eol S
						start = stat(tokenPtr, f) && eol(tokenPtr, f) && start(tokenPtr, f);
						break;
		case TOK_EOF:	//rule #3 START -> eof
						start = true;
						*tokenPtr = getToken(f);
						break;
		default:		error("ERROR %d in parser.c in func. start: unexpected token n. %d!\n", ERR_SYNTAX, type);
						break;						
	}
	return start;
}

/**
	represents STAT non-terminal
*/
bool stat(TToken **tokenPtr, FILE *f) {
	bool stat = false;
	TTokenType type = (*tokenPtr)->type;
	string keyW = (*tokenPtr)->data.s;
	switch (type) {
		case TOK_ID:	//rule #10 STAT -> id ASS-OR-FUN
						*tokenPtr = getToken(f); //cover id
						stat = assorfun(tokenPtr, f);
						break;
		case TOK_KEY:	if (strcmp(keyW, "if") == 0) {
							//rule #11 STAT -> if expr then eol ST-LIST else eol ST-LIST end
							*tokenPtr = getToken(f); //if is present, call next token
							stat = expr(tokenPtr, f) && then(tokenPtr, f) && eol(tokenPtr, f) && stlist(tokenPtr, f) &&
								   telse(tokenPtr, f) && eol(tokenPtr, f) && stlist(tokenPtr, f) && end(tokenPtr, f);
						} else if (strcmp(keyW, "while") == 0) {
							//rule #12 STAT -> while expr do eol ST-LIST end
							*tokenPtr = getToken(f); //while is present, call next token
							stat = expr(tokenPtr, f) && tdo(tokenPtr, f) && eol(tokenPtr, f) && stlist(tokenPtr, f) && end(tokenPtr, f);
						}
						break;
		case TOK_EOL:	//rule #13 STAT -> eps
						return true;
						break;
		default:		error("ERROR %d in parser.c in func. stat: unexpected token n. %d!\n", ERR_SYNTAX, type);
						break;							
	}
	return stat;
}

/**
	represents FUN-DEF non-terminal
*/
bool fundef(TToken **tokenPtr, FILE *f) {
	bool fundef = false;
	TTokenType type = (*tokenPtr)->type;
	string keyW = (*tokenPtr)->data.s;
	
	if (type == TOK_KEY) {
		if (strcmp(keyW, "def") == 0) {
			//rule #6 FUN-DEF -> def id ( P-LIST ) eol ST-LIST end
			*tokenPtr = getToken(f); //def is present, call next token
			fundef = id(tokenPtr, f) && lbr(tokenPtr, f) && plist(tokenPtr, f) && rbr(tokenPtr, f) &&
					 eol(tokenPtr, f) && stlist(tokenPtr, f) && end(tokenPtr, f);
		} 
	} else {
		error("ERROR %d in parser.c in func. fundef: unexpected token n. %d!\n", ERR_SYNTAX, type);
	} 
	return fundef;
}

/**
	represents ST-LIST non-terminal
*/
bool stlist(TToken **tokenPtr, FILE *f) {
	bool stlist = false;
	TTokenType type = (*tokenPtr)->type;
	string keyW = (*tokenPtr)->data.s;
	switch (type) {
		case TOK_KEY:	if (strcmp(keyW, "else") == 0 || strcmp(keyW, "end") == 0) {
							//rule #5 ST-LIST -> eps
							*tokenPtr = getToken(f);
							stlist = true;
							break;
						} else if (strcmp(keyW, "if") == 0 || strcmp(keyW, "while") == 0) {
							//continue to rule #4
						} else 
							break;
		case TOK_ID:
		case TOK_EOL:	//rule #4 ST-LIST -> STAT eol ST-LIST
						stlist = stat(tokenPtr, f) && eol(tokenPtr, f) && stlist(tokenPtr, f);
						break;
		default:		error("ERROR %d in parser.c in func. stlist: unexpected token n. %d!\n", ERR_SYNTAX, type);
						break;						
	}
	return stlist;
}

/**
	represents P-LIST non-terminal
*/
bool plist(TToken **tokenPtr, FILE *f) {
	bool plist = false;
	TTokenType type = (*tokenPtr)->type;
	string keyW = (*tokenPtr)->data.s;
	switch (type) {
		case TOK_KEY:	if (strcmp(keyW, "if") == 0 ) {
							//continue to rule #7
						} else 
							break;
		case TOK_STRING:
		case TOK_FLOAT:
		case TOK_INT:
		case TOK_NIL:	//rule #7 P-LIST -> TERM P-LIST
						plist = term(tokenPtr, f) && plist(tokenPtr, f);
						break;
		case TOK_COMMA: //rule #8 P-LIST -> , TERM P-LIST
						*tokenPtr = getToken(f); //comma is present, call next token
						plist = term(tokenPtr, f) && plist(tokenPtr, f);
						break;
		case TOK_RIGHT_BRACKET:
		case EOL:		//rule #9 P-LIST -> eps
						return true;
						break;
		default:		error("ERROR %d in parser.c in func. plist: unexpected token n. %d!\n", ERR_SYNTAX, type);
						break;						
	}
	return plist;
}

/**
	represents TERM non-terminal
*/
bool term(TToken **tokenPtr, FILE *f) {
	bool term = false;
	TTokenType type = (*tokenPtr)->type;
	switch (type) {
		case TOK_ID:	//rule #20 TERM -> id
						*tokenPtr = getToken(f); //id is present, call next token
						return true;
		case TOK_STRING:
		case TOK_FLOAT:
		case TOK_INT:	//rule #21 TERM -> const
						*tokenPtr = getToken(f); //const is present, call next token
						return true;
		case TOK_NIL:	//rule #22 TERM -> nil
						*tokenPtr = getToken(f); //nil is present, call next token
						return true;
						break;
		default:		error("ERROR %d in parser.c in func. term: unexpected token n. %d!\n", ERR_SYNTAX, type);
						break;						
	}
	return term;
}

/**
	represents ASS-OR-FUN non-terminal
*/
bool assorfun(TToken **tokenPtr, FILE *f) {
	bool assorfun = false;
	TTokenType type = (*tokenPtr)->type;
	switch (type) {
		case TOK_ASSIGN: //rule #14 ASS-OR-FUN -> = ASSIGN
						*tokenPtr = getToken(f); //= is present, call next token
						assorfun = assign(tokenPtr, f);
						break;
		case TOK_COMMA:
		case TOK_RIGHT_BRACKET:
		case TOK_NIL:		
		case TOK_STRING:
		case TOK_FLOAT:
		case TOK_INT:
		case TOK_ID:
		case TOK_EOL:	//rule #15 ASS-OR-FUN -> P-BODY
						assorfun = pbody(tokenPtr, f);
						break;
		default:		error("ERROR %d in parser.c in func. assorfun: unexpected token n. %d!\n", ERR_SYNTAX, type);
						break;						
	}
	return assorfun;
}

/**
	represents ASSIGN non-terminal
*/
bool assign(TToken **tokenPtr, FILE *f) {
	bool assign = false;
	TTokenType type = (*tokenPtr)->type;
	switch (type) {
		case TOK_ID:	//rule #16 ASSIGN -> id P-BODY
						*tokenPtr = getToken(f); //id is present, call next token
						assign = pbody(tokenPtr, f);
						break;
		// TODO: case EXPRESSION
		default:		error("ERROR %d in parser.c in func. assign: unexpected token n. %d!\n", ERR_SYNTAX, type);
						break;						
	}
	return assign;
}

/**
	represents pbody non-terminal
	pbody represents syntax for inserting parameters when calling a function like '(a, b, c)'
*/
bool pbody(TToken **tokenPtr, FILE *f) {
	bool pbody = false;
	TTokenType type = (*tokenPtr)->type;
	switch (type) {
		case TOK_LEFT_BRACKET: //rule #18 P-BODY -> ( P-LIST )
						*tokenPtr = getToken(f); //( is present, call next token
						pbody = plist(tokenPtr, f) && rbr(tokenPtr, f);
						break;
		case TOK_COMMA:
		case TOK_NIL:		
		case TOK_STRING:
		case TOK_FLOAT:
		case TOK_INT:
		case TOK_ID:
		case TOK_EOL:	//rule #19 P-BODY -> P-LIST
						pbody = plist(tokenPtr, f);
						break;
		default:		error("ERROR %d in parser.c in func. pbody: unexpected token n. %d!\n", ERR_SYNTAX, type);
						break;						
	}
	return pbody;
}

/**
	represents eol terminal processing
*/
bool eol(TToken **tokenPtr, FILE *f) {
	bool eol = false;
	TTokenType type = (*tokenPtr)->type;
	if (type == TOK_EOL) {
		*tokenPtr = getToken(f); //eol is present, call next token
		eol = true;
	} else 
		error("ERROR %d in parser.c in func. eol: unexpected token n. %d!\n", ERR_SYNTAX, type);
	return eol;
}

/**
	represents expr terminal processing
*//*
bool expr(TToken **tokenPtr, FILE *f) {
	bool expr = false;
	TTokenType type = (*tokenPtr)->type;
	if (type == TOK_EOL) {
		*tokenPtr = getToken(f); //eol is present, call next token
		eol = true;
	} else 
		error("ERROR %d in parser.c in func. expr: unexpected token n. %d!\n", ERR_SYNTAX, type);
	return expr;
}
*/

/**
	represents then terminal processing
*/
bool then(TToken **tokenPtr, FILE *f) {
	bool then = false;
	TTokenType type = (*tokenPtr)->type;
	string keyW = (*tokenPtr)->data.s;
	if (type == TOK_KEY && (strcmp(keyW, "then") == 0)) {
		*tokenPtr = getToken(f); //then is present, call next token
		then = true;
	} else 
		error("ERROR %d in parser.c in func. then: unexpected token n. %d or keyword %s!\n", ERR_SYNTAX, type, keyW);
	return then;
}

/**
	represents else terminal processing
*/
bool telse(TToken **tokenPtr, FILE *f) {
	bool telse = false;
	TTokenType type = (*tokenPtr)->type;
	string keyW = (*tokenPtr)->data.s;
	if (type == TOK_KEY && (strcmp(keyW, "else") == 0)) {
		*tokenPtr = getToken(f); //else is present, call next token
		telse = true;
	} else 
		error("ERROR %d in parser.c in func. telse: unexpected token n. %d or keyword %s!\n", ERR_SYNTAX, type, keyW);
	return telse;
}


/**
	represents end terminal processing
*/
bool end(TToken **tokenPtr, FILE *f) {
	bool end = false;
	TTokenType type = (*tokenPtr)->type;
	string keyW = (*tokenPtr)->data.s;
	if (type == TOK_KEY && (strcmp(keyW, "end") == 0)) {
		*tokenPtr = getToken(f); //end is present, call next token
		end = true;
	} else 
		error("ERROR %d in parser.c in func. end: unexpected token n. %d or keyword %s!\n", ERR_SYNTAX, type, keyW);
	return end;
}

/**
	represents do terminal processing
*/
bool tdo(TToken **tokenPtr, FILE *f) {
	bool tdo = false;
	TTokenType type = (*tokenPtr)->type;
	string keyW = (*tokenPtr)->data.s;
	if (type == TOK_KEY && (strcmp(keyW, "do") == 0)) {
		*tokenPtr = getToken(f); //do is present, call next token
		tdo = true;
	} else 
		error("ERROR %d in parser.c in func. tdo: unexpected token n. %d or keyword %s!\n", ERR_SYNTAX, type, keyW);
	return tdo;
}

/**
	represents id terminal processing
*/
bool id(TToken **tokenPtr, FILE *f) {
	bool id = false;
	TTokenType type = (*tokenPtr)->type;
	if (type == TOK_ID) {
		*tokenPtr = getToken(f); //id is present, call next token
		id = true;
	} else 
		error("ERROR %d in parser.c in func. id: unexpected token n. %d!\n", ERR_SYNTAX, type);
	return id;
}

/**
	represents left bracket terminal processing
*/
bool lbr(TToken **tokenPtr, FILE *f) {
	bool lbr = false;
	TTokenType type = (*tokenPtr)->type;
	if (type == TOK_LEFT_BRACKET) {
		*tokenPtr = getToken(f); //lbr is present, call next token
		lbr = true;
	} else 
		error("ERROR %d in parser.c in func. lbr: unexpected token n. %d!\n", ERR_SYNTAX, type);
	return lbr;
}

/**
	represents right bracket terminal processing
*/
bool rbr(TToken **tokenPtr, FILE *f) {
	bool rbr = false;
	TTokenType type = (*tokenPtr)->type;
	if (type == TOK_RIGHT_BRACKET) {
		*tokenPtr = getToken(f); //rbr is present, call next token
		rbr = true;
	} else 
		error("ERROR %d in parser.c in func. rbr: unexpected token n. %d!\n", ERR_SYNTAX, type);
	return rbr;
}

/** end of parser.c