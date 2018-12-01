/*
 * file name:       code_gen_list.c
 * project:         VUT-FIT-IFJ-2018
 * created:         1.12.2018
 * last modified:   1.12.2018
 * 
 * created by:      Jan Havlín xhavli47@stud.fit.vutbr.cz 
 * modification:
 * 
 * description: Functions and datatypes for working with a doubly linked list of instructions. 
 */

#include "code_gen_list.h"

void ILInit (TInstrList *L){
    L->First = NULL;
	L->Last = NULL;
	L->Act = NULL;
}

void ILSetActLast (TInstrList *L) {
	L->Act = L->Last;
}

void ILDisposeList (TInstrList *L){
	if (L->First == NULL)
		return;
	
	TILElemPtr tmp = L->First;
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

void ILInsertLast(TInstrList *L, TInst inst, bool inWhile){
	TILElemPtr new = (TILElemPtr)malloc(sizeof(struct TILElem));
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

void ILPostActInsert (TInstrList *L, TInst inst) {
	if (L->Act == NULL)
		return;
	
	TILElemPtr new = (TILElemPtr)malloc(sizeof(struct TILElem));	
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

void printAllInst(TInstrList L){
    TInstrList tmpList = L;
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
