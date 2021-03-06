/*
 * file name:       code_gen_list.c
 * project:         VUT-FIT-IFJ-2018
 * created:         01.12.2018
 * last modified:   05.12.2018
 * 
 * created by:      Jan Havlín xhavli47@stud.fit.vutbr.cz 
 * modifications:
 * 
 * description: Functions and datatypes for working with a doubly linked list of instructions. 
 */

#include "code_gen_list.h"

void ILInit (TInstrList *L){
    L->First = NULL;
	L->Last = NULL;
	L->BeforeWhile = NULL;
	L->LabMain = NULL;
    L->InFunDefBeforeWhile = NULL;
}

void ILSetBeforeWhile (TInstrList *L) {
	L->BeforeWhile = L->Last;
}

void ILSetInFunDefBeforeWhile (TInstrList *L) {
	L->InFunDefBeforeWhile = L->LabMain->lptr;
}

void ILSetLabMain (TInstrList *L) {
	L->LabMain = L->First;
}

void disposeAdr(TAdr *adr){
    if (adr->type == ADRTYPE_VAR || adr->type == ADRTYPE_VAR_TMP || adr->type == ADRTYPE_STRING || adr->type == ADRTYPE_LABEL || adr->type == ADRTYPE_BOOL)
        free(adr->val.s);
}

void disposeInstr(TInst *inst){
    switch (inst->op){
        case OP_DEFVAR:
        case OP_LABEL:
        case OP_EXIT:
        case OP_JUMP:
        case OP_CALL:
        case OP_WRITE:
        case OP_READ:
            disposeAdr(&(inst->adr1));
        return;
        
        case OP_MOVE:
        case OP_TYPE:
        case OP_INT2FLOAT:
        case OP_FLOAT2INT:
        case OP_NOT:
            disposeAdr(&(inst->adr1));
            disposeAdr(&(inst->adr2));
        return;
        
        case OP_JUMPIFEQ:
        case OP_JUMPIFNEQ:
        case OP_CONCAT:
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_LT:
        case OP_GT:
        case OP_LEQ:
        case OP_GEQ:
        case OP_EQ:
        case OP_NEQ:
            disposeAdr(&(inst->adr1));
            disposeAdr(&(inst->adr2));
            disposeAdr(&(inst->adr3));
        return;
        
        default:
        return;
    }
}

void ILDisposeList (TInstrList *L){
	if (L->First == NULL)
		return;
	
	TILElemPtr tmp = L->First;
	while(tmp->rptr != NULL)
	{
		tmp = tmp->rptr;
        //free(tmp->lptr->inst);
        disposeInstr(&(tmp->lptr->inst));
		free(tmp->lptr);
	}
	
    disposeInstr(&(tmp->inst));
    //free(tmp->inst);
	free(tmp);
	L->First = NULL;
	L->Last = NULL;
	L->BeforeWhile = NULL;
    L->InFunDefBeforeWhile = NULL;
    L->LabMain = NULL;
}

void ILInsertFirst (TInstrList *L, TInst inst) {
	TILElemPtr new = (TILElemPtr)malloc(sizeof(struct TILElem));
	if (new == NULL){
		errflg = ERR_RUNTIME;
		return;
	}

	new->inst = inst;

	if (L->First == NULL)
	{	
		new->lptr = NULL;
		new->rptr = NULL;
		L->First = new;
		L->Last = new;
	}
	else
	{	
		new->lptr = NULL;
		new->rptr = L->First;
		L->First->lptr = new;
		L->First = new;	
	}
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

    if (!inWhile)
        ILSetBeforeWhile(L);
}

void ILPreInsertLabMain (TInstrList *L, TInst inst, bool inWhile) {
	if (L->LabMain == NULL)
		return;
	
	TILElemPtr new = (TILElemPtr)malloc(sizeof(struct TILElem));	
    if (new == NULL){
        errflg = ERR_RUNTIME;
        return;
    }
	
	if (L->First == L->LabMain)
		L->First = new;
	
	new->inst = inst;
	
	new->lptr = L->LabMain->lptr;
	new->rptr = L->LabMain;

	if (L->LabMain->lptr != NULL)
		L->LabMain->lptr->rptr = new;
	
	L->LabMain->lptr = new;

    if (!inWhile)
        ILSetInFunDefBeforeWhile(L);
}

