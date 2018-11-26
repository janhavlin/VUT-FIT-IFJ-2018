/*
 * file name:       code_gen.c
 * project:         VUT-FIT-IFJ-2018
 * created:         23.11.2018
 * last modified:   23.11.2018
 * 
 * created by:      Jan Havlín xhavli47@stud.fit.vutbr.cz
 * modification:    
 * 
 * description: Functions for converting specific block of code into three address code. 
 */

#include <stdio.h>
#include <stdarg.h>
#include "code_gen.h"
#include "ifj_error.h"
#include "type_conv.h"

static tDLList L;
static int funPara;
int errflg;

void print_elements_of_list(tDLList TL)	{
	
	tDLList TempList=TL;
	int CurrListLength = 0;
	printf("-----------------");
	while ((TempList.First!=NULL))	{
		printf("\n \t%s",TempList.First->inst);
		if ((TempList.First==TL.Act) && (TL.Act!=NULL))
			printf("\t <= toto je aktivní prvek ");
		TempList.First=TempList.First->rptr;
		CurrListLength++;
	}
	printf("\n-----------------\n");     
}

void DLInitList (tDLList *L){
    L->First = NULL;
	L->Last = NULL;
	L->Act = NULL;
}

void DLDisposeList (tDLList *L){
	if (L->First == NULL)
		return;
	
	tDLElemPtr tmp = L->First;
	while(tmp->rptr != NULL)
	{
		tmp = tmp->rptr;
        free(tmp->lptr->inst);
		free(tmp->lptr);
	}
	
    free(tmp->inst);
	free(tmp);
	L->First = NULL;
	L->Last = NULL;
	L->Act = NULL;
}

void DLInsertLast(tDLList *L, char *inst, bool inWhile){
	tDLElemPtr new = (tDLElemPtr)malloc(sizeof(struct tDLElem));
	if (new == NULL){
		errflg = ERR_RUNTIME;
		return;
	}
	
	new->inst = inst;
    new->inWhile = inWhile;
	if (L->Last == NULL)
	{	
		new->lptr = NULL;
		new->rptr = NULL;
		L->First = new;
		L->Last = new;
	}
	else
	{	
		new->rptr = NULL;
		new->lptr = L->Last;
		L->Last->rptr = new;
		L->Last = new;	
	}
}

void DLItemShiftLeft(tDLList *L, tDLElemPtr item){
    if (item->lptr == NULL)
        return;
    
    tDLElemPtr item_left = item->lptr;

    item_left->rptr = item->rptr;

    if (item_left->lptr)
        item_left->lptr->rptr = item;

    item->lptr = item_left->lptr;
    item_left->lptr = item;
    if (item->rptr)
        item->rptr->lptr = item_left;

    item->rptr = item_left;

    if (L->First == item_left)
        L->First = item;
    if (L->Last == item)
        L->Last = item_left;
}


char *getInst(int n, ...){
    va_list vl, vs;
    va_start(vl, n);
    va_start(vs, n);
    int len = 0;
    for (int i = 0; i < n; i++){
        len += strlen(va_arg(vl, char*));
    }

    char *p = calloc(len+1, sizeof(char));
    if (p == NULL)
        errflg = ERR_RUNTIME;

    for (int i = 0; i < n; i++){
        strcat(p, va_arg(vs, char*));
    }

    va_end(vl);
    va_end(vs);
    return p;
}
void genFunCallBegin(bool inWhile){
    char *inst;
    funPara = 0;
    inst = getInst(1, "CREATE FRAME");
    DLInsertLast(&L, inst, inWhile);
}

void genFunCallPar(char *var, bool inWhile){
    char *inst;
    char *conv = convIntToStr(++funPara);
    inst = getInst(2, "DEFVAR TF@\x25", conv);
    DLInsertLast(&L, inst, inWhile);

    inst = getInst(4, "MOVE TF@\x25", conv, " LF@\x25", var);
    DLInsertLast(&L, inst, inWhile);
    free(conv);
}

void genFunCallEnd(char *fun, bool inWhile){
    char *inst;
    inst = getInst(1, "PUSHFRAME");
    DLInsertLast(&L, inst, inWhile);
    inst = getInst(2, "CALL $", fun);
    DLInsertLast(&L, inst, inWhile);
}

void genDefVar(char *var, bool inWhile){
    char *inst;
    inst = getInst(2, "DEFVAR LF@\x25", var);
    DLInsertLast(&L, inst, inWhile);
    inst = getInst(3, "MOVE LF@\x25", var, " nil@nil");
    DLInsertLast(&L, inst, inWhile);
}

int main(){
    genDefVar("a", false);
    print_elements_of_list(L);

    genFunCallBegin(false);
    genFunCallPar("aaa", false);
    for(int i = 0; i < 1; i++)
        genFunCallPar("ddd", false);
    genFunCallEnd("myFunc", false);
    print_elements_of_list(L);
    DLDisposeList(&L);
    return 0;
}
