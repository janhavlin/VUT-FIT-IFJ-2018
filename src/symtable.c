#include "symtable.h"

void symTableInit(NodePtr *t){
    *t = NULL;
}

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
        return NULL; //dořešit co vlastně se bude vracet
    }
}

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
int main(){
    NodePtr* t;
    symTableInit(t);

}