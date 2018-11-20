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
	t_token *token = (t_token *) malloc(sizeof(t_token));
	if (token == NULL) {
		error("ERROR in parser.c in func. main: allocation failed! Code=%d\n", ERR_RUNTIME);
		return ERR_RUNTIME;
	}
	t_token **tokenPtr = &token;
	FILE *f = fopen(stdin, r);
	
	//TODO: assume that getToken(f) vrací token.
	*tokenPtr = getToken(f);
	
	return start(tokenPtr, f);
}

/**
	represents START non-terminal
*/
bool start(t_token **tokenPtr, FILE *f) {
	bool start = false;
	token_type type = (*tokenPtr)->type;
	token_type keyW = (*tokenPtr)->data.str;
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
		default:		break;						
	}
	return start;
}

/**
	represents STAT non-terminal
*/
bool stat(t_token **tokenPtr, FILE *f) {
	bool stat = false;
	token_type type = (*tokenPtr)->type;
	token_type keyW = (*tokenPtr)->data.str;
	switch (type) {
		case TOK_ID:	//rule #10 STAT -> id ASS-OR-FUN
						*tokenPtr = getToken(f); //cover id
						start = assorfun(tokenPtr, f);
						break;
		case TOK_KEY:	if (strcmp(keyW, "if") == 0) {
							//rule #11 STAT -> if expr then eol ST-LIST else eol ST-LIST end
							*tokenPtr = getToken(f); //cover if
							stat = expr(tokenPtr, f) && then(tokenPtr, f) && eol(tokenPtr, f) && stlist(tokenPtr, f) &&
								   ntelse(tokenPtr, f) && eol(tokenPtr, f) && stlist(tokenPtr, f) && end(tokenPtr, f);
						} else if (strcmp(keyW, "while") == 0) {
							//rule #12 STAT -> while expr do eol ST-LIST end
							*tokenPtr = getToken(f); //cover while
							stat = expr(tokenPtr, f) && tdo(tokenPtr, f) && eol(tokenPtr, f) && stlist(tokenPtr, f) && end(tokenPtr, f);
						}
						break;
		case TOK_EOL:	//rule #13 STAT -> eps
						return true;
		default:		break;						
	}
	return stat;
}

/**
	represents FUN-DEF non-terminal
*/
bool fundef(t_token **tokenPtr, FILE *f) {
	bool fundef = false;
	token_type type = (*tokenPtr)->type;
	token_type keyW = (*tokenPtr)->data.str;
	
	if (type == TOK_KEY) {
		if (strcmp(keyW, "def") == 0) {
			//rule #6 FUN-DEF -> def id ( P-LIST ) eol ST-LIST end
			*tokenPtr = getToken(f); //cover def
			fundef = id(tokenPtr, f) && lbr(tokenPtr, f) && plist(tokenPtr, f) && rbr(tokenPtr, f) &&
					 eol(tokenPtr, f) && stlist(tokenPtr, f) && end(tokenPtr, f);
		} 
	}
	return fundef;
}

/**
	represents ST-LIST non-terminal
*/
bool stlist(t_token **tokenPtr, FILE *f) {
	bool stlist = false;
	token_type type = (*tokenPtr)->type;
	token_type keyW = (*tokenPtr)->data.str;
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
		default:		break;						
	}
	return stlist;
}

/** end of parser.c