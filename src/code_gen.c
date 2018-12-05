/*
 * file name:       code_gen.c
 * project:         VUT-FIT-IFJ-2018
 * created:         23.11.2018
 * last modified:   4.12.2018
 * 
 * created by:      Jan Havlín xhavli47@stud.fit.vutbr.cz
 * modification:    
 * 
 * description: Functions for transforming pieces of IFJ18 code into instruction structures and adding them to a doubly linked list of instructions. 
 */

#include "code_gen.h"

//static TInstrList L;
static int funPara;
int errflg;

void genPrgBegin(TInstrList *L){
    TInst inst;
    TAdr adr1, adr2, undef;
    inst = getInst(OP_LABEL, ADRLAB(adr1, "$main", ""), undef, undef); ILInsertLast(L, inst, false);
    inst = getInst(OP_CREATEFRAME, undef, undef, undef); ILInsertLast(L, inst, false);
    inst = getInst(OP_PUSHFRAME, undef, undef, undef); ILInsertLast(L, inst, false);
    inst = getInst(OP_DEFVAR, ADRVAR(adr1, "RETVAL"), undef, undef); ILInsertLast(L, inst, false);
    inst = getInst(OP_MOVE, ADRVAR(adr1, "RETVAL"), ADRNIL(adr2), undef); ILInsertLast(L, inst, false);
    //ILSetBeforeWhile(L);
    ILSetLabMain(L);
}

