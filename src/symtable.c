/**
	file name:		symtable.c
	project:		VUT-FIT-IFJ-2018
	created:		15.11.2018
	last modified:	24.11.2018
	
	created by: 	Jan Vavřina xvavri10@stud.fit.vutbr.cz
	modifications:	
	
	description:	Table of symbols realized using a binary tree				
*/
#include "symtable.h"


/**
 * Function used to search for an item in a binary tree
 * @param t is the binary tree
 * @param key is the key we are searching for
 * @return data of the respective node
 */
TsymDataPtr symTableSearch(NodePtr t, string key){
    if (t != NULL)
    {
        NodePtr *searchedItem = NULL;
        if (t->key > key)
        {
            return symTableSearch(t->lPtr, key);
        }
        else if (t->key < key)
        {
            return symTableSearch(t->rPtr, key);
        }
        else
        {
            return t->data;
        }
    } else{
        return NULL;
    }
}
/**
 * Inserts an item into the binary tree
 * @param *t pointer to the binary tree
 * @param key is the key we are searching for
 * @param data are the data we want to include into the node
 */
int symTableInsert(NodePtr *t, string key, TsymDataPtr data)
{
    if (t != NULL){
        if  ((*t)->key > key) symTableInsert(&(*t)->lPtr, key, data);
		else if((*t)->key < key) symTableInsert(&(*t)->rPtr, key, data);
		else (*t)->data = data;
    }else{
        NodePtr newItem;
        if ((newItem = malloc(sizeof(struct TNode))) != NULL)
        {
            newItem->key = memcpy(newItem->key, key, strlen(key)+1);
            newItem->data = data;
            newItem->lPtr = NULL;
            newItem->rPtr = NULL;
            (*t) = newItem;
        }
    }
}

/**
 * Free the memory allocated by the symtable
 * @param *t pointer to the binary tree
 */
void symTableFree(NodePtr *t)
{
    if (t != NULL)
    {
        symTableFree(&(*t)->rPtr);
        symTableFree(&(*t)->lPtr);
        free(&(*t));
        *t = NULL;
    }
}
/**
 * Initialization of the symtable and keywords
 * @param *t pointer to the binary tree
 */
void symTableInit(NodePtr *t){
    *t = NULL;
    for(int i = 0; i < 12; i++) symTableInsert(t,keywords[i],NULL);
}
/* end of symtable.c */