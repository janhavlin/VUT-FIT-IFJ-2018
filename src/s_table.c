/**
	file name:		s_table.c
	project:		VUT-FIT-IFJ-2018
	created:		24.11.2018
	last modified:	24.11.2018
	
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
 * number of keywords
 */ 
#define KWD_AMMOUNT 9
/**
 * Fills given symbol table with the IFJ18 keywords
 */ 
void fillTabWithKwds (TsymItem **tablePP) {
	// Keywords in the IFJ18 language 
	char *kwds[] = {
		"not", "end", "then", "def", "if", "nil", "while", "do", "else"
	};
	TsymData *newData = (TsymData *) malloc(sizeof(TsymData));
	if(newData != NULL){
		newData->type = TYPE_KWD;
		newData->defined = false;
		newData->value.i = 0;

		for (int i = 0; i < KWD_AMMOUNT; i++)
			symTabInsert(tablePP, kwds[i], newData);

		free(newData);
		newData = NULL;
	} else {
		errflg = ERR_RUNTIME;
		ifjErrorPrint("ERROR %d in s_table.c in func. symTabInsert: allocation failed!\n", errflg);
	}
}

/**
 * Finds a node with key in symbol table tree
 * 
 * @param data is destination to copy found data to
 */ 
bool symTabSearch (TsymItem *rootPtr, string key, TsymData *data)	{
	if (DEBUG) printf("Function: %s\n", __func__);
	if (rootPtr == NULL)
		return false;	
	else if (strcmp(rootPtr->key, key) == 0) {
		memcpy(data, rootPtr->data, sizeof(TsymData));
		return true;
	} else if (strcmp(rootPtr->key, key) > 0) 
		return symTabSearch(rootPtr->lPtr, key, data);
	else
		return symTabSearch(rootPtr->rPtr, key, data);
} 

/**
 * Inserts one node into the symbol table tree
 */ 
void symTabInsert (TsymItem **rootPP, string key, TsymData *data)	{
	if (DEBUG) printf("Function: %s\n", __func__);	
	if (*rootPP == NULL) {	//pointer points to an empty tree
		TsymItem *newItemPtr = (TsymItem *) malloc(sizeof(TsymItem));
		if (newItemPtr != NULL) {
			newItemPtr->key = (string) calloc(strlen(key)+1, sizeof(char));
			if(newItemPtr->key != NULL) {
				strcpy(newItemPtr->key, key);
				newItemPtr->data = (TsymData *) malloc(sizeof(TsymData));
				if (newItemPtr->data != NULL) {
					memcpy(newItemPtr->data, data, sizeof(TsymData));
					newItemPtr->lPtr = NULL;
					newItemPtr->rPtr = NULL;
					*rootPP = newItemPtr;
				} else {	//INTERNAL ALLOCATION ERROR
					free(newItemPtr->key);
					free(newItemPtr);
					ifjErrorPrint("ERROR %d in s_table.c in func. symTabInsert: allocation failed!\n", ERR_RUNTIME);
					errflg = ERR_RUNTIME;
					return;
				}	
			} else {	//INTERNAL ALLOCATION ERROR
				free(newItemPtr);
				ifjErrorPrint("ERROR %d in s_table.c in func. symTabInsert: allocation failed!\n", ERR_RUNTIME);
				errflg = ERR_RUNTIME;
				return;
			}
		} else {	//INTERNAL ALLOCATION ERROR
			ifjErrorPrint("ERROR %d in s_table.c in func. symTabInsert: allocation failed!\n", ERR_RUNTIME);
			errflg = ERR_RUNTIME;
			return;
		}
	} else if (strcmp((*rootPP)->key, key) == 0) {
		memcpy((*rootPP)->data, data, sizeof(TsymData));
	} else if (strcmp((*rootPP)->key, key) > 0) {
		symTabInsert(&((*rootPP)->lPtr), key, data);	//I send pointer to a pointer to a tree
	} else 
		symTabInsert(&((*rootPP)->rPtr), key, data);
}