void genFunDefBegin(TInstrList *L, char *fun, bool inWhile, bool inFunDef){
    TInst inst;
    TAdr adr1, undef;
    inst = getInst(OP_LABEL, ADRLAB(adr1, fun,""), undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
}

void genFunDefEnd(TInstrList *L, bool inWhile, bool inFunDef){
    TInst inst;
    TAdr undef;
    inst = getInst(OP_RETURN, undef, undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
}

void genFunCallBegin(TInstrList *L, char *fun, bool inWhile, bool inFunDef){
    if (strcmp(fun, "print") == 0 ||
        strcmp(fun, "inputs") == 0 ||
        strcmp(fun, "inputi") == 0 ||
        strcmp(fun, "inputf") == 0){
        return;
    }

    TInst inst;
    TAdr undef;
    funPara = 0;
    inst = getInst(OP_CREATEFRAME, undef, undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
}

void genFunCallPar(TInstrList *L, char *fun, unsigned parNum, TAdr srcVar, unsigned srcVarParOrder, bool inWhile, bool inFunDef){
    TInst inst;
    TAdr adr1, adr2, undef;
    char *funParStr = convIntToStr(srcVarParOrder);
    if (strcmp(fun, "print") == 0){
        switch (srcVar.type){
            case ADRTYPE_VAR:
                if (srcVarParOrder > 0)
                    inst = getInst(OP_WRITE, ADRVAR(adr2, funParStr), undef, undef);
                else
                    inst = getInst(OP_WRITE, ADRVAR(adr2, srcVar.val.s), undef, undef);
                break;
            case ADRTYPE_INT:    inst = getInst(OP_WRITE, ADRINT(adr2, srcVar.val.i), undef, undef); break;
            case ADRTYPE_FLOAT:  inst = getInst(OP_WRITE, ADRFLT(adr2, srcVar.val.f), undef, undef); break;
            case ADRTYPE_STRING: inst = getInst(OP_WRITE, ADRSTR(adr2, srcVar.val.s), undef, undef); break;
            case ADRTYPE_NIL:    inst = getInst(OP_WRITE, ADRNIL(adr2),            undef, undef); break;
        }
        INSERTLAST(L, inst, inWhile, inFunDef);
    }
    // This shouldn't be needed...
    else if (strcmp(fun, "inputs") == 0 ||
             strcmp(fun, "inputi") == 0 ||
             strcmp(fun, "inputf") == 0){
        return;
    }
    else {
        char *conv = convIntToStr(parNum);
        inst = getInst(OP_DEFVAR,   ADRVARTMP(adr1, conv), undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
        switch (srcVar.type){
            //TODO funpar...
            case ADRTYPE_VAR:
                if (srcVarParOrder > 0)
                    inst = getInst(OP_MOVE, ADRVARTMP(adr1, conv), ADRVAR(adr2, funParStr), undef);
                else
                    inst = getInst(OP_MOVE, ADRVARTMP(adr1, conv), ADRVAR(adr2, srcVar.val.s), undef);
                break;

            case ADRTYPE_INT:    inst = getInst(OP_MOVE, ADRVARTMP(adr1, conv), ADRINT(adr2, srcVar.val.i), undef); break;
            case ADRTYPE_FLOAT:  inst = getInst(OP_MOVE, ADRVARTMP(adr1, conv), ADRFLT(adr2, srcVar.val.f), undef); break;
            case ADRTYPE_STRING: inst = getInst(OP_MOVE, ADRVARTMP(adr1, conv), ADRSTR(adr2, srcVar.val.s), undef); break;
            case ADRTYPE_NIL:    inst = getInst(OP_MOVE, ADRVARTMP(adr1, conv), ADRNIL(adr2),            undef); break;
        }
        INSERTLAST(L, inst, inWhile, inFunDef);
        free(conv);
    }
    free(funParStr);
}

void genFunCallEnd(TInstrList *L, char *fun, bool inWhile, bool inFunDef){
    TInst inst;
    TAdr adr1, adr2, undef;

    if (strcmp(fun, "print") == 0){
        inst = getInst(OP_MOVE, ADRVAR(adr1, "RETVAL"), ADRNIL(adr2), undef);
    }
    else if (strcmp(fun, "inputs") == 0){
        inst = getInst(OP_READ, ADRVAR(adr1, "RETVAL string"), undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
    }
    else if (strcmp(fun, "inputi") == 0){
        inst = getInst(OP_READ, ADRVAR(adr1, "RETVAL int"), undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
    }
    else if (strcmp(fun, "inputf") == 0){
        inst = getInst(OP_READ, ADRVAR(adr1, "RETVAL float"), undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
    }
    else {
    inst = getInst(OP_DEFVAR,       ADRVARTMP(adr1, "RETVAL"),  undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_MOVE,         ADRVARTMP(adr1, "RETVAL"),  ADRNIL(adr2), undef); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_PUSHFRAME,    undef,                      undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_CALL,         ADRLAB(adr1, fun, ""),      undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_POPFRAME,     undef,                      undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_MOVE,         ADRVAR(adr1, "RETVAL"),     ADRVARTMP(adr2, "RETVAL"), undef); INSERTLAST(L, inst, inWhile, inFunDef);
    }

}

void genWhileBegin(TInstrList *L, unsigned whileCnt, bool inWhile, bool inFunDef){
    TInst inst;
    TAdr adr1, undef;
    char *whileStr = convIntToStr(whileCnt);
    char *lab = getStr(2, "while", whileStr);
    inst = getInst(OP_LABEL, ADRLAB(adr1, lab, "begin"), undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
    free(whileStr);
    free(lab);
}

void genWhileCond(TInstrList *L, unsigned whileCnt, unsigned psa, unsigned psaResultE, bool inWhile, bool inFunDef){
    TInst inst;
    TAdr adr1, adr2, adr3, undef;
    char *whileStr = convIntToStr(whileCnt);
    char *psaStr = convIntToStr(psa);
    char *resStr = convIntToStr(psaResultE);
    char *EStr = getStr(4, "PSA", psaStr, "E", resStr);
    char *lab = getStr(2, "while", whileStr);
    char *condStr = getStr(3, "while", whileStr, "condtype");

    // Result of the PSA must be bool so we'll check that first
    /*inst = getInst(OP_DEFVAR,   ADRVAR(adr1, condStr),      undef,                  undef);                 INSERTBEFOREWHILE(L, inst, inWhile, inFunDef);
    inst = getInst(OP_TYPE,     ADRVAR(adr1, condStr),      ADRVAR(adr2, EStr),     undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ, ADRLAB(adr1, lab, "cond"),  ADRVAR(adr2, condStr),  ADRSTR(adr3, "bool"));  INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_EXIT,     ADRINT(adr1, 4),            undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_LABEL,    ADRLAB(adr1, lab, "cond"),  undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);*/

    inst = getInst(OP_DEFVAR,   ADRVAR(adr1, condStr),      undef,                  undef);                 INSERTBEFOREWHILE(L, inst, inWhile, inFunDef);
    inst = getInst(OP_TYPE,     ADRVAR(adr1, condStr),      ADRVAR(adr2, EStr),     undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ, ADRLAB(adr1, lab, "cond"),  ADRVAR(adr2, condStr),  ADRSTR(adr3, "bool"));  INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ, ADRLAB(adr1, lab, "end"),  ADRVAR(adr2, condStr),  ADRSTR(adr3, "nil"));  INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMP, ADRLAB(adr1, lab, "body"), undef, undef);  INSERTLAST(L, inst, inWhile, inFunDef);

    // Condition
    inst = getInst(OP_LABEL,    ADRLAB(adr1, lab, "cond"),  undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ, ADRLAB(adr1, lab, "end"),   ADRVAR(adr2, EStr),     ADRBOOL(adr3, "false"));INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_LABEL,    ADRLAB(adr1, lab, "body"),  undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

    free(whileStr);
    free(psaStr);
    free(resStr);
    free(EStr);
    free(lab);
    free(condStr);
}

void genWhileEnd(TInstrList *L, unsigned whileCnt, bool inWhile, bool inFunDef){
    TInst inst;
    TAdr adr1, undef;
    char *whileStr = convIntToStr(whileCnt);
    char *lab = getStr(2, "while", whileStr);
    inst = getInst(OP_JUMP,  ADRLAB(adr1, lab, "begin"), undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_LABEL, ADRLAB(adr1, lab, "end"),   undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
    free(whileStr);
    free(lab);
}

void genIfCond(TInstrList *L, unsigned ifCnt, unsigned psa, unsigned psaResultE, bool inWhile, bool inFunDef){
    TInst inst;
    TAdr adr1, adr2, adr3, undef;
    char *ifStr = convIntToStr(ifCnt);
    char *psaStr = convIntToStr(psa);
    char *resStr = convIntToStr(psaResultE);
    char *EStr = getStr(4, "PSA", psaStr, "E", resStr);
    char *lab = getStr(2, "if", ifStr);
    char *condStr = getStr(3, "if", ifStr, "condtype");

    // Result of the PSA must be bool so we'll check that first
    /*inst = getInst(OP_DEFVAR,   ADRVAR(adr1, condStr),      undef,                  undef);                 INSERTBEFOREWHILE(L, inst, inWhile, inFunDef);
    inst = getInst(OP_TYPE,     ADRVAR(adr1, condStr),      ADRVAR(adr2, EStr),     undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ, ADRLAB(adr1, lab, "cond"),  ADRVAR(adr2, condStr),  ADRSTR(adr3, "bool"));  INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_EXIT,     ADRINT(adr1, 4),            undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_LABEL,    ADRLAB(adr1, lab, "cond"),  undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);*/

    inst = getInst(OP_DEFVAR,   ADRVAR(adr1, condStr),      undef,                  undef);                 INSERTBEFOREWHILE(L, inst, inWhile, inFunDef);
    inst = getInst(OP_TYPE,     ADRVAR(adr1, condStr),      ADRVAR(adr2, EStr),     undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ, ADRLAB(adr1, lab, "cond"),  ADRVAR(adr2, condStr),  ADRSTR(adr3, "bool"));  INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ, ADRLAB(adr1, lab, "else"),  ADRVAR(adr2, condStr),  ADRSTR(adr3, "nil"));  INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMP, ADRLAB(adr1, lab, "true"), undef, undef);  INSERTLAST(L, inst, inWhile, inFunDef);

    // Condition
    inst = getInst(OP_LABEL, ADRLAB(adr1, lab, "cond"),  undef, undef);  INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ, ADRLAB(adr1, lab, "else"),   ADRVAR(adr2, EStr),     ADRBOOL(adr3, "false"));INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_LABEL,    ADRLAB(adr1, lab, "true"),  undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

    free(ifStr);
    free(psaStr);
    free(resStr);
    free(EStr);
    free(lab);
    free(condStr);
}

void genIfElse(TInstrList *L, unsigned ifCnt, bool inWhile, bool inFunDef){
    TInst inst;
    TAdr adr1, undef;
    char *ifStr = convIntToStr(ifCnt);
    char *lab = getStr(2, "if", ifStr);
    inst = getInst(OP_JUMP,  ADRLAB(adr1, lab, "end"), undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_LABEL, ADRLAB(adr1, lab, "else"),   undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
    free(ifStr);
    free(lab);
}

void genIfEnd(TInstrList *L, unsigned ifCnt, bool inWhile, bool inFunDef){
    TInst inst;
    TAdr adr1, undef;
    char *ifStr = convIntToStr(ifCnt);
    char *lab = getStr(2, "if", ifStr);
    inst = getInst(OP_LABEL, ADRLAB(adr1, lab, "end"),   undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
    free(ifStr);
    free(lab);
}

void genE(TInstrList *L, unsigned psa, unsigned resultE, TAdr srcVar, unsigned srcVarParOrder, bool inWhile, bool inFunDef){
    TInst inst;
    TAdr adr1, adr2, undef;
    char *psaStr = convIntToStr(psa);
    char *resEStr = convIntToStr(resultE);
    char *resVarStr = getStr(4, "PSA", psaStr, "E", resEStr);
    char *funParStr = convIntToStr(srcVarParOrder);
    inst = getInst(OP_DEFVAR, ADRVAR(adr1, resVarStr), undef, undef); INSERTBEFOREWHILE(L, inst, inWhile, inFunDef);
    switch (srcVar.type){
        case ADRTYPE_VAR:
            if (srcVarParOrder > 0){
                inst = getInst(OP_MOVE, ADRVAR(adr1, resVarStr), ADRVAR(adr2, funParStr), undef);
    fprintf(stderr, "E var par: %s\n", srcVar.val.s);}
            else{
                inst = getInst(OP_MOVE, ADRVAR(adr1, resVarStr), ADRVAR(adr2, srcVar.val.s), undef);
    fprintf(stderr, "E variable: %s\n", srcVar.val.s);}
            break;
        case ADRTYPE_INT:    inst = getInst(OP_MOVE, ADRVAR(adr1, resVarStr), ADRINT(adr2, srcVar.val.i), undef); break;
        case ADRTYPE_FLOAT:  inst = getInst(OP_MOVE, ADRVAR(adr1, resVarStr), ADRFLT(adr2, srcVar.val.f), undef); break;
        case ADRTYPE_STRING: inst = getInst(OP_MOVE, ADRVAR(adr1, resVarStr), ADRSTR(adr2, srcVar.val.s), undef); break;
        case ADRTYPE_NIL:    inst = getInst(OP_MOVE, ADRVAR(adr1, resVarStr), ADRNIL(adr2), undef); break;
    }
    INSERTLAST(L, inst, inWhile, inFunDef);
    free(funParStr);
    free(psaStr);
    free(resEStr);
    free(resVarStr);
}

void genPlusE(TInstrList *L, unsigned psa, unsigned resultE, bool inWhile, bool inFunDef){
    TInst inst;
    TAdr adr1, adr2, undef;
    char *psaStr = convIntToStr(psa);
    char *resEStr = convIntToStr(resultE);
    char *resVarStr = getStr(4, "PSA", psaStr, "E", resEStr);

    inst = getInst(OP_CREATEFRAME,  undef,                      undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_DEFVAR,       ADRVARTMP(adr1, "1"),  undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_MOVE,         ADRVARTMP(adr1, "1"),     ADRVAR(adr2, resVarStr), undef); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_PUSHFRAME,    undef,                      undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_CALL,         ADRLAB(adr1, "$genminusE", ""),      undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_POPFRAME,     undef,                      undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);

    free(psaStr);
    free(resEStr);
    free(resVarStr);
}

void genMinusE(TInstrList *L, unsigned psa, unsigned resultE, bool inWhile, bool inFunDef){
    TInst inst;
    TAdr adr1, adr2, undef;
    fprintf(stderr, "received idx: %u\n", resultE);
    char *psaStr = convIntToStr(psa);
    char *resEStr = convIntToStr(resultE);
    char *resVarStr = getStr(4, "PSA", psaStr, "E", resEStr);

    inst = getInst(OP_CREATEFRAME,  undef,                      undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
    //inst = getInst(OP_DEFVAR,       ADRVARTMP(adr1, "RETVAL"),  undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_DEFVAR,       ADRVARTMP(adr1, "1"),  undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_MOVE,         ADRVARTMP(adr1, "1"),     ADRVAR(adr2, resVarStr), undef); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_PUSHFRAME,    undef,                      undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_CALL,         ADRLAB(adr1, "$genminusE", ""),      undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_POPFRAME,     undef,                      undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_MOVE,         ADRVAR(adr1, resVarStr),    ADRVARTMP(adr2, "RETVAL"), undef); INSERTLAST(L, inst, inWhile, inFunDef);

    free(psaStr);
    free(resEStr);
    free(resVarStr);
}

void genDefVar(TInstrList *L, char *dstVar, bool inWhile, bool inFunDef){
    TInst inst;
    TAdr adr1, adr2, adr3;
    inst = getInst(OP_DEFVAR,   ADRVAR(adr1, dstVar),   adr2,           adr3);   INSERTBEFOREWHILE(L, inst, inWhile, inFunDef);   
    inst = getInst(OP_MOVE,     ADRVAR(adr1, dstVar),   ADRNIL(adr2),   adr3);   INSERTLAST(L, inst, inWhile, inFunDef);
}

void genAssign(TInstrList *L, char *dstVar, unsigned dstVarParOrder, unsigned psa, unsigned psaResultE, bool inWhile, bool inFunDef){
    TInst inst;
    TAdr adr1, adr2, adr3, undef;
    char *psaStr = convIntToStr(psa);
    char *EStr = convIntToStr(psaResultE);
    char *psaVarStr = getStr(4, "PSA", psaStr, "E", EStr);
    char *psaVarStrType = getStr(2, psaVarStr, "type");
    char *funParStr = convIntToStr(dstVarParOrder);

    inst = getInst(OP_DEFVAR, ADRVAR(adr1, psaVarStrType), undef, undef); INSERTBEFOREWHILE(L, inst, inWhile, inFunDef);
    inst = getInst(OP_TYPE, ADRVAR(adr1, psaVarStrType), ADRVAR(adr2, psaVarStr), undef); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFNEQ, ADRLAB(adr1, psaVarStr,"notbool"), ADRVAR(adr2, psaVarStrType), ADRSTR(adr3, "bool")); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_EXIT, ADRINT(adr1, 4), undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_LABEL, ADRLAB(adr1, psaVarStr,"notbool"), undef, undef); INSERTLAST(L, inst, inWhile, inFunDef);

    if (dstVarParOrder > 0){

        inst = getInst(OP_MOVE, ADRVAR(adr1, funParStr), ADRVAR(adr2, psaVarStr), undef); INSERTLAST(L, inst, inWhile, inFunDef);
    }
    else {
        inst = getInst(OP_MOVE, ADRVAR(adr1, dstVar), ADRVAR(adr2, psaVarStr), undef); INSERTLAST(L, inst, inWhile, inFunDef);
    }
    free(psaStr);
    free(psaVarStr);
    free(psaVarStrType);
    free(EStr);
    free(funParStr);
}

void genAssignRetval(TInstrList *L, char *dstVar, unsigned dstVarParOrder, bool inWhile, bool inFunDef){
    TInst inst;
    TAdr adr1, adr2, undef;
    char *funParStr = convIntToStr(dstVarParOrder);
    if (dstVarParOrder > 0){
        inst = getInst(OP_MOVE, ADRVAR(adr1, funParStr), ADRVAR(adr2, "RETVAL"), undef); INSERTLAST(L, inst, inWhile, inFunDef);
    }
    else {
        inst = getInst(OP_MOVE, ADRVAR(adr1, dstVar), ADRVAR(adr2, "RETVAL"), undef); INSERTLAST(L, inst, inWhile, inFunDef);
    }
    free(funParStr);
}

void genAdd(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile, bool inFunDef){
    TInst inst;
    TAdr adr1, adr2, adr3, undef;
    
    char *psaStr = convIntToStr(psa);;
    char *resStr = convIntToStr(res);
    char *var1Str = convIntToStr(var1);
    char *var2Str = convIntToStr(var2);
    char *E1 = getStr(4, "PSA", psaStr, "E", var1Str);  
    char *E2 = getStr(4, "PSA", psaStr, "E", var2Str);  
    char *E3 = getStr(4, "PSA", psaStr, "E", resStr);  
    char *E1type = getStr(5, "PSA", psaStr, "E", var1Str, "type");  
    char *E2type = getStr(5, "PSA", psaStr, "E", var2Str, "type");  
    char *lab = getStr(4, "PSA", psaStr, "E", resStr);

    // Define result variable, operand1 type and operand2 type variable
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E3),                   undef,                  undef);                 INSERTBEFOREWHILE(L, inst, inWhile, inFunDef);
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E1type),               undef,                  undef);                 INSERTBEFOREWHILE(L, inst, inWhile, inFunDef);
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E2type),               undef,                  undef);                 INSERTBEFOREWHILE(L, inst, inWhile, inFunDef);

    // Get types of operands
    inst = getInst(OP_TYPE,         ADRVAR(adr1, E1type),               ADRVAR(adr2, E1),       undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_TYPE,         ADRVAR(adr1, E2type),               ADRVAR(adr2, E2),       undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    
    // If types are equal, skip to typeeq; if one of them is float, skip to conversion; else exit
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "typeeq"),        ADRVAR(adr2, E1type),   ADRVAR(adr3, E2type));  INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "floatfirst"),    ADRVAR(adr2, E1type),   ADRSTR(adr3, "float")); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "floatsecond"),   ADRVAR(adr2, E2type),   ADRSTR(adr3, "float")); INSERTLAST(L, inst, inWhile, inFunDef);

    // Exit 4
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "exit"),          undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_EXIT,         ADRINT(adr1, 4),                    undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

    // First operand is float, if second is integer, convert it to float and skip to addition, else exit
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "floatfirst"),    undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFNEQ,    ADRLAB(adr1, lab, "exit"),          ADRVAR(adr2, E2type),   ADRSTR(adr3, "int"));   INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_INT2FLOAT,    ADRVAR(adr1, E2),                   ADRVAR(adr2, E2),       undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "add"),           undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

    // Second operand is float, if first is integer, convert it to float and skip to addition, else exit
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "floatsecond"),   undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFNEQ,    ADRLAB(adr1, lab, "exit"),          ADRVAR(adr2, E1type),   ADRSTR(adr3, "int"));   INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_INT2FLOAT,    ADRVAR(adr1, E1),                   ADRVAR(adr2, E1),       undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "add"),           undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

    // Types are equal: if they are both float or integer, add them; if they are strings, cat them; else exit
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "typeeq"),        undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "add"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "int"));   INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "add"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "float")); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "cat"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "string"));INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_EXIT,         ADRINT(adr1, 4),                    undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

    // Concatenates strings and finishes
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "cat"),           undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_CONCAT,       ADRVAR(adr1, E3),                   ADRVAR(adr2, E1),       ADRVAR(adr3, E2));      INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "end"),           undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

    // Adds variables and finishes
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "add"),           undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_ADD,          ADRVAR(adr1, E3),                   ADRVAR(adr2, E1),       ADRVAR(adr3, E2));      INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "end"),           undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

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

