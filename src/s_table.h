/**
	file name:		s_table.h
	project:		VUT-FIT-IFJ-2018
	created:		24.11.2018
	last modified:	24.11.2018
	
	created by: 	Jakub Karpíšek xkarpi06@stud.fit.vutbr.cz
	modifications:	
	
	description:	Symbol table implemented by binary tree recursively
*/
#ifndef IFJ18_S_TABLE_H
#define IFJ18_S_TABLE_H

#include<stdlib.h>
#include<stdbool.h>
#include "ifj_error.h"
#include "dyn_arr.h"
#define DEBUG 0

/**
 * EXAMPLE OF SYMBOL TABLE USAGE:
 * 
 * TsymItem *tablePtr = NULL;
	TsymItem **tablePP = &tablePtr;
	symTabInit(tablePP);
	TsymData *newData = (TsymData *) malloc(sizeof(TsymData));
	if(newData != NULL){
		newData->type = TYPE_KWD;
		newData->defined = false;
		newData->value.i = 0;

		//INITIALIZATION OF KEYWORDS

		for (int i = 0; i < KWD_AMMOUNT; i++)
			symTabInsert(tablePP, kwds[i], newData);
				
		//END OF INITIALIZATION

		printf("Tree:\n");
		symTabToString(*tablePP, 0);		
		
		free(newData);
		newData = NULL;
	}
	symTabDispose(tablePP);
*/

/**
 * types of items in symbol table
 */ 
typedef enum {
    TYPE_INT,
    TYPE_FLT,
    TYPE_STR,
    TYPE_FUN,
    TYPE_KWD
} TsymType;

/**
 * data structure for item in symbol table
 */ 
typedef struct {
	TsymType type;  
    bool defined;   //only used for functions, false by default
	union Value {
        int i;
        double f;
        string s;
    } value;
} TsymData;

/**
 * Node in symbol table tree
 */ 
typedef struct symItem {
	string key;
	TsymData *data;
	struct symItem *lPtr;
	struct symItem *rPtr;
} TsymItem;

void symTabInit(TsymItem **rootPP);
void fillTabWithKwds (TsymItem **tablePP);
bool symTabSearch(TsymItem *rootPtr, string key, TsymData *data);
void symTabInsert(TsymItem **rootPP, string key, TsymData *data);
void replaceByRightmost (TsymItem *ptrReplaced, TsymItem **rootPP);
void symTabDelete (TsymItem **rootPP, string key);
void symTabDispose(TsymItem **rootPP);
void symTabToString(TsymItem *rootPtr, int depth);

#endif //IFJ18_S_TABLE_H
/** end of s_table.h */