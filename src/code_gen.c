/*
 * file name:       code_gen.c
 * project:         VUT-FIT-IFJ-2018
 * created:         23.11.2018
 * last modified:   30.11.2018
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

void DLLast (tDLList *L) {
	L->Act = L->Last;
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

    if (!inWhile){
        DLLast(L);
    }
}

void DLPostInsert (tDLList *L, char *inst) {
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
void genFunCallBegin(bool inWhile){
    char *inst;
    funPara = 0;
    inst = getInst(1, "CREATE FRAME");
    DLInsertLast(&L, inst, inWhile);
}

void genFunCallPar(char *var, bool inWhile){
    char *inst;
    char *conv = convIntToStr(++funPara);
    inst = getInst(2, "DEFVAR TF@&", conv);
    DLPostInsert(&L, inst);

    inst = getInst(4, "MOVE TF@&", conv, " LF@&", var);
    DLInsertLast(&L, inst, inWhile);
    free(conv);
}

void genFunCallEnd(char *fun, bool inWhile){
    char *inst;
    inst = getInst(1, "PUSHFRAME");
    DLInsertLast(&L, inst, inWhile);
    inst = getInst(2, "CALL func", fun);
    DLInsertLast(&L, inst, inWhile);
}

void genWhileBegin(unsigned whileCnt, bool inWhile){
    char *inst;
    char *whileStr = convIntToStr(whileCnt);
    /*inst = getinst(3, "DEFVAR LF@&while", whileStr, "cond\n");
    DLInsertLast(&L, inst); // TODO: Insert it after/before active*/
    inst = getInst(3, "$while", whileStr, "begin:\n");
    DLInsertLast(&L, inst, inWhile);
    free(whileStr);
}

void genWhileCond(unsigned whileCnt, unsigned psa, unsigned res, bool inWhile){
    char *inst;
    char *whileStr = convIntToStr(whileCnt);
    char *psaStr = convIntToStr(psa);
    char *resStr = convIntToStr(res);
    inst = getInst(7, "JUMPIFEQ $while", whileStr, "end LF@&psa", psaStr, "E", resStr, " bool@false\n");
    DLInsertLast(&L, inst, inWhile);
    free(whileStr);
    free(psaStr);
    free(resStr);
}

void genWhileEnd(unsigned whileCnt, bool inWhile){
    char *inst;
    char *whileStr = convIntToStr(whileCnt);
    inst = getInst(6, "JUMP $while", whileStr, "begin\n",
                      "$while", whileStr, "end:\n"); 
    DLInsertLast(&L, inst, inWhile);
    free(whileStr);
}
void genE(unsigned psa, unsigned res, char *var, bool inWhile){
    char *inst;
    char *psaStr = convIntToStr(psa);
    char *resStr = convIntToStr(res);
    inst = getInst(5, "DEFVAR LF@&psa", psaStr, "E", resStr, "\n");
    DLPostInsert(&L, inst); // TODO: Insert it after/before active
    inst = getInst(7, "MOVE LF@&psa", psaStr, "E", resStr, " LF@", var, "\n");
    DLInsertLast(&L, inst, inWhile);
    free(psaStr);
    free(resStr);
}