void genSubOrMul(TOperation op, TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile, bool inFunDef){
    TInst inst;
    TAdr adr1, adr2, adr3, undef;
    
    char *psaStr = convIntToStr(psa);
    char *resStr = convIntToStr(res);
    char *var1Str = convIntToStr(var1);
    char *var2Str = convIntToStr(var2);
    char *E1 = getStr(4, "PSA", psaStr, "E", var1Str);  
    char *E2 = getStr(4, "PSA", psaStr, "E", var2Str);  
    char *E3 = getStr(4, "PSA", psaStr, "E", resStr);  
    char *E1type = getStr(5, "PSA", psaStr, "E", var1Str, "type");  
    char *E2type = getStr(5, "PSA", psaStr, "E", var2Str, "type");  
    char *lab = getStr(4, "PSA", psaStr, "E", resStr);

    // Define result variable, operand1 type and operand2 type variable
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E3),                   undef,                  undef);                 INSERTBEFOREWHILE(L, inst, inWhile, inFunDef);
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E1type),               undef,                  undef);                 INSERTBEFOREWHILE(L, inst, inWhile, inFunDef);
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E2type),               undef,                  undef);                 INSERTBEFOREWHILE(L, inst, inWhile, inFunDef);

    // Get types of operands
    inst = getInst(OP_TYPE,         ADRVAR(adr1, E1type),               ADRVAR(adr2, E1),       undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_TYPE,         ADRVAR(adr1, E2type),               ADRVAR(adr2, E2),       undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    
    // If types are equal, skip to typeeq; if one of them is float, skip to conversion; else exit
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "typeeq"),        ADRVAR(adr2, E1type),   ADRVAR(adr3, E2type));  INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "floatfirst"),    ADRVAR(adr2, E1type),   ADRSTR(adr3, "float")); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "floatsecond"),   ADRVAR(adr2, E2type),   ADRSTR(adr3, "float")); INSERTLAST(L, inst, inWhile, inFunDef);

    // Exit 4
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "exit"),          undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_EXIT,         ADRINT(adr1, 4),                    undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

    // First operand is float, if second is integer, convert it to float and skip to operation, else exit
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "floatfirst"),    undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFNEQ,    ADRLAB(adr1, lab, "exit"),          ADRVAR(adr2, E2type),   ADRSTR(adr3, "int"));   INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_INT2FLOAT,    ADRVAR(adr1, E2),                   ADRVAR(adr2, E2),       undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "op"),           undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

    // Second operand is float, if first is integer, convert it to float and skip to operation, else exit
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "floatsecond"),   undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFNEQ,    ADRLAB(adr1, lab, "exit"),          ADRVAR(adr2, E1type),   ADRSTR(adr3, "int"));   INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_INT2FLOAT,    ADRVAR(adr1, E1),                   ADRVAR(adr2, E1),       undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "op"),           undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

    // Types are equal: if they are both float or integer, add them; if they are strings, cat them; else exit
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "typeeq"),        undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "op"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "int"));   INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "op"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "float")); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_EXIT,         ADRINT(adr1, 4),                    undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

    // Subs/muls variables and finishes
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "op"),           undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    if (op == OP_SUB){
        inst = getInst(OP_SUB,          ADRVAR(adr1, E3),                   ADRVAR(adr2, E1),       ADRVAR(adr3, E2));      INSERTLAST(L, inst, inWhile, inFunDef);
    }
    else {
        inst = getInst(OP_MUL,          ADRVAR(adr1, E3),                   ADRVAR(adr2, E1),       ADRVAR(adr3, E2));      INSERTLAST(L, inst, inWhile, inFunDef);
    }
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "end"),           undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

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

