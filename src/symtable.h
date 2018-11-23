#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<string.h>

#define TYPE_INT 0
#define TYPE_FLOAT 1
#define TYPE_STRING 2
#define TYPE_FUNCTION 3
#define TYPE_KEYWORD 4

typedef union {
	int i;
	double f;
	string s;
} TsymType

typedef struct {
	int type;
	TsymType value;
} TsymData;

typedef struct symItem {
	string key;
	TsymData data;
	struct symItem *lPtr;
	struct symItem *rPtr;
} TsymItem;

typedef struct {
	struct symItem *first;
} TsymTable;

void symTableInit(TsymTable *t);
int symTableInsert(TsymTable *t, string key, TsymData data);
Tsym_data *symTableSearch(TsymTable *t, string key);
void symTableFree(TsymTable *t);