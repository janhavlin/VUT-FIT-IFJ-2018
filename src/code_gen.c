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
/*
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
}*/

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
        //free(tmp->lptr->inst);
		free(tmp->lptr);
	}
	
    //free(tmp->inst);
	free(tmp);
	L->First = NULL;
	L->Last = NULL;
	L->Act = NULL;
}

void DLInsertLast(tDLList *L, TInst inst, bool inWhile){
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

void DLPostInsert (tDLList *L, TInst inst) {
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

void printAdr(TAdr adr){
    switch (adr.type){
        case ADRTYPE_VAR:
            printf("LF@&%s", adr.val.s);
        return;
        case ADRTYPE_INT:
            printf("int@%d", adr.val.i);
        return;
        case ADRTYPE_FLOAT:
            printf("float@%e", adr.val.f);
        return;
        case ADRTYPE_STRING:
            printf("string@%s", adr.val.s);
        return;
        case ADRTYPE_NIL:
            printf("nil@nil");
        return;
        case ADRTYPE_LABEL:
            printf("$%s", adr.val.s);
        return;
    }
}

void printInst(TInst inst){
    switch (inst.op){
        case OP_DEFVAR: printf("DEFVAR "); break;
        case OP_MOVE: printf("MOVE "); break;
        case OP_TYPE: printf("TYPE "); break;
        case OP_JUMPIFEQ: printf("JUMPIFEQ "); break;
        case OP_LABEL: printf("LABEL "); break;
        case OP_EXIT: printf("EXIT "); break;
        case OP_JUMPIFNEQ: printf("JUMPIFNEQ "); break;
        case OP_INT2FLOAT: printf("INT2FLOAT "); break;
        case OP_JUMP: printf("JUMP "); break;
        case OP_CONCAT: printf("CONCAT "); break;
        case OP_ADD: printf("ADD "); break;
        case OP_CREATEFRAME: printf("CREATEFRAME "); break;
        case OP_PUSHFRAME: printf("PUSHFRAME "); break;
        case OP_CALL: printf("CALL "); break;
        break;
    }

    switch (inst.op){
        case OP_DEFVAR:
        case OP_LABEL:
        case OP_EXIT:
        case OP_JUMP:
        case OP_CALL:
            printAdr(inst.adr1);
        return;
        
        case OP_MOVE:
        case OP_TYPE:
        case OP_INT2FLOAT:
            printAdr(inst.adr1);
            printf(" ");
            printAdr(inst.adr2);
        return;
        
        case OP_JUMPIFEQ:
        case OP_JUMPIFNEQ:
        case OP_CONCAT:
        case OP_ADD:
            printAdr(inst.adr1);
            printf(" ");
            printAdr(inst.adr2);
            printf(" ");
            printAdr(inst.adr3);
        return;
        
        default:
        return;
    }
}

void printAllInst(tDLList L){
    tDLList tmpList = L;
	while (tmpList.First != NULL){
        printInst(tmpList.First->inst);
        if ((tmpList.First == tmpList.Act) && (tmpList.Act != NULL))
            printf("\t# <-- Active\n");
        else
            printf("\n");

		tmpList.First=tmpList.First->rptr;
	}      
}

char *getStr(int n, ...){
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

TInst getInst(TOperation op, TAdr adr1, TAdr adr2, TAdr adr3){
    TInst inst;
    inst.op = op;
    switch (inst.op){
        case OP_DEFVAR:
        case OP_LABEL:
        case OP_EXIT:
        case OP_JUMP:
            inst.adr1 = adr1;
        return inst;
        
        case OP_MOVE:
        case OP_TYPE:
        case OP_INT2FLOAT:
            inst.adr1 = adr1;
            inst.adr2 = adr2;
        return inst;
        
        case OP_JUMPIFEQ:
        case OP_JUMPIFNEQ:
        case OP_CONCAT:
        case OP_ADD:
            inst.adr1 = adr1;
            inst.adr2 = adr2;
            inst.adr3 = adr3;
        return inst;

        default:
        return inst;
    }
}
void genFunCallBegin(bool inWhile){
    TInst inst;
    TAdr undef;
    funPara = 0;
    inst = getInst(OP_CREATEFRAME, undef, undef, undef); DLInsertLast(&L, inst, inWhile);
}

void genFunCallPar(char *var, bool inWhile){
    TInst inst;
    TAdr adr1, adr2, undef;
    char *conv = convIntToStr(++funPara);
    inst = getInst(OP_DEFVAR, ADRVAR(adr1, conv), undef, undef); DLPostInsert(&L, inst);
    inst = getInst(OP_MOVE, ADRVAR(adr1, conv), ADRVAR(adr2, var), undef); DLInsertLast(&L, inst, inWhile);
    free(conv);
}

void genFunCallEnd(char *fun, bool inWhile){
    TInst inst;
    TAdr adr1, undef;
    inst = getInst(OP_PUSHFRAME,    undef, undef, undef); DLInsertLast(&L, inst, inWhile);
    inst = getInst(OP_CALL,         ADRLAB(adr1, fun, ""), undef, undef); DLInsertLast(&L, inst, inWhile);
}

void genWhileBegin(unsigned whileCnt, bool inWhile){
    TInst inst;
    TAdr adr1, undef;
    char *whileStr = convIntToStr(whileCnt);
    char *lab = getStr(2, "while", whileStr);
    //inst = getStr(3, "DEFVAR LF@&while", whileStr, "cond\n");DLInsertLast(&L, inst); // TODO: Insert it after/before active
    inst = getInst(OP_LABEL, ADRLAB(adr1, lab, "begin"), undef, undef); DLInsertLast(&L, inst, inWhile);
    free(whileStr);
}

void genWhileCond(unsigned whileCnt, unsigned psa, unsigned res, bool inWhile){
    TInst inst;
    TAdr adr1, adr2, adr3, undef;
    char *whileStr = convIntToStr(whileCnt);
    char *psaStr = convIntToStr(psa);
    char *resStr = convIntToStr(res);
    char *EStr = getStr(4, "psa", psaStr, "E", resStr);
    char *lab = getStr(2, "while", whileStr);

    inst = getInst(OP_JUMPIFEQ, ADRLAB(adr1, lab, "end"), ADRVAR(adr2, EStr), ADRBOOL(adr3, "false")); DLInsertLast(&L, inst, inWhile);
    //inst = getStr(7, "JUMPIFEQ $while", whileStr, "end LF@&psa", psaStr, "E", resStr, " bool@false\n");
    DLInsertLast(&L, inst, inWhile);
    free(whileStr);
    free(psaStr);
    free(resStr);
    free(EStr);
    free(lab);
}

void genWhileEnd(unsigned whileCnt, bool inWhile){
    TInst inst;
    TAdr adr1, undef;
    char *whileStr = convIntToStr(whileCnt);
    char *lab = getStr(2, "while", whileStr);
    inst = getInst(OP_JUMP,  ADRLAB(adr1, lab, "begin"), undef, undef); DLInsertLast(&L, inst, inWhile);
    inst = getInst(OP_LABEL, ADRLAB(adr1, lab, "end"),   undef, undef); DLInsertLast(&L, inst, inWhile);
    free(whileStr);
    free(lab);
}

void genE(unsigned psa, unsigned res, char *var, bool inWhile){
    TInst inst;
    TAdr adr1, adr2, undef;
    char *psaStr = convIntToStr(psa);
    char *resEStr = convIntToStr(res);
    char *resVarStr = getStr(4, "psa", psaStr, "E", resEStr);
    inst = getInst(OP_DEFVAR, ADRVAR(adr1, resVarStr), undef, undef);           DLPostInsert(&L, inst);
    inst = getInst(OP_MOVE, ADRVAR(adr1, resVarStr), ADRVAR(adr2, var), undef); DLInsertLast(&L, inst, inWhile);
    free(psaStr);
    free(resEStr);
    free(resVarStr);
}

void genAdd(unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile){
    TInst inst;
    TAdr adr1, adr2, adr3, undef;
    
    char *psaStr = convIntToStr(psa);
    char *resStr = convIntToStr(res);
    char *var1Str = convIntToStr(var1);
    char *var2Str = convIntToStr(var2);
    char *E1 = getStr(4, "psa", psaStr, "E", var1Str);  
    char *E2 = getStr(4, "psa", psaStr, "E", var2Str);  
    char *E3 = getStr(4, "psa", psaStr, "E", resStr);  
    char *E1type = getStr(5, "psa", psaStr, "E", var1Str, "type");  
    char *E2type = getStr(5, "psa", psaStr, "E", var2Str, "type");  
    char *lab = getStr(4, "psa", psaStr, "E", resStr);

    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E3),                   undef,                  undef);                 DLPostInsert(&L, inst);
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E1type),               undef,                  undef);                 DLPostInsert(&L, inst);
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E2type),               undef,                  undef);                 DLPostInsert(&L, inst);

    inst = getInst(OP_TYPE,         ADRVAR(adr1, E1type),               ADRVAR(adr2, E1),       undef);                 DLInsertLast(&L, inst, inWhile);
    inst = getInst(OP_TYPE,         ADRVAR(adr1, E2type),               ADRVAR(adr2, E2),       undef);                 DLInsertLast(&L, inst, inWhile);
   
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "typeeq"),        ADRVAR(adr2, E1type),   ADRVAR(adr3, E2type));  DLInsertLast(&L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "floatfirst"),    ADRVAR(adr2, E1type),   ADRSTR(adr3, "float")); DLInsertLast(&L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "floatsecond"),   ADRVAR(adr2, E2type),   ADRSTR(adr3, "float")); DLInsertLast(&L, inst, inWhile);

    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "exit"),          undef,                  undef);                 DLInsertLast(&L, inst, inWhile);
    inst = getInst(OP_EXIT,         ADRINT(adr1, 4),                    undef,                  undef);                 DLInsertLast(&L, inst, inWhile);

    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "floatfirst"),    undef,                  undef);                 DLInsertLast(&L, inst, inWhile);
    inst = getInst(OP_JUMPIFNEQ,    ADRLAB(adr1, lab, "exit"),          ADRVAR(adr2, E2type),   ADRSTR(adr3, "int"));   DLInsertLast(&L, inst, inWhile);
    inst = getInst(OP_INT2FLOAT,    ADRVAR(adr1, E2),                   ADRVAR(adr1, E2),       undef);                 DLInsertLast(&L, inst, inWhile);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "add"),           undef,                  undef);                 DLInsertLast(&L, inst, inWhile);

    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "floatsecond"),   undef,                  undef);                 DLInsertLast(&L, inst, inWhile);
    inst = getInst(OP_JUMPIFNEQ,    ADRLAB(adr1, lab, "exit"),          ADRVAR(adr2, E1type),   ADRSTR(adr3, "int"));   DLInsertLast(&L, inst, inWhile);
    inst = getInst(OP_INT2FLOAT,    ADRVAR(adr1, E1),                   ADRVAR(adr1, E1),       undef);                 DLInsertLast(&L, inst, inWhile);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "add"),           undef,                  undef);                 DLInsertLast(&L, inst, inWhile);

    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "typeeq"),        undef,                  undef);                 DLInsertLast(&L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "add"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "int"));   DLInsertLast(&L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "add"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "float")); DLInsertLast(&L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "cat"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "string"));DLInsertLast(&L, inst, inWhile);
    inst = getInst(OP_EXIT,         ADRINT(adr1, 4),                    undef,                  undef);                 DLInsertLast(&L, inst, inWhile);

    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "cat"),           undef,                  undef);                 DLInsertLast(&L, inst, inWhile);
    inst = getInst(OP_CONCAT,       ADRVAR(adr1, E3),                   ADRVAR(adr2, E1),       ADRVAR(adr3, E2));      DLInsertLast(&L, inst, inWhile);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "end"),           undef,                  undef);                 DLInsertLast(&L, inst, inWhile);

    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "add"),           undef,                  undef);                 DLInsertLast(&L, inst, inWhile);
    inst = getInst(OP_ADD,          ADRVAR(adr1, E3),                   ADRVAR(adr2, E1),       ADRVAR(adr3, E2));      DLInsertLast(&L, inst, inWhile);

    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "end"),           undef,                  undef);                 DLInsertLast(&L, inst, inWhile);

    free(psaStr);
    free(resStr);
    free(var1Str);
    free(var2Str);
    free(E1);
    free(E2);
    free(E3);
    free(E1type);
    free(E2type);
    free(lab);
}


void genDefVar(char *var, bool inWhile){
    TInst inst;
    TAdr adr1, adr2, adr3;
    inst = getInst(OP_DEFVAR,   ADRVAR(adr1, var),   adr2,           adr3);   DLInsertLast(&L, inst, inWhile);   
    inst = getInst(OP_MOVE,     ADRVAR(adr1, var),   ADRNIL(adr2),   adr3);   DLInsertLast(&L, inst, inWhile);
    
}
/*
int main(){
    DLInitList(&L);
    //TInst inst;
    /*inst = getStr(1, ".IFJcode18\n");
    DLInsertLast(&L, inst, false);*/

    /*print_elements_of_list(L);*/

    /*genFunCallBegin(false);
    genFunCallPar("aaa", false);
    genFunCallPar("bbb", false);
    genFunCallPar("ccc", false);
    for(int i = 0; i < 3; i++)
        genFunCallPar("ddd", false);
    genFunCallEnd("myFunc", false);

    /*genDefVar("a", false);
    genDefVar("b", false);
    genE(5, 1,"a", false);
    genE(5, 2,"b", false);
    genAdd(5, 3, 1, 2, false);*/

    /*printAllInst(L);
    DLDisposeList(&L);
    return 0;
}*/