void genSub(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile, bool inFunDef){
    genSubOrMul(OP_SUB, L, psa, res, var1, var2, inWhile, inFunDef);
}
void genMul(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile, bool inFunDef){
    genSubOrMul(OP_MUL, L, psa, res, var1, var2, inWhile, inFunDef);
}

void genDiv(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile, bool inFunDef){
    TInst inst;
    TAdr adr1, adr2, adr3, undef;
    
    char *psaStr = convIntToStr(psa);
    char *resStr = convIntToStr(res);
    char *var1Str = convIntToStr(var1);
    char *var2Str = convIntToStr(var2);
    char *E1 = getStr(4, "PSA", psaStr, "E", var1Str);  
    char *E2 = getStr(4, "PSA", psaStr, "E", var2Str);  
    char *E3 = getStr(4, "PSA", psaStr, "E", resStr);  
    char *E1type = getStr(5, "PSA", psaStr, "E", var1Str, "type");  
    char *E2type = getStr(5, "PSA", psaStr, "E", var2Str, "type");  
    char *lab = getStr(4, "PSA", psaStr, "E", resStr);

    // Define result variable, operand1 type and operand2 type variable
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E3),                   undef,                  undef);                 INSERTBEFOREWHILE(L, inst, inWhile, inFunDef);
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E1type),               undef,                  undef);                 INSERTBEFOREWHILE(L, inst, inWhile, inFunDef);
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E2type),               undef,                  undef);                 INSERTBEFOREWHILE(L, inst, inWhile, inFunDef);

    // Get types of operands
    inst = getInst(OP_TYPE,         ADRVAR(adr1, E1type),               ADRVAR(adr2, E1),       undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_TYPE,         ADRVAR(adr1, E2type),               ADRVAR(adr2, E2),       undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    
    // If types are equal, skip to typeeq; if one of them is float, skip to conversion; else exit
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "typeeq"),        ADRVAR(adr2, E1type),   ADRVAR(adr3, E2type));  INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "floatfirst"),    ADRVAR(adr2, E1type),   ADRSTR(adr3, "float")); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "floatsecond"),   ADRVAR(adr2, E2type),   ADRSTR(adr3, "float")); INSERTLAST(L, inst, inWhile, inFunDef);

    // Exit 4
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "exit"),          undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_EXIT,         ADRINT(adr1, 4),                    undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

    // First operand is float, if second is integer, convert it to float and skip to operation, else exit
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "floatfirst"),    undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFNEQ,    ADRLAB(adr1, lab, "exit"),          ADRVAR(adr2, E2type),   ADRSTR(adr3, "int"));   INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_INT2FLOAT,    ADRVAR(adr1, E2),                   ADRVAR(adr2, E2),       undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "checkfloat"),           undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

    // Second operand is float, if first is integer, convert it to float and skip to operation, else exit
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "floatsecond"),   undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFNEQ,    ADRLAB(adr1, lab, "exit"),          ADRVAR(adr2, E1type),   ADRSTR(adr3, "int"));   INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_INT2FLOAT,    ADRVAR(adr1, E1),                   ADRVAR(adr2, E1),       undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "checkfloat"),           undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

    // Types are equal: if they are both float or integer, add them; if they are strings, cat them; else exit
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "typeeq"),        undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "checkint"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "int"));   INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "checkfloat"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "float")); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_EXIT,         ADRINT(adr1, 4),                    undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

    // Before division check whether second operand is nonzero
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "checkint"),           undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_INT2FLOAT,    ADRVAR(adr1, E1),                   ADRVAR(adr2, E1),       undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_INT2FLOAT,    ADRVAR(adr1, E2),                   ADRVAR(adr2, E2),       undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFNEQ,     ADRLAB(adr1, lab, "opint"),           ADRVAR(adr2, E2),   ADRFLT(adr3, 0));   INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_EXIT,         ADRINT(adr1, 9),                    undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

    // Before division check whether second operand is nonzero
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "checkfloat"),           undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "exit"),           ADRVAR(adr2, E2),   ADRFLT(adr3, 0));   INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_DIV,          ADRVAR(adr1, E3),                   ADRVAR(adr2, E1),       ADRVAR(adr3, E2));      INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "end"),           undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
        
    // Divides variables that were previously integers, converts them back to int and finishes
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "opint"),           undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_DIV,          ADRVAR(adr1, E3),                   ADRVAR(adr2, E1),       ADRVAR(adr3, E2));      INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_FLOAT2INT,    ADRVAR(adr1, E3),                   ADRVAR(adr2, E3),       undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "end"),           undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

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

