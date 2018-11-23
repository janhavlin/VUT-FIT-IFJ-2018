#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "symtable.h"

void symTableInit(TsymTable *t)
{
    *t = NULL;
}

TsymData symTableSearch(TsymTable *t, string key)
{
    if (t != NULL)
    {
        TsymItem *searchedItem = NULL;
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
            return &((*t)->data));
        }
    } else{
        return NULL;
    }
}

int symTableInsert(TsymTable *t, string key, TsymData data)
{
    if (t != NULL)
    {
        if  ((*t)->key > key) symTableInsert(&(*t)->lPtr, key, data);
		else if((*t)->key < key) symTableInsert(&(*t)->rPtr, key, data);
		else (*t)->data = data;
    }
}
else
{
    TsymItem newItem;
    if ((newItem = malloc(sizeof(struct symItem))) != NULL)
    {
        newItem->key = key;
        newItem->data = data;
        newItem->lPtr = NULL;
        newItem->rPtr = NULL;
        (*t) = newItem;
    }
}

void symTableFree(TsymTable *t)
{
    if (*t != NULL)
    {
        symTableFree(&(*t)->*rPtr);
        symTableFree(&(*t)->*lPtr);
        free(*t)
            *t = NULL;
    }