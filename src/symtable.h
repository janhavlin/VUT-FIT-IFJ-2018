#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<string.h>
#include<ctype.h>

#define TYPE_INT 0
#define TYPE_FLOAT 1
#define TYPE_STRING 2
#define TYPE_FUNCTION 3
#define TYPE_KEYWORD 4

/*typedef enum {
	TYPE_INT = 0
	TYPE_FLOAT = 1
	TYPE_STRING = 2
	TYPE_FUCNTION = 3
	TYPE_KEYWORD = 4
} types;*/

typedef char* string;

typedef union {
	int i;
	double f;
	string s;
} TsymType;

typedef struct TsymData {
	int type;
	TsymType value;
} *TsymDataPtr;

typedef struct TNode {
	string key;
	TsymDataPtr data;
	struct TNode *lPtr;
	struct TNode *rPtr;
} *NodePtr;

/*typedef struct {
	struct symItem *first;
} TsymTable;*/

void symTableInit(NodePtr *);
int symTableInsert(NodePtr *, string, TsymDataPtr);
TsymDataPtr symTableSearch(NodePtr, string);
void symTableFree(NodePtr *t);