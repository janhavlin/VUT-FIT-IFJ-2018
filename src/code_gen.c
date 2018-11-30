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
		printf("---\n%s",TempList.First->inst);
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

void DLInsertLast(tDLList *L, char *inst){
	tDLElemPtr new = (tDLElemPtr)malloc(sizeof(struct tDLElem));
	if (new == NULL){
		errflg = ERR_RUNTIME;
		return;
	}
	
	new->inst = inst;

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

void DLPostInsert (tDLList *L, char *inst, bool inWhile) {
	if (L->Act == NULL)
		return;
	
	tDLElemPtr new = (tDLElemPtr)malloc(sizeof(struct tDLElem));	
    //TODO: If new == NULL
	
	if (L->Last == L->Act)
		L->Last = new;
	
	new->inst = inst;
	
	new->rptr = L->Act->rptr;
	new->lptr = L->Act;
	
	// pokud neni aktivni prvek posledni, tak staremu nasledujicimu prvku nastavime na levy ukazatel na novy prvek
	if (L->Act->rptr != NULL)
		L->Act->rptr->lptr = new;
	
	L->Act->rptr = new;	
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
void genFunCallBegin(){
    char *inst;
    funPara = 0;
    inst = getInst(1, "CREATE FRAME");
    DLInsertLast(&L, inst);
}

void genFunCallPar(char *var, bool inWhile){
    char *inst;
    char *conv = convIntToStr(++funPara);
    inst = getInst(2, "DEFVAR TF@\x25", conv);
    DLInsertLast(&L, inst);

    inst = getInst(4, "MOVE TF@\x25", conv, " LF@\x25", var);
    DLInsertLast(&L, inst);
    free(conv);
}

void genFunCallEnd(char *fun, bool inWhile){
    char *inst;
    inst = getInst(1, "PUSHFRAME");
    DLInsertLast(&L, inst);
    inst = getInst(2, "CALL $func$", fun);
    DLInsertLast(&L, inst);
}

void genWhile(){
    
}

void genAdd(unsigned psa, unsigned res, unsigned var1, unsigned var2){
    char *inst;
    char *psaStr = convIntToStr(psa);
    char *resStr = convIntToStr(res);
    char *var1Str = convIntToStr(var1);
    char *var2Str = convIntToStr(var2);

    inst = getInst(15,  "DEFVAR LF@\x25psa", psaStr, "\x25""E", resStr, "\n",
                        "DEFVAR LF@\x25psa", psaStr, "\x25""E", var1Str, "\x25type\n",
                        "DEFVAR LF@\x25psa", psaStr, "\x25""E", var2Str, "\x25type\n");
    DLInsertLast(&L, inst); // TODO: Insert it after/before active

                       //TYPE %psaN%e1%type                              %psaN%e1
    inst = getInst(9,  "TYPE LF@\x25psa", psaStr, "\x25""E", var1Str, "\x25type LF@\x25psa", psaStr, "\x25""E", var1Str, "\n");
    inst = getInst(10, inst, "TYPE LF@\x25psa", psaStr, "\x25""E", var2Str, "\x25type LF@\x25psa", psaStr, "\x25""E", var2Str, "\n");
    inst = getInst(14, inst, "JUMPIFEQ $psa", psaStr, "$E", resStr, "$typeeq LF@\x25""psa", psaStr, "\x25""E", var1Str, "\x25""type LF@psa", psaStr, "\x25""E", var2Str, "\x25""type\n");
    inst = getInst(10, inst, "JUMPIFEQ $psa", psaStr, "$E", resStr, "$1float LF@\x25""psa", psaStr, "\x25""E", var1Str, "\x25""type string@float\n");
    inst = getInst(10, inst, "JUMPIFEQ $psa", psaStr, "$E", resStr, "$2float LF@\x25""psa", psaStr, "\x25""E", var2Str, "\x25""type string@float\n");

    inst = getInst(6, inst, "$psa", psaStr, "$E", resStr, "$exit:\n");
    inst = getInst(2, inst, "EXIT int@4");

    inst = getInst(6, inst, "$psa", psaStr, "$E", resStr, "$1float:\n");
    inst = getInst(6, inst, "JUMPIFNEQ LF@\x25""psa", psaStr, "\x25""E", var2Str, "\x25""type string@int\n");
    inst = getInst(6, inst, "INT2FLOAT LF@\x25""psa", psaStr, "\x25""E", var2Str, "\n");
    inst = getInst(6, inst, "JUMP $psa", psaStr, "$E", resStr, "$add\n");

    inst = getInst(6, inst, "$psa", psaStr, "$E", resStr, "$2float:\n");
    inst = getInst(6, inst, "JUMPIFNEQ LF@\x25""psa", psaStr, "\x25""E", var1Str, "\x25""type string@int\n");
    inst = getInst(6, inst, "INT2FLOAT LF@\x25""psa", psaStr, "\x25""E", var1Str, "\n");
    inst = getInst(6, inst, "JUMP $psa", psaStr, "$E", resStr, "$add\n");

    inst = getInst(6, inst, "$psa", psaStr, "$E", resStr, "$typeeq:\n");
    inst = getInst(10, inst, "JUMPIFEQ $psa", psaStr, "$E", resStr, "$add LF@\x25""psa", psaStr, "\x25""E", var1Str, "\x25""type" "string@int\n");
    inst = getInst(10, inst, "JUMPIFEQ $psa", psaStr, "$E", resStr, "$add LF@\x25""psa", psaStr, "\x25""E", var1Str, "\x25""type" "string@float\n");
    inst = getInst(10, inst, "JUMPIFEQ $psa", psaStr, "$E", resStr, "$cat LF@\x25""psa", psaStr, "\x25""E", var1Str, "\x25""type" "string@string\n");
    inst = getInst(2, inst, "EXIT int@4\n");

    inst = getInst(14, inst, "CONCAT LF@\x25""psa", psaStr, "\x25""E", resStr, "LF@\x25""psa", psaStr, "\x25""E", var1Str, "LF@\x25""psa", psaStr, "\x25""E", var2Str, "\n");
    inst = getInst(6, inst, "JUMP $psa", psaStr, "$E", resStr, "$end\n");

    inst = getInst(6, inst, "$psa", psaStr, "$E", resStr, "$add:\n");
    inst = getInst(14, inst, "ADD LF@\x25""psa", psaStr, "\x25""E", resStr, "LF@\x25""psa", psaStr, "\x25""E", var1Str, "LF@\x25""psa", psaStr, "\x25""E", var2Str, "\n");

    inst = getInst(6, inst, "$psa", psaStr, "$E", resStr, "$end:\n");



    //inst = getInst(6, "ADD LF@\x25", resStr, " LF@\x25", var1Str, " LF@\x25", var2Str);
    DLInsertLast(&L, inst);
}

void genDefVar(char *var){
    char *inst;
    inst = getInst(3, "DEFVAR LF@\x25\n", var, "\n");
    DLInsertLast(&L, inst);
    inst = getInst(3, "MOVE LF@\x25", var, " nil@nil\n");
    DLInsertLast(&L, inst);
}

int main(){
    /*genDefVar("a", false);
    print_elements_of_list(L);

    genFunCallBegin(false);
    genFunCallPar("aaa", false);
    genFunCallPar("bbb", false);
    genFunCallPar("ccc", false);
    //for(int i = 0; i < 3; i++)
    //    genFunCallPar("ddd", false);
    genFunCallEnd("myFunc", false);*/

    genAdd(42, 3, 1, 2);
    print_elements_of_list(L);
    DLDisposeList(&L);
    return 0;
}