void ILPostInsertBeforeWhile (TInstrList *L, TInst inst) {
	if (L->BeforeWhile == NULL)
		return;
	
	TILElemPtr new = (TILElemPtr)malloc(sizeof(struct TILElem));	
    if (new == NULL){
        errflg = ERR_RUNTIME;
        return;
    }
	
	if (L->Last == L->BeforeWhile)
		L->Last = new;
	
	new->inst = inst;
	
	new->rptr = L->BeforeWhile->rptr;
	new->lptr = L->BeforeWhile;
	
	if (L->BeforeWhile->rptr != NULL)
		L->BeforeWhile->rptr->lptr = new;
	
	L->BeforeWhile->rptr = new;
    

}

void ILPostInsertInFunDefBeforeWhile (TInstrList *L, TInst inst) {
	if (L->InFunDefBeforeWhile == NULL)
		return;
	
	TILElemPtr new = (TILElemPtr)malloc(sizeof(struct TILElem));	
    if (new == NULL){
        errflg = ERR_RUNTIME;
        return;
    }
	
	if (L->Last == L->InFunDefBeforeWhile)
		L->Last = new;
	
	new->inst = inst;
	
	new->rptr = L->InFunDefBeforeWhile->rptr;
	new->lptr = L->InFunDefBeforeWhile;
	
	if (L->InFunDefBeforeWhile->rptr != NULL)
		L->InFunDefBeforeWhile->rptr->lptr = new;
	
	L->InFunDefBeforeWhile->rptr = new;	
}

static void printAdr(TAdr adr){
    switch (adr.type){
        case ADRTYPE_VAR:
            printf("LF@&%s", adr.val.s);
        return;
        case ADRTYPE_VAR_TMP:
            printf("TF@&%s", adr.val.s);
        return;
        case ADRTYPE_INT:
            printf("int@%d", adr.val.i);
        return;
        case ADRTYPE_FLOAT:
            printf("float@%a", adr.val.f);
        return;
        case ADRTYPE_STRING:
            printf("string@%s", adr.val.s);
        return;
        case ADRTYPE_BOOL:
            printf("bool@%s", adr.val.s);
        return;
        case ADRTYPE_NIL:
            printf("nil@nil");
        return;
        case ADRTYPE_LABEL:
            printf("$%s", adr.val.s);
        return;
    }
}

