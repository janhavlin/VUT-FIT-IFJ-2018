/**
	file name:		s_table.c
	project:		VUT-FIT-IFJ-2018
	created:		24.11.2018
	last modified:	01.12.2018
	
	created by: 	Jakub Karpíšek xkarpi06@stud.fit.vutbr.cz
	modifications:	
	
	description:	Symbol table implemented by binary tree recursively
*/
#include "s_table.h"

void symTabInit (TsymItem **rootPP) {
	if (DEBUG) printf("Function: %s\n", __func__);
	*rootPP = NULL;
}	

/**
 * Finds a node with key in symbol table tree
 * 
 * @param data will return pointer to found data
 */ 
TsymData *symTabSearch (TsymItem **rootPP, string key)	{
	if (DEBUG) printf("Function:%s key:%s\n", __func__, key);
	if (*rootPP == NULL)
		return NULL;	
	else if (strcmp((*rootPP)->key, key) == 0) {
		return &((*rootPP)->data);
	} else if (strcmp((*rootPP)->key, key) > 0) {
		return symTabSearch(&((*rootPP)->lPtr), key);
	} else {
		return symTabSearch(&((*rootPP)->rPtr), key);
	}
} 

/**
 * Inserts one node into the symbol table tree
 */ 
TsymData *symTabInsert (TsymItem **rootPP, string key, TsymData data)	{
	if (DEBUG) printf("Function:%s key:%s\n", __func__, key);	
	if (*rootPP == NULL) {	//pointer points to an empty tree
		TsymItem *newItemPtr = (TsymItem *) malloc(sizeof(TsymItem));
		if (newItemPtr != NULL) {
			newItemPtr->key = (string) calloc(strlen(key)+1, sizeof(char));
			if(newItemPtr->key != NULL) {
				strcpy(newItemPtr->key, key);
				newItemPtr->data = data;
				newItemPtr->lPtr = NULL;
				newItemPtr->rPtr = NULL;
				*rootPP = newItemPtr;
				return &(newItemPtr->data);
			} else {	//INTERNAL ALLOCATION ERROR
				free(newItemPtr);
				ifjErrorPrint("ERROR %d in s_table.c in func. symTabInsert: allocation failed!\n", ERR_RUNTIME);
				errflg = ERR_RUNTIME;
				return NULL;
			}
		} else {	//INTERNAL ALLOCATION ERROR
			ifjErrorPrint("ERROR %d in s_table.c in func. symTabInsert: allocation failed!\n", ERR_RUNTIME);
			errflg = ERR_RUNTIME;
			return NULL;
		}
	} else if (strcmp((*rootPP)->key, key) == 0) {
		(*rootPP)->data = data;
		return &((*rootPP)->data);
	} else if (strcmp((*rootPP)->key, key) > 0) {
		return symTabInsert(&((*rootPP)->lPtr), key, data);	//I send pointer to a pointer to a tree
	} else 
		return symTabInsert(&((*rootPP)->rPtr), key, data);
	
}

/**
 * Destroys whole symbol table tree
 * tree will return to its state after initialization
 */ 
void symTabDispose (TsymItem **rootPP) {
	if (*rootPP != NULL) {
		if (DEBUG) printf("Function:%s rootKey:%s\n", __func__, (*rootPP)->key);
		symTabDispose(&((*rootPP)->lPtr));
		symTabDispose(&((*rootPP)->rPtr));
		free((*rootPP)->key);
		if ((*rootPP)->data.LT != NULL)
			symTabDispose(&((*rootPP)->data.LT)); //dispose LT inside data node 
		free(*rootPP);
		*rootPP = NULL;
	}
}

/**
 * number of keywords
 */ 
#define KWD_AMMOUNT 9
/**
 * Fills given symbol table with the IFJ18 keywords
 */ 
void symTabFillKwds (TsymItem **tablePP) {
	// Keywords in the IFJ18 language 
	char *kwds[] = {
		"not", "end", "then", "def", "if", "nil", "while", "do", "else"
	};
	TsymData newData;
	newData.type = TYPE_KWD;
	newData.defined = false;
	newData.LT = NULL;

	for (int i = 0; i < KWD_AMMOUNT; i++)
		symTabInsert(tablePP, kwds[i], newData);
}

/**
 * Fills given symbol table with the IFJ18 predefined functions
 * 
 * these functions don't need LT
 */ 
void symTabFillFuns (TsymItem **tablePP) {
		TsymData newData;
		newData.type = TYPE_FUN;
		newData.defined = true;
		newData.called = false;
		newData.params = 0;
		newData.LT = NULL;
		//functions with 0 parameters
		symTabInsert(tablePP, "inputs", newData);
		symTabInsert(tablePP, "inputi", newData);
		symTabInsert(tablePP, "inputf", newData);
		//functions with 1 parameter
		newData.params = 1;
		symTabInsert(tablePP, "length", newData);
		symTabInsert(tablePP, "chr", newData);
		//functions with 2 parameters
		newData.params = 2;
		symTabInsert(tablePP, "ord", newData);
		//functions with 3 parameters
		newData.params = 3;
		symTabInsert(tablePP, "substr", newData);
		//unlimited parameters 
		newData.params = -1;
		symTabInsert(tablePP, "print", newData); //print must have at least one parameter!!
}

/**
 * Prints the tree structure, call with initial depth of 0
 */
void symTabToString(TsymItem *rootPtr, int depth) {
	if (rootPtr != NULL) {
		TsymData d = rootPtr->data;
		for (int i = 0; i < depth; i++)
			printf(" ");
		switch(d.type) {
			case TYPE_VAR:	printf("key:%s\ttype:%d\torder:%u\n", rootPtr->key, d.type, d.order); break;
			case TYPE_FUN:	printf("key:%s\ttype:%d\tdefined:%d\tcalled:%d\tparams:%d\n", rootPtr->key, d.type, d.defined, d.called, d.params); break;
			case TYPE_KWD:	printf("key:%s\ttype:%d\n", rootPtr->key, d.type); break;
		}
		symTabToString(rootPtr->lPtr, depth+1);
		symTabToString(rootPtr->rPtr, depth+1);
	}
	if (depth == 0) printf("end of tree\n");
}

/** end of s_table.c */
