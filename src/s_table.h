/**
	file name:		s_table.h
	project:		VUT-FIT-IFJ-2018
	created:		24.11.2018
	last modified:	04.12.2018
	
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

/**
 * types of items in symbol table
 */ 
typedef enum {
    TYPE_VAR,
    TYPE_FUN,
    TYPE_KWD
} TsymType;

struct symItem;

#define UNLIMITED_P -1
/**
 * data structure for item in symbol table
 */ 
typedef struct {
	TsymType type;  
	unsigned order;				//VAR ONLY: 1, 2, .. for parameter, 0 for later defined variable
    bool defined;   			//FUN ONLY: false if the function was called but not defined
	bool called;				//FUN ONLY: I only care if defined == false
	int params;					//FUN ONLY: number of parameters (-1 means unlimited)
	struct symItem *LT;			//FUN ONLY: own LT = local symbol table
	//return type?
} TsymData;

/**
 * Node in symbol table tree
 */ 
typedef struct symItem {
	string key;
	TsymData data;
	struct symItem *lPtr;
	struct symItem *rPtr;
} TsymItem;

void symTabInit(TsymItem **rootPP);
TsymData *symTabSearch(TsymItem **rootPP, string key);
TsymData *symTabInsert(TsymItem **rootPP, string key, TsymData data);
void symTabDispose(TsymItem **rootPP);
void symTabFillKwds(TsymItem **tablePP);
void symTabFillFuns(TsymItem **tablePP);
void symTabToString(TsymItem *rootPtr, int depth);

#endif //IFJ18_S_TABLE_H
/** end of s_table.h */