/**
 * Helper Function for symDelete
 */ 
void replaceByRightmost (TsymItem *ptrReplaced, TsymItem **rootPP) {
	if (DEBUG) printf("Function: %s\n", __func__);
	if ((*rootPP)->rPtr != NULL)
		//if the root of the subtree has a right child, then go deeper
		replaceByRightmost(ptrReplaced, &((*rootPP)->rPtr));
	else {
		TsymItem *rightMost = *rootPP;
		*rootPP = (rightMost)->lPtr;	//parent node will show directly to left child (can be also NULL)
		strcpy(ptrReplaced->key, rightMost->key);	//shift of the key
		memcpy(ptrReplaced->data, rightMost->data, sizeof(TsymData));	//shift of the content
		free(rightMost->data);
		free(rightMost);
		rightMost = NULL;
	}
}

/**
 * Deletes one node from the tree 
 */ 
void symTabDelete (TsymItem **rootPP, string key) {
	if (DEBUG) printf("Function: %s\n", __func__);
	if (*rootPP != NULL) {
		if (strcmp((*rootPP)->key, key) == 0) {
			TsymItem *toDelete = *rootPP;
			if (toDelete->lPtr == NULL) {
				*rootPP = toDelete->rPtr;
				free(toDelete->key);
				free(toDelete->data);
				free(toDelete);
				toDelete = NULL;
			} else if (toDelete->rPtr == NULL) {
				*rootPP = toDelete->lPtr;
				free(toDelete->key);
				free(toDelete->data);
				free(toDelete);
				toDelete = NULL;
			} else {
				replaceByRightmost(toDelete, &(toDelete->lPtr));	
			}				
		} else if (strcmp((*rootPP)->key, key) > 0)
			symTabDelete(&((*rootPP)->lPtr), key);
		else 
			symTabDelete(&((*rootPP)->rPtr), key);		
	}
} 
/**
 * Destroys whole symbol table tree
 * tree will return to its state after initialization
 */ 
void symTabDispose (TsymItem **rootPP) {
	if (DEBUG) printf("Function: %s\n", __func__);
	if (*rootPP != NULL) {
		symTabDispose(&((*rootPP)->lPtr));
		symTabDispose(&((*rootPP)->rPtr));
		free((*rootPP)->key);
		free((*rootPP)->data);
		free(*rootPP);
		*rootPP = NULL;
	}
}

/**
 * Prints the tree structure, call with initial depth of 0
 */
void symTabToString(TsymItem *rootPtr, int depth) {
	if (DEBUG) printf("Function: %s\n", __func__);
	if (rootPtr != NULL) {
		if (rootPtr->data != NULL) {
			for (int i = 0; i < depth; i++)
				printf(" ");
			switch(rootPtr->data->type) {
				case TYPE_INT:	printf("key:%s\ttype:%d\tdefined:%d\tvalue:%d\n", rootPtr->key, rootPtr->data->type, rootPtr->data->defined, rootPtr->data->value.i); break;
				case TYPE_FLT: printf("key:%s\ttype:%d\tdefined:%d\tvalue:%f\n", rootPtr->key, rootPtr->data->type, rootPtr->data->defined, rootPtr->data->value.f); break;
				case TYPE_STR: printf("key:%s\ttype:%d\tdefined:%d\tvalue:%s\n", rootPtr->key, rootPtr->data->type, rootPtr->data->defined, rootPtr->data->value.s); break;
				default:	printf("key:%s\ttype:%d\tdefined:%d\n", rootPtr->key, rootPtr->data->type, rootPtr->data->defined); break;
			}
		}
		symTabToString(rootPtr->lPtr, depth+1);
		symTabToString(rootPtr->rPtr, depth+1);
	}
}

/** end of s_table.c */