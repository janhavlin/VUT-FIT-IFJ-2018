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
			printf("\t <= toto je aktivní prvek\n");
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

void printAllInst(tDLList L){
    tDLList tmpList = L;
	while (tmpList.First != NULL){
		printf("%s", tmpList.First->inst);
		tmpList.First=tmpList.First->rptr;
	}      
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
    char *labelPrefix = getInst(4, "$psa", psaStr, "E", resStr);
    inst = getInst(9,  "DEFVAR ", E3, "\n",
                       "DEFVAR ", E1type, "\n",
                       "DEFVAR ", E2type, "\n");
    DLPostInsert(&L, inst);

    //               |--OPERATION----|--ADR1-------------------------|--ADR2-----|--ADR3-------------|                    
    inst = getInst(5, "TYPE ",        E1type, " ",                    E1,"\n");                       DLInsertLast(&L, inst, inWhile);
    inst = getInst(5, "TYPE ",        E2type, " ",                    E2, "\n");                      DLInsertLast(&L, inst, inWhile);
                    
    inst = getInst(7, "JUMPIFEQ ",    labelPrefix, "typeeq ",         E1type,     " ", E2type, "\n"); DLInsertLast(&L, inst, inWhile);
    inst = getInst(5, "JUMPIFEQ ",    labelPrefix, "floatfirst ",     E1type,     " string@float\n"); DLInsertLast(&L, inst, inWhile);
    inst = getInst(5, "JUMPIFEQ ",    labelPrefix, "floatsecond ",    E2type,     " string@float\n"); DLInsertLast(&L, inst, inWhile);

    inst = getInst(3, "LABEL ",       labelPrefix, "exit\n");                                         DLInsertLast(&L, inst, inWhile);
    inst = getInst(1, "EXIT "         "int@4\n");                                                     DLInsertLast(&L, inst, inWhile);

    inst = getInst(3, "LABEL ",       labelPrefix, "floatfirst\n");                                   DLInsertLast(&L, inst, inWhile);
    inst = getInst(5, "JUMPIFNEQ ",   labelPrefix, "exit ",           E2type,     " string@int\n");   DLInsertLast(&L, inst, inWhile);
    inst = getInst(5, "INT2FLOAT ",   E2, " ",                        E2, "\n");                      DLInsertLast(&L, inst, inWhile);
    inst = getInst(3, "JUMP ",        labelPrefix, "add\n");                                          DLInsertLast(&L, inst, inWhile);

    inst = getInst(3, "LABEL ",       labelPrefix, "floatsecond\n");                                  DLInsertLast(&L, inst, inWhile);
    inst = getInst(5, "JUMPIFNEQ ",   labelPrefix, "exit ",           E1type,     " string@int\n");   DLInsertLast(&L, inst, inWhile);
    inst = getInst(5, "INT2FLOAT ",   E1, " ",                        E1, "\n");                      DLInsertLast(&L, inst, inWhile);
    inst = getInst(3, "JUMP ",        labelPrefix, "add\n");                                          DLInsertLast(&L, inst, inWhile);

    inst = getInst(3, "LABEL ",       labelPrefix, "typeeq\n");                                       DLInsertLast(&L, inst, inWhile);
    inst = getInst(5, "JUMPIFEQ ",    labelPrefix, "add ",            E1type,     " string@int\n");   DLInsertLast(&L, inst, inWhile);
    inst = getInst(5, "JUMPIFEQ ",    labelPrefix, "add ",            E1type,     " string@float\n"); DLInsertLast(&L, inst, inWhile);
    inst = getInst(5, "JUMPIFEQ ",    labelPrefix, "cat ",            E1type,     " string@string\n");DLInsertLast(&L, inst, inWhile);
    inst = getInst(1, "EXIT "         "int@4\n");                                                     DLInsertLast(&L, inst, inWhile);

    inst = getInst(3, "LABEL ",       labelPrefix, "cat\n");                                          DLInsertLast(&L, inst, inWhile);
    inst = getInst(7, "CONCAT ",      E3, " ",                        E1, " ",    E2, "\n");          DLInsertLast(&L, inst, inWhile);
    inst = getInst(3, "JUMP ",        labelPrefix, "end\n");                                          DLInsertLast(&L, inst, inWhile);

    inst = getInst(3, "LABEL ",       labelPrefix, "add\n");                                          DLInsertLast(&L, inst, inWhile);
    inst = getInst(7, "ADD ",         E3, " ",                        E1, " ",    E2, "\n");          DLInsertLast(&L, inst, inWhile);

    inst = getInst(3, "LABEL ",       labelPrefix, "end\n");                                          DLInsertLast(&L, inst, inWhile);

    //DLInsertLast(&L, inst, inWhile);
    free(psaStr);
    free(resStr);
    free(var1Str);
    free(var2Str);
    free(E1);
    free(E2);
    free(E3);
    free(E1type);
    free(E2type);
    free(labelPrefix);
}

void genDefVar(char *var, bool inWhile){
    char *inst;
    inst = getInst(3, "DEFVAR LF@&\n", var, "\n");
    DLInsertLast(&L, inst, inWhile);
    inst = getInst(3, "MOVE LF@&", var, " nil@nil\n");
    DLInsertLast(&L, inst, inWhile);
}

int main(){
    DLInitList(&L);
    char *inst;
    inst = getInst(1, ".IFJcode18\n");
    DLInsertLast(&L, inst, false);
    /*genDefVar("a", false);
    print_elements_of_list(L);

    genFunCallBegin(false);
    genFunCallPar("aaa", false);
    genFunCallPar("bbb", false);
    genFunCallPar("ccc", false);
    //for(int i = 0; i < 3; i++)
    //    genFunCallPar("ddd", false);*/
    //genFunCallEnd("myFunc", false);

    genAdd(42, 3, 1, 2, false);
    //genE(5,3,"i", false);
    //genE(5,3,"j", true);
    //genE(5,3,"k", true);
    //genE(5,3,"l", true);
    //print_elements_of_list(L);
    printAllInst(L);
    DLDisposeList(&L);
    return 0;
}