void genCmp(TOperation op, TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile, bool inFunDef){
    TInst inst;
    TAdr adr1, adr2, adr3, undef;
    
    char *psaStr = convIntToStr(psa);
    char *resStr = convIntToStr(res);
    char *var1Str = convIntToStr(var1);
    char *var2Str = convIntToStr(var2);
    char *E1 = getStr(4, "PSA", psaStr, "E", var1Str);  
    char *E2 = getStr(4, "PSA", psaStr, "E", var2Str);  
    char *E3 = getStr(4, "PSA", psaStr, "E", resStr);  
    char *E1type = getStr(5, "PSA", psaStr, "E", var1Str, "type");  
    char *E2type = getStr(5, "PSA", psaStr, "E", var2Str, "type");  
    char *lab = getStr(4, "PSA", psaStr, "E", resStr);

     // Define result variable, operand1 type and operand2 type variable
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E3),                   undef,                  undef);                 INSERTBEFOREWHILE(L, inst, inWhile, inFunDef);
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E1type),               undef,                  undef);                 INSERTBEFOREWHILE(L, inst, inWhile, inFunDef);
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E2type),               undef,                  undef);                 INSERTBEFOREWHILE(L, inst, inWhile, inFunDef);

    // Get types of operands
    inst = getInst(OP_TYPE,         ADRVAR(adr1, E1type),               ADRVAR(adr2, E1),       undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_TYPE,         ADRVAR(adr1, E2type),               ADRVAR(adr2, E2),       undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    
    // If types are equal, skip to typeeq; if one of them is float, skip to conversion; else exit
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "typeeq"),        ADRVAR(adr2, E1type),   ADRVAR(adr3, E2type));  INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "floatfirst"),    ADRVAR(adr2, E1type),   ADRSTR(adr3, "float")); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "floatsecond"),   ADRVAR(adr2, E2type),   ADRSTR(adr3, "float")); INSERTLAST(L, inst, inWhile, inFunDef);

    // Exit 4
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "exit"),          undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_EXIT,         ADRINT(adr1, 4),                    undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

    // First operand is float, if second is integer, convert it to float and skip to addition, else exit
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "floatfirst"),    undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFNEQ,    ADRLAB(adr1, lab, "exit"),          ADRVAR(adr2, E2type),   ADRSTR(adr3, "int"));   INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_INT2FLOAT,    ADRVAR(adr1, E2),                   ADRVAR(adr2, E2),       undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "cmp"),           undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

    // Second operand is float, if first is integer, convert it to float and skip to addition, else exit
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "floatsecond"),   undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFNEQ,    ADRLAB(adr1, lab, "exit"),          ADRVAR(adr2, E1type),   ADRSTR(adr3, "int"));   INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_INT2FLOAT,    ADRVAR(adr1, E1),                   ADRVAR(adr2, E1),       undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "cmp"),           undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

    // Types are equal: if they are both float or integer or strings, cmp them; else exit
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "typeeq"),        undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "cmp"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "int"));   INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "cmp"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "float")); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "cmp"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "string"));INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_EXIT,         ADRINT(adr1, 4),                    undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

    // Compares variables and finishes
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "cmp"),           undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    switch (op){
        case OP_LT:
            inst = getInst(OP_LT,   ADRVAR(adr1, E3),                   ADRVAR(adr2, E1),       ADRVAR(adr3, E2));      INSERTLAST(L, inst, inWhile, inFunDef);
            break;
        case OP_GT:
            inst = getInst(OP_GT,   ADRVAR(adr1, E3),                   ADRVAR(adr2, E1),       ADRVAR(adr3, E2));      INSERTLAST(L, inst, inWhile, inFunDef);
            break;
        case OP_LEQ:
            inst = getInst(OP_GT,   ADRVAR(adr1, E3),                   ADRVAR(adr2, E1),       ADRVAR(adr3, E2));      INSERTLAST(L, inst, inWhile, inFunDef);
            inst = getInst(OP_NOT,  ADRVAR(adr1, E3),                   ADRVAR(adr2, E3),       undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
            break;
        case OP_GEQ:
            inst = getInst(OP_LT,   ADRVAR(adr1, E3),                   ADRVAR(adr2, E1),       ADRVAR(adr3, E2));      INSERTLAST(L, inst, inWhile, inFunDef);
            inst = getInst(OP_NOT,  ADRVAR(adr1, E3),                   ADRVAR(adr2, E3),       undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
            break;
    }

    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "end"),           undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

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

void genLT(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile, bool inFunDef){
    genCmp(OP_LT, L, psa, res, var1, var2, inWhile, inFunDef);
}
void genGT(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile, bool inFunDef){
    genCmp(OP_GT, L, psa, res, var1, var2, inWhile, inFunDef);
}
void genLEQ(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile, bool inFunDef){
    genCmp(OP_LEQ, L, psa, res, var1, var2, inWhile, inFunDef);
}
void genGEQ(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile, bool inFunDef){
    genCmp(OP_GEQ, L, psa, res, var1, var2, inWhile, inFunDef);
}

void genEQorNEQ(TOperation op, TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile, bool inFunDef){
    TInst inst;
    TAdr adr1, adr2, adr3, undef;
    
    char *psaStr = convIntToStr(psa);
    char *resStr = convIntToStr(res);
    char *var1Str = convIntToStr(var1);
    char *var2Str = convIntToStr(var2);
    char *E1 = getStr(4, "PSA", psaStr, "E", var1Str);  
    char *E2 = getStr(4, "PSA", psaStr, "E", var2Str);  
    char *E3 = getStr(4, "PSA", psaStr, "E", resStr);  
    char *E1type = getStr(5, "PSA", psaStr, "E", var1Str, "type");  
    char *E2type = getStr(5, "PSA", psaStr, "E", var2Str, "type");  
    char *lab = getStr(4, "PSA", psaStr, "E", resStr);

     // Define result variable, operand1 type and operand2 type variable
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E3),                   undef,                  undef);                 INSERTBEFOREWHILE(L, inst, inWhile, inFunDef);
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E1type),               undef,                  undef);                 INSERTBEFOREWHILE(L, inst, inWhile, inFunDef);
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E2type),               undef,                  undef);                 INSERTBEFOREWHILE(L, inst, inWhile, inFunDef);

    // Get types of operands
    inst = getInst(OP_TYPE,         ADRVAR(adr1, E1type),               ADRVAR(adr2, E1),       undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_TYPE,         ADRVAR(adr1, E2type),               ADRVAR(adr2, E2),       undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    
    // If types are equal, skip to typeeq; if one of them is float, skip to conversion; else exit
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "typeeq"),        ADRVAR(adr2, E1type),   ADRVAR(adr3, E2type));  INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "floatfirst"),    ADRVAR(adr2, E1type),   ADRSTR(adr3, "float")); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "floatsecond"),   ADRVAR(adr2, E2type),   ADRSTR(adr3, "float")); INSERTLAST(L, inst, inWhile, inFunDef);

    // Different types => always false
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "false"),          undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    //inst = getInst(OP_EXIT,         ADRINT(adr1, 4),                    undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_MOVE,         ADRVAR(adr1, E3),                   ADRBOOL(adr2, "true"),  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "end"),           undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

    // First operand is float, if second is integer, convert it to float and skip to addition, else false
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "floatfirst"),    undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFNEQ,    ADRLAB(adr1, lab, "false"),          ADRVAR(adr2, E2type),   ADRSTR(adr3, "int"));   INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_INT2FLOAT,    ADRVAR(adr1, E2),                   ADRVAR(adr2, E2),       undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "cmp"),           undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

    // Second operand is float, if first is integer, convert it to float and skip to addition, else false
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "floatsecond"),   undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFNEQ,    ADRLAB(adr1, lab, "false"),          ADRVAR(adr2, E1type),   ADRSTR(adr3, "int"));   INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_INT2FLOAT,    ADRVAR(adr1, E1),                   ADRVAR(adr2, E1),       undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "cmp"),           undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

    // Types are equal: if they are both float or integer or strings, cmp them; else false
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "typeeq"),        undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "cmp"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "int"));   INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "cmp"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "float")); INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "cmp"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "string"));INSERTLAST(L, inst, inWhile, inFunDef);
    
    // Both operands are nil => true
    inst = getInst(OP_MOVE,         ADRVAR(adr1, E3),                   ADRBOOL(adr2, "true"),  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "end"),           undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

    // Compares variables and finishes
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "cmp"),           undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    inst = getInst(OP_EQ,   ADRVAR(adr1, E3),                   ADRVAR(adr2, E1),       ADRVAR(adr3, E2));      INSERTLAST(L, inst, inWhile, inFunDef);

    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "end"),           undef,                  undef);                 INSERTLAST(L, inst, inWhile, inFunDef);

    if (op == OP_NEQ){
        inst = getInst(OP_NOT,  ADRVAR(adr1, E3),                   ADRVAR(adr2, E3),       undef);                 INSERTLAST(L, inst, inWhile, inFunDef);
    }

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

