/**
	file name:		symtable.h
	project:		VUT-FIT-IFJ-2018
	created:		15.11.2018
	last modified:	24.11.2018
	
	created by: 	Jan Vavřina xvavri10@stud.fit.vutbr.cz
	modifications:	
	
	description:	Header file for the symtable			
*/
#ifndef SYMTABLE
#define SYMTABLE

#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<string.h>
#include<ctype.h>

typedef char* string; //tmp - testing only

/**
 * Definition of used keywords
 */
char keywords[12][7] = {
	"def", 
	"do", 
	"else", 
	"end", 
	"if", 
	"not", 
	"nil", 
	"then", 
	"while", 
	"Integer", 
	"Float", 
	"String"
};

/** An enum type
 * Enumerate for token types
 */
typedef enum {
    TYPE_INT,
    TYPE_FLT,
    TYPE_STR,
    TYPE_FUN,
    TYPE_KWD
} TsymType;

typedef union {
	int i;
	double f;
	string s;
} TsymValue;

typedef struct TsymData {
	int type;
	TsymValue value;
} *TsymDataPtr;

typedef struct TNode {
	string key;
	TsymDataPtr data;
	struct TNode *lPtr;
	struct TNode *rPtr;
} *NodePtr;

void symTableInit(NodePtr *);
int symTableInsert(NodePtr *, string, TsymDataPtr);
TsymDataPtr symTableSearch(NodePtr, string);
void symTableFree(NodePtr *t);

#endif //SYMTABLE
/* end of symtable.h */