static void printInst(TInst inst){
    switch (inst.op){
        case OP_DEFVAR: printf("DEFVAR "); break;
        case OP_MOVE: printf("MOVE "); break;
        case OP_TYPE: printf("TYPE "); break;
        case OP_JUMPIFEQ: printf("JUMPIFEQ "); break;
        case OP_LABEL: printf("LABEL "); break;
        case OP_EXIT: printf("EXIT "); break;
        case OP_JUMPIFNEQ: printf("JUMPIFNEQ "); break;
        case OP_INT2FLOAT: printf("INT2FLOAT "); break;
        case OP_FLOAT2INT: printf("FLOAT2INT "); break;
        case OP_JUMP: printf("JUMP "); break;
        case OP_CONCAT: printf("CONCAT "); break;
        case OP_CREATEFRAME: printf("CREATEFRAME "); break;
        case OP_PUSHFRAME: printf("PUSHFRAME "); break;
        case OP_POPFRAME: printf("POPFRAME "); break;
        case OP_RETURN: printf("RETURN "); break;
        case OP_CALL: printf("CALL "); break;
        case OP_ADD: printf("ADD "); break;
        case OP_SUB: printf("SUB "); break;
        case OP_MUL: printf("MUL "); break;
        case OP_DIV: printf("DIV "); break;
        case OP_LT: printf("LT "); break;
        case OP_GT: printf("GT "); break;
        case OP_LEQ: printf("LE "); break;
        case OP_GEQ: printf("GE "); break;
        case OP_EQ: printf("EQ "); break;
        case OP_NEQ: printf("NEQ "); break;
        case OP_NOT: printf("NOT "); break;
        case OP_WRITE: printf("WRITE "); break;
        case OP_READ: printf("READ "); break;
        break;
    }

    switch (inst.op){
        case OP_DEFVAR:
        case OP_LABEL:
        case OP_EXIT:
        case OP_JUMP:
        case OP_CALL:
        case OP_WRITE:
        case OP_READ:
            printAdr(inst.adr1);
        return;
        
        case OP_MOVE:
        case OP_TYPE:
        case OP_INT2FLOAT:
        case OP_FLOAT2INT:
        case OP_NOT:
            printAdr(inst.adr1);
            printf(" ");
            printAdr(inst.adr2);
        return;
        
        case OP_JUMPIFEQ:
        case OP_JUMPIFNEQ:
        case OP_CONCAT:
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_LT:
        case OP_GT:
        case OP_LEQ:
        case OP_GEQ:
        case OP_EQ:
        case OP_NEQ:
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

void ILPrintAllInst(TInstrList L){
    TInstrList tmpList = L;
    printf(".IFJcode18\n");
    printf("JUMP $$main\n");
    printf(FUN_LENGTH);
    printf(FUN_ORD);
    printf(FUN_CHR);
    printf(FUN_SUBSTR);
    printf(FUN_GENPLUSE);
    printf(FUN_GENMINUSE);
	while (tmpList.First != NULL){
        printInst(tmpList.First->inst);
        if ((tmpList.First == tmpList.BeforeWhile) && (tmpList.BeforeWhile != NULL)) printf("\t# <-- Outside while in main");
        if ((tmpList.First == tmpList.InFunDefBeforeWhile) && (tmpList.BeforeWhile != NULL)) printf("\t# <-- Outside while in fun def");
        if ((tmpList.First == tmpList.LabMain) && (tmpList.LabMain != NULL)) printf("\t# <-- Main label");
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
    if (p == NULL){
        errflg = ERR_RUNTIME;
        return NULL;
    }

    for (int i = 0; i < n; i++){
        strcat(p, va_arg(vs, char*));
    }

    va_end(vl);
    va_end(vs);
    return p;
}

char *getIfjCodeStr(char *s){
    int len = 0;
    for (int i = 0; i < strlen(s); i++){
        if ((s[i] >= 0 && s[i] <= 32) || s[i] == 35 || s[i] == 95)
            len += 4;
        else
            len += 1;
    }
    //printf("Len of ifjcode string: %u\n", len);
    char *p = calloc(len+1, sizeof(char));
    if (p == NULL){
        errflg = ERR_RUNTIME;
        return NULL;
    }

    int s_i = 0;
    int p_i = 0;
    while (p_i < len){
        if ((s[s_i] >= 0 && s[s_i] <= 32) || s[s_i] == 35 || s[s_i] == 95){
            p[p_i++] = '\\';
            p[p_i++] = '0';
            p[p_i++] = s[s_i] / 10 + '0';
            p[p_i++] = s[s_i] % 10 + '0';
        }
        else {
            p[p_i++] = s[s_i];
        }
        s_i++;
    }
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
        case OP_CALL:
        case OP_WRITE:
        case OP_READ:
            inst.adr1 = adr1;
        return inst;
        
        case OP_MOVE:
        case OP_TYPE:
        case OP_INT2FLOAT:
        case OP_FLOAT2INT:
        case OP_NOT:
            inst.adr1 = adr1;
            inst.adr2 = adr2;
        return inst;
        
        case OP_JUMPIFEQ:
        case OP_JUMPIFNEQ:
        case OP_CONCAT:
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_LT:
        case OP_GT:
        case OP_LEQ:
        case OP_GEQ:
        case OP_EQ:
        case OP_NEQ:
            inst.adr1 = adr1;
            inst.adr2 = adr2;
            inst.adr3 = adr3;
        return inst;

        default:
        return inst;
    }
}