void genEQ(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile, bool inFunDef){
    genEQorNEQ(OP_EQ, L, psa, res, var1, var2, inWhile, inFunDef);
}
void genNEQ(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile, bool inFunDef){
    genEQorNEQ(OP_NEQ, L, psa, res, var1, var2, inWhile, inFunDef);
}
/*
int main(){
    TInstrList L;
    ILInit(&L);
    genPrgBegin(&L);
    /*genAdd(&L, 5, 3, 1, 2, true);
    genDefVar(&L, "yoo", true);
    genFunCallBegin(&L, "inputs", true);*/
    
    /*TAdr var;
    var.type = ADRTYPE_INT;
    var.val.i = 2;
    genFunCallPar(&L, "print", 1, var, true, false);
    
    var.type = ADRTYPE_STRING;
    var.val.s = "ahoj svete\n :)";
    genFunCallPar(&L, "print", 2, var, true, false);
    
    var.type = ADRTYPE_FLOAT;
    var.val.f = 3.14;
    genFunCallPar(&L, "print", 3, var, true, false);

    var.type = ADRTYPE_NIL;
    genFunCallPar(&L, "print", 3, var, true, false);

    var.type = ADRTYPE_VAR;
    var.val.s = "variable";
    genFunCallPar(&L, "print", 4, var, true, false);*/
    
    //genFunCallEnd(&L, "inputs", true);
    
    /*genWhileBegin(&L, 6, false);
    genWhileCond(&L, 6, 5, 8, false);
    genWhileEnd(&L, 6, false);*/




    //genWhileCond(&L, 1, 1, 1, true, true);
    //genWhileBegin(&L, 1, true, true);
    //genFunDefEnd(&L, false, true);
    //genGEQ(&L, 5, 3, 1, 2, false);
    //genLEQ(&L, 5, 3, 1, 2, false);
    //genGT(&L, 5, 3, 1, 2, false);
    //genLT(&L, 5, 3, 1, 2, false);
    /*genIfCond(&L, 5, 3, 3, false);
    genIfElse(&L, 5, false);
    genIfEnd(&L, 5, false);
    genAdd(&L,1,3,1,2,false);
    genSub(&L,2,3,1,2,false);
    genMul(&L,3,3,1,2,false);
    genDiv(&L,4,3,1,2,false);
    genLT(&L,5,3,1,2,false);
    genGT(&L,6,3,1,2,false);
    genLEQ(&L,7,3,1,2,false);
    genGEQ(&L,8,3,1,2,false);*/
    //genPrgEnd(&L);/*
    /*
    ILPrintAllInst(L);
    ILDisposeList(&L);
}*/