void genAdd(unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile){
    char *inst;
    char *psaStr = convIntToStr(psa);
    char *resStr = convIntToStr(res);
    char *var1Str = convIntToStr(var1);
    char *var2Str = convIntToStr(var2);
    char *E1 = getInst(4, "LF@&psa", psaStr, "E", var1Str);  
    char *E2 = getInst(4, "LF@&psa", psaStr, "E", var2Str);  
    char *E3 = getInst(4, "LF@&psa", psaStr, "E", resStr);  
    char *E1type = getInst(5, "LF@&psa", psaStr, "E", var1Str, "type");  
    char *E2type = getInst(5, "LF@&psa", psaStr, "E", var2Str, "type");  

    inst = getInst(9,  "DEFVAR ", E3, "\n",
                       "DEFVAR ", E1type, "\n",
                       "DEFVAR ", E2type, "\n");
    DLPostInsert(&L, inst);

    inst = getInst(5, "TYPE ", E1type, " ", E1,"\n");
    inst = getInst(6, inst, "TYPE ", E2type, " ", E2, "\n");

    inst = getInst(10, inst, "JUMPIFEQ $psa", psaStr, "E", resStr, "typeeq ", E1type, " ", E2type, "\n");
    inst = getInst(8, inst, "JUMPIFEQ $psa", psaStr, "E", resStr, "floatfirst ", E1type, " string@float\n");
    inst = getInst(8, inst, "JUMPIFEQ $psa", psaStr, "E", resStr, "floatsecond ", E2type, " string@float\n");

    inst = getInst(6, inst, "$psa", psaStr, "E", resStr, "exit:\n");
    inst = getInst(2, inst, "EXIT int@4\n");

    inst = getInst(6, inst, "$psa", psaStr, "E", resStr, "floatfirst:\n");
    inst = getInst(10, inst, "JUMPIFNEQ $psa", psaStr, "E", resStr, "exit LF@&psa", psaStr, "E", var2Str, "&type string@int\n");
    inst = getInst(6, inst, "INT2FLOAT LF@&psa", psaStr, "E", var2Str, "\n");
    inst = getInst(6, inst, "JUMP $psa", psaStr, "E", resStr, "add\n");

    inst = getInst(6, inst, "$psa", psaStr, "E", resStr, "floatsecond:\n");
    inst = getInst(10, inst, "JUMPIFNEQ $psa", psaStr, "E", resStr, "exit LF@&psa", psaStr, "E", var1Str, "&type string@int\n");
    inst = getInst(6, inst, "INT2FLOAT LF@&psa", psaStr, "E", var1Str, "\n");
    inst = getInst(6, inst, "JUMP $psa", psaStr, "E", resStr, "add\n");

    inst = getInst(6, inst, "$psa", psaStr, "E", resStr, "typeeq:\n");
    inst = getInst(10, inst, "JUMPIFEQ $psa", psaStr, "E", resStr, "add LF@&psa", psaStr, "E", var1Str, "type string@int\n");
    inst = getInst(10, inst, "JUMPIFEQ $psa", psaStr, "E", resStr, "add LF@&psa", psaStr, "E", var1Str, "type string@float\n");
    inst = getInst(10, inst, "JUMPIFEQ $psa", psaStr, "E", resStr, "cat LF@&psa", psaStr, "E", var1Str, "type string@string\n");
    inst = getInst(2, inst, "EXIT int@4\n");

    inst = getInst(14, inst, "CONCAT LF@&psa", psaStr, "E", resStr, " LF@&psa", psaStr, "E", var1Str, " LF@&psa", psaStr, "E", var2Str, "\n");
    inst = getInst(6, inst, "JUMP psa", psaStr, "E", resStr, "end\n");

    inst = getInst(6, inst, "$psa", psaStr, "E", resStr, "add:\n");
    inst = getInst(14, inst, "ADD LF@&psa", psaStr, "E", resStr, " LF@&psa", psaStr, "E", var1Str, " LF@&psa", psaStr, "E", var2Str, "\n");

    inst = getInst(6, inst, "$psa", psaStr, "E", resStr, "end:\n");



    //inst = getInst(6, "ADD LF@&", resStr, " LF@&", var1Str, " LF@&", var2Str);
    DLInsertLast(&L, inst, inWhile);
}

void genDefVar(char *var, bool inWhile){
    char *inst;
    inst = getInst(3, "DEFVAR LF@&\n", var, "\n");
    DLInsertLast(&L, inst, inWhile);
    inst = getInst(3, "MOVE LF@&", var, " nil@nil\n");
    DLInsertLast(&L, inst, inWhile);
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

    //genAdd(42, 3, 1, 2);
    genE(5,3,"i", false);
    genE(5,3,"i", true);
    genE(5,3,"i", true);
    genE(5,3,"i", true);
    print_elements_of_list(L);
    DLDisposeList(&L);
    return 0;
}
