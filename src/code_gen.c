/*
 * file name:       code_gen.c
 * project:         VUT-FIT-IFJ-2018
 * created:         23.11.2018
 * last modified:   1.12.2018
 * 
 * created by:      Jan Havlín xhavli47@stud.fit.vutbr.cz
 * modification:    
 * 
 * description: Functions for converting specific block of code into three address code. 
 */

#include "code_gen.h"

//static TInstrList L;
static int funPara;
int errflg;

void genPrgBegin(TInstrList *L){
    TInst inst;
    TAdr adr1, adr2, undef;
    inst = getInst(OP_LABEL, ADRLAB(adr1, "$main", "    # Main"), undef, undef); ILInsertLast(L, inst, false);
    inst = getInst(OP_CREATEFRAME, undef, undef, undef); ILInsertLast(L, inst, false);
    inst = getInst(OP_PUSHFRAME, undef, undef, undef); ILInsertLast(L, inst, false);
    inst = getInst(OP_DEFVAR, ADRVAR(adr1, "RETVAL"), undef, undef); ILInsertLast(L, inst, false);
    inst = getInst(OP_MOVE, ADRVAR(adr1, "RETVAL"), ADRNIL(adr2), undef); ILInsertLast(L, inst, false);
    ILSetActFunFirst(L);
}

void genPrgEnd(TInstrList *L){
    return;
    /*TInst inst;
    TAdr adr1, undef;
    inst = getInst(OP_JUMP, ADRLAB(adr1, "main", ""), undef, undef); ILInsertFirst(L, inst);*/
}

/*void genFunLength(TInstrList *L){
    TInst inst;
    TAdr adr1, adr2, adr3, undef;
    inst = getInst(OP_LABEL, ADRLAB(adr1, "length", ""), undef, undef); ILPreActFunInsert(L, inst);

    inst = getInst(OP_MOVE, ADRVAR(adr1, "RETVAL"), ADRNIL(adr2), undef); ILInsertLast(L, inst, false);
    inst = getInst(OP_RETURN, undef, undef, undef); ILPreActFunInsert(L, inst);
}*/
void genFunDefBegin(TInstrList *L, char *fun, bool inWhile){
    TInst inst;
    TAdr adr1, undef;
    inst = getInst(OP_LABEL, ADRLAB(adr1, fun,""), undef, undef); ILInsertLast(L, inst, inWhile);
}

void genFunDefEnd(TInstrList *L, bool inWhile){
    TInst inst;
    TAdr undef;
    inst = getInst(OP_RETURN, undef, undef, undef); ILInsertLast(L, inst, inWhile);
}

void genFunCallBegin(TInstrList *L, char *fun, bool inWhile){
    if (strcmp(fun, "print") == 0 ||
        strcmp(fun, "inputs") == 0 ||
        strcmp(fun, "inputi") == 0 ||
        strcmp(fun, "inputf") == 0){
        return;
    }

    TInst inst;
    TAdr undef;
    funPara = 0;
    inst = getInst(OP_CREATEFRAME, undef, undef, undef); ILInsertLast(L, inst, inWhile);
}

void genFunCallPar(TInstrList *L, char *fun, unsigned parNum, TAdr var, bool inWhile){
    TInst inst;
    TAdr adr1, adr2, undef;
    
    if (strcmp(fun, "print") == 0){
        switch (var.type){
            case ADRTYPE_VAR:    inst = getInst(OP_WRITE, ADRVAR(adr2, var.val.s), undef, undef); break;
            case ADRTYPE_INT:    inst = getInst(OP_WRITE, ADRINT(adr2, var.val.i), undef, undef); break;
            case ADRTYPE_FLOAT:  inst = getInst(OP_WRITE, ADRFLT(adr2, var.val.f), undef, undef); break;
            case ADRTYPE_STRING: inst = getInst(OP_WRITE, ADRSTR(adr2, var.val.s), undef, undef); break;
            case ADRTYPE_NIL:    inst = getInst(OP_WRITE, ADRNIL(adr2),            undef, undef); break;
        }
        ILInsertLast(L, inst, inWhile);
    }
    // This shouldn't be needed...
    else if (strcmp(fun, "inputs") == 0 ||
             strcmp(fun, "inputi") == 0 ||
             strcmp(fun, "inputf") == 0){
        return;
    }
    else {
        char *conv = convIntToStr(parNum);
        inst = getInst(OP_DEFVAR,   ADRVARTMP(adr1, conv), undef, undef); ILPostActInsert(L, inst);
        switch (var.type){
            case ADRTYPE_VAR:    inst = getInst(OP_MOVE, ADRVARTMP(adr1, conv), ADRVAR(adr2, var.val.s), undef); break;
            case ADRTYPE_INT:    inst = getInst(OP_MOVE, ADRVARTMP(adr1, conv), ADRINT(adr2, var.val.i), undef); break;
            case ADRTYPE_FLOAT:  inst = getInst(OP_MOVE, ADRVARTMP(adr1, conv), ADRFLT(adr2, var.val.f), undef); break;
            case ADRTYPE_STRING: inst = getInst(OP_MOVE, ADRVARTMP(adr1, conv), ADRSTR(adr2, var.val.s), undef); break;
            case ADRTYPE_NIL:    inst = getInst(OP_MOVE, ADRVARTMP(adr1, conv), ADRNIL(adr2),            undef); break;
        }
        ILInsertLast(L, inst, inWhile);
        free(conv);
    }
}

void genFunCallEnd(TInstrList *L, char *fun, bool inWhile){
    TInst inst;
    TAdr adr1, adr2, undef;

    if (strcmp(fun, "print") == 0){
        inst = getInst(OP_MOVE, ADRVAR(adr1, "RETVAL"), ADRNIL(adr2), undef);
    }
    else if (strcmp(fun, "inputs") == 0){
        inst = getInst(OP_READ, ADRVAR(adr1, "RETVAL string"), undef, undef); ILInsertLast(L, inst, inWhile);
    }
    else if (strcmp(fun, "inputi") == 0){
        inst = getInst(OP_READ, ADRVAR(adr1, "RETVAL int"), undef, undef); ILInsertLast(L, inst, inWhile);
    }
    else if (strcmp(fun, "inputf") == 0){
        inst = getInst(OP_READ, ADRVAR(adr1, "RETVAL float"), undef, undef); ILInsertLast(L, inst, inWhile);
    }
    else {
    inst = getInst(OP_DEFVAR,       ADRVARTMP(adr1, "RETVAL"),  undef, undef); ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_MOVE,         ADRVARTMP(adr1, "RETVAL"),  ADRNIL(adr2), undef); ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_PUSHFRAME,    undef,                      undef, undef); ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_CALL,         ADRLAB(adr1, fun, ""),      undef, undef); ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_POPFRAME,     undef,                      undef, undef); ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_MOVE,         ADRVAR(adr1, "RETVAL"),     ADRVARTMP(adr2, "RETVAL"), undef); ILInsertLast(L, inst, inWhile);
    }

}

void genWhileBegin(TInstrList *L, unsigned whileCnt, bool inWhile){
    TInst inst;
    TAdr adr1, undef;
    char *whileStr = convIntToStr(whileCnt);
    char *lab = getStr(2, "while", whileStr);
    inst = getInst(OP_LABEL, ADRLAB(adr1, lab, "begin"), undef, undef); ILInsertLast(L, inst, inWhile);
    free(whileStr);
    free(lab);
}

void genWhileCond(TInstrList *L, unsigned whileCnt, unsigned psa, unsigned psaResultE, bool inWhile){
    TInst inst;
    TAdr adr1, adr2, adr3, undef;
    char *whileStr = convIntToStr(whileCnt);
    char *psaStr = convIntToStr(psa);
    char *resStr = convIntToStr(psaResultE);
    char *EStr = getStr(4, "PSA", psaStr, "E", resStr);
    char *lab = getStr(2, "while", whileStr);
    char *condStr = getStr(3, "while", whileStr, "condtype");

    // Result of the PSA must be bool so we'll check that first
    inst = getInst(OP_DEFVAR,   ADRVAR(adr1, condStr),      undef,                  undef);                 ILPostActInsert(L, inst);
    inst = getInst(OP_TYPE,     ADRVAR(adr1, condStr),      ADRVAR(adr2, EStr),     undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ, ADRLAB(adr1, lab, "cond"),  ADRVAR(adr2, condStr),  ADRSTR(adr3, "bool"));  ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_EXIT,     ADRINT(adr1, 4),            undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_LABEL,    ADRLAB(adr1, lab, "cond"),  undef,                  undef);                 ILInsertLast(L, inst, inWhile);

    // Condition
    inst = getInst(OP_JUMPIFEQ, ADRLAB(adr1, lab, "end"),   ADRVAR(adr2, EStr),     ADRBOOL(adr3, "false"));ILInsertLast(L, inst, inWhile);

    free(whileStr);
    free(psaStr);
    free(resStr);
    free(EStr);
    free(lab);
    free(condStr);
}

void genWhileEnd(TInstrList *L, unsigned whileCnt, bool inWhile){
    TInst inst;
    TAdr adr1, undef;
    char *whileStr = convIntToStr(whileCnt);
    char *lab = getStr(2, "while", whileStr);
    inst = getInst(OP_JUMP,  ADRLAB(adr1, lab, "begin"), undef, undef); ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_LABEL, ADRLAB(adr1, lab, "end"),   undef, undef); ILInsertLast(L, inst, inWhile);
    free(whileStr);
    free(lab);
}

void genIfCond(TInstrList *L, unsigned ifCnt, unsigned psa, unsigned psaResultE, bool inWhile){
    TInst inst;
    TAdr adr1, adr2, adr3, undef;
    char *ifStr = convIntToStr(ifCnt);
    char *psaStr = convIntToStr(psa);
    char *resStr = convIntToStr(psaResultE);
    char *EStr = getStr(4, "PSA", psaStr, "E", resStr);
    char *lab = getStr(2, "if", ifStr);
    char *condStr = getStr(3, "if", ifStr, "condtype");

    // Result of the PSA must be bool so we'll check that first
    inst = getInst(OP_DEFVAR,   ADRVAR(adr1, condStr),      undef,                  undef);                 ILPostActInsert(L, inst);
    inst = getInst(OP_TYPE,     ADRVAR(adr1, condStr),      ADRVAR(adr2, EStr),     undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ, ADRLAB(adr1, lab, "cond"),  ADRVAR(adr2, condStr),  ADRSTR(adr3, "bool"));  ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_EXIT,     ADRINT(adr1, 4),            undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_LABEL,    ADRLAB(adr1, lab, "cond"),  undef,                  undef);                 ILInsertLast(L, inst, inWhile);

    // Condition
    inst = getInst(OP_JUMPIFEQ, ADRLAB(adr1, lab, "else"),   ADRVAR(adr2, EStr),     ADRBOOL(adr3, "false"));ILInsertLast(L, inst, inWhile);

    free(ifStr);
    free(psaStr);
    free(resStr);
    free(EStr);
    free(lab);
    free(condStr);
}

void genIfElse(TInstrList *L, unsigned ifCnt, bool inWhile){
    TInst inst;
    TAdr adr1, undef;
    char *ifStr = convIntToStr(ifCnt);
    char *lab = getStr(2, "if", ifStr);
    inst = getInst(OP_JUMP,  ADRLAB(adr1, lab, "end"), undef, undef); ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_LABEL, ADRLAB(adr1, lab, "else"),   undef, undef); ILInsertLast(L, inst, inWhile);
    free(ifStr);
    free(lab);
}

void genIfEnd(TInstrList *L, unsigned ifCnt, bool inWhile){
    TInst inst;
    TAdr adr1, undef;
    char *ifStr = convIntToStr(ifCnt);
    char *lab = getStr(2, "if", ifStr);
    inst = getInst(OP_LABEL, ADRLAB(adr1, lab, "end"),   undef, undef); ILInsertLast(L, inst, inWhile);
    free(ifStr);
    free(lab);
}

void genE(TInstrList *L, unsigned psa, unsigned resultE, TAdr var, bool inWhile){
    TInst inst;
    TAdr adr1, adr2, undef;
    char *psaStr = convIntToStr(psa);
    char *resEStr = convIntToStr(resultE);
    char *resVarStr = getStr(4, "PSA", psaStr, "E", resEStr);
    inst = getInst(OP_DEFVAR, ADRVAR(adr1, resVarStr), undef, undef); ILPostActInsert(L, inst);

    switch (var.type){
        case ADRTYPE_VAR:    inst = getInst(OP_MOVE, ADRVAR(adr1, resVarStr), ADRVAR(adr2, var.val.s), undef); break;
        case ADRTYPE_INT:    inst = getInst(OP_MOVE, ADRVAR(adr1, resVarStr), ADRINT(adr2, var.val.i), undef); break;
        case ADRTYPE_FLOAT:  inst = getInst(OP_MOVE, ADRVAR(adr1, resVarStr), ADRFLT(adr2, var.val.f), undef); break;
        case ADRTYPE_STRING: inst = getInst(OP_MOVE, ADRVAR(adr1, resVarStr), ADRSTR(adr2, var.val.s), undef); break;
        case ADRTYPE_NIL:    inst = getInst(OP_MOVE, ADRVAR(adr1, resVarStr), ADRNIL(adr2), undef); break;
    }
    ILInsertLast(L, inst, inWhile); 
    free(psaStr);
    free(resEStr);
    free(resVarStr);
}

void genDefVar(TInstrList *L, char *var, bool inWhile){
    TInst inst;
    TAdr adr1, adr2, adr3;
    inst = getInst(OP_DEFVAR,   ADRVAR(adr1, var),   adr2,           adr3);   ILPostActInsert(L, inst);   
    inst = getInst(OP_MOVE,     ADRVAR(adr1, var),   ADRNIL(adr2),   adr3);   ILInsertLast(L, inst, inWhile);
}

void genAssign(TInstrList *L, char *var, unsigned funParOrder, unsigned psa, unsigned psaResultE, bool inWhile){
    TInst inst;
    TAdr adr1, adr2, undef;
    char *psaStr = convIntToStr(psa);
    char *EStr = convIntToStr(psaResultE);
    char *psaVarStr = getStr(4, "PSA", psaStr, "E", EStr);
    char *funParStr = convIntToStr(funParOrder);

    if (funParOrder > 0){
        inst = getInst(OP_MOVE, ADRVAR(adr1, funParStr), ADRVAR(adr2, psaVarStr), undef); ILInsertLast(L, inst, inWhile);
    }
    else {
        inst = getInst(OP_MOVE, ADRVAR(adr1, var), ADRVAR(adr2, psaVarStr), undef); ILInsertLast(L, inst, inWhile);
    }
    
    
    free(psaStr);
    free(psaVarStr);
    free(EStr);
    free(funParStr);
}

void genAssignRetval(TInstrList *L, char *var, unsigned funParOrder, bool inWhile){
    TInst inst;
    TAdr adr1, adr2, undef;
    char *funParStr = convIntToStr(funParOrder);
    if (funParOrder > 0){
        inst = getInst(OP_MOVE, ADRVAR(adr1, funParStr), ADRVAR(adr2, "RETVAL"), undef); ILInsertLast(L, inst, inWhile);
    }
    else {
        inst = getInst(OP_MOVE, ADRVAR(adr1, var), ADRVAR(adr2, "RETVAL"), undef); ILInsertLast(L, inst, inWhile);
    }
    free(funParStr);
}

void genAdd(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile){
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
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E3),                   undef,                  undef);                 ILPostActInsert(L, inst);
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E1type),               undef,                  undef);                 ILPostActInsert(L, inst);
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E2type),               undef,                  undef);                 ILPostActInsert(L, inst);

    // Get types of operands
    inst = getInst(OP_TYPE,         ADRVAR(adr1, E1type),               ADRVAR(adr2, E1),       undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_TYPE,         ADRVAR(adr1, E2type),               ADRVAR(adr2, E2),       undef);                 ILInsertLast(L, inst, inWhile);
    
    // If types are equal, skip to typeeq; if one of them is float, skip to conversion; else exit
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "typeeq"),        ADRVAR(adr2, E1type),   ADRVAR(adr3, E2type));  ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "floatfirst"),    ADRVAR(adr2, E1type),   ADRSTR(adr3, "float")); ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "floatsecond"),   ADRVAR(adr2, E2type),   ADRSTR(adr3, "float")); ILInsertLast(L, inst, inWhile);

    // Exit 4
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "exit"),          undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_EXIT,         ADRINT(adr1, 4),                    undef,                  undef);                 ILInsertLast(L, inst, inWhile);

    // First operand is float, if second is integer, convert it to float and skip to addition, else exit
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "floatfirst"),    undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFNEQ,    ADRLAB(adr1, lab, "exit"),          ADRVAR(adr2, E2type),   ADRSTR(adr3, "int"));   ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_INT2FLOAT,    ADRVAR(adr1, E2),                   ADRVAR(adr2, E2),       undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "add"),           undef,                  undef);                 ILInsertLast(L, inst, inWhile);

    // Second operand is float, if first is integer, convert it to float and skip to addition, else exit
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "floatsecond"),   undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFNEQ,    ADRLAB(adr1, lab, "exit"),          ADRVAR(adr2, E1type),   ADRSTR(adr3, "int"));   ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_INT2FLOAT,    ADRVAR(adr1, E1),                   ADRVAR(adr2, E1),       undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "add"),           undef,                  undef);                 ILInsertLast(L, inst, inWhile);

    // Types are equal: if they are both float or integer, add them; if they are strings, cat them; else exit
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "typeeq"),        undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "add"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "int"));   ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "add"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "float")); ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "cat"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "string"));ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_EXIT,         ADRINT(adr1, 4),                    undef,                  undef);                 ILInsertLast(L, inst, inWhile);

    // Concatenates strings and finishes
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "cat"),           undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_CONCAT,       ADRVAR(adr1, E3),                   ADRVAR(adr2, E1),       ADRVAR(adr3, E2));      ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "end"),           undef,                  undef);                 ILInsertLast(L, inst, inWhile);

    // Adds variables and finishes
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "add"),           undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_ADD,          ADRVAR(adr1, E3),                   ADRVAR(adr2, E1),       ADRVAR(adr3, E2));      ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "end"),           undef,                  undef);                 ILInsertLast(L, inst, inWhile);

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

void genSubOrMul(TOperation op, TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile){
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
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E3),                   undef,                  undef);                 ILPostActInsert(L, inst);
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E1type),               undef,                  undef);                 ILPostActInsert(L, inst);
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E2type),               undef,                  undef);                 ILPostActInsert(L, inst);

    // Get types of operands
    inst = getInst(OP_TYPE,         ADRVAR(adr1, E1type),               ADRVAR(adr2, E1),       undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_TYPE,         ADRVAR(adr1, E2type),               ADRVAR(adr2, E2),       undef);                 ILInsertLast(L, inst, inWhile);
    
    // If types are equal, skip to typeeq; if one of them is float, skip to conversion; else exit
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "typeeq"),        ADRVAR(adr2, E1type),   ADRVAR(adr3, E2type));  ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "floatfirst"),    ADRVAR(adr2, E1type),   ADRSTR(adr3, "float")); ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "floatsecond"),   ADRVAR(adr2, E2type),   ADRSTR(adr3, "float")); ILInsertLast(L, inst, inWhile);

    // Exit 4
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "exit"),          undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_EXIT,         ADRINT(adr1, 4),                    undef,                  undef);                 ILInsertLast(L, inst, inWhile);

    // First operand is float, if second is integer, convert it to float and skip to operation, else exit
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "floatfirst"),    undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFNEQ,    ADRLAB(adr1, lab, "exit"),          ADRVAR(adr2, E2type),   ADRSTR(adr3, "int"));   ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_INT2FLOAT,    ADRVAR(adr1, E2),                   ADRVAR(adr2, E2),       undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "op"),           undef,                  undef);                 ILInsertLast(L, inst, inWhile);

    // Second operand is float, if first is integer, convert it to float and skip to operation, else exit
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "floatsecond"),   undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFNEQ,    ADRLAB(adr1, lab, "exit"),          ADRVAR(adr2, E1type),   ADRSTR(adr3, "int"));   ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_INT2FLOAT,    ADRVAR(adr1, E1),                   ADRVAR(adr2, E1),       undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "op"),           undef,                  undef);                 ILInsertLast(L, inst, inWhile);

    // Types are equal: if they are both float or integer, add them; if they are strings, cat them; else exit
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "typeeq"),        undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "op"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "int"));   ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "op"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "float")); ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_EXIT,         ADRINT(adr1, 4),                    undef,                  undef);                 ILInsertLast(L, inst, inWhile);

    // Subs/muls variables and finishes
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "op"),           undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    if (op == OP_SUB){
        inst = getInst(OP_SUB,          ADRVAR(adr1, E3),                   ADRVAR(adr2, E1),       ADRVAR(adr3, E2));      ILInsertLast(L, inst, inWhile);
    }
    else {
        inst = getInst(OP_MUL,          ADRVAR(adr1, E3),                   ADRVAR(adr2, E1),       ADRVAR(adr3, E2));      ILInsertLast(L, inst, inWhile);
    }
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "end"),           undef,                  undef);                 ILInsertLast(L, inst, inWhile);

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

void genSub(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile){
    genSubOrMul(OP_SUB, L, psa, res, var1, var2, inWhile);
}
void genMul(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile){
    genSubOrMul(OP_MUL, L, psa, res, var1, var2, inWhile);
}

void genDiv(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile){
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
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E3),                   undef,                  undef);                 ILPostActInsert(L, inst);
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E1type),               undef,                  undef);                 ILPostActInsert(L, inst);
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E2type),               undef,                  undef);                 ILPostActInsert(L, inst);

    // Get types of operands
    inst = getInst(OP_TYPE,         ADRVAR(adr1, E1type),               ADRVAR(adr2, E1),       undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_TYPE,         ADRVAR(adr1, E2type),               ADRVAR(adr2, E2),       undef);                 ILInsertLast(L, inst, inWhile);
    
    // If types are equal, skip to typeeq; if one of them is float, skip to conversion; else exit
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "typeeq"),        ADRVAR(adr2, E1type),   ADRVAR(adr3, E2type));  ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "floatfirst"),    ADRVAR(adr2, E1type),   ADRSTR(adr3, "float")); ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "floatsecond"),   ADRVAR(adr2, E2type),   ADRSTR(adr3, "float")); ILInsertLast(L, inst, inWhile);

    // Exit 4
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "exit"),          undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_EXIT,         ADRINT(adr1, 4),                    undef,                  undef);                 ILInsertLast(L, inst, inWhile);

    // First operand is float, if second is integer, convert it to float and skip to operation, else exit
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "floatfirst"),    undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFNEQ,    ADRLAB(adr1, lab, "exit"),          ADRVAR(adr2, E2type),   ADRSTR(adr3, "int"));   ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_INT2FLOAT,    ADRVAR(adr1, E2),                   ADRVAR(adr2, E2),       undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "checkfloat"),           undef,                  undef);                 ILInsertLast(L, inst, inWhile);

    // Second operand is float, if first is integer, convert it to float and skip to operation, else exit
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "floatsecond"),   undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFNEQ,    ADRLAB(adr1, lab, "exit"),          ADRVAR(adr2, E1type),   ADRSTR(adr3, "int"));   ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_INT2FLOAT,    ADRVAR(adr1, E1),                   ADRVAR(adr2, E1),       undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "checkfloat"),           undef,                  undef);                 ILInsertLast(L, inst, inWhile);

    // Types are equal: if they are both float or integer, add them; if they are strings, cat them; else exit
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "typeeq"),        undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "checkint"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "int"));   ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "checkfloat"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "float")); ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_EXIT,         ADRINT(adr1, 4),                    undef,                  undef);                 ILInsertLast(L, inst, inWhile);

    // Before division check whether second operand is nonzero
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "checkint"),           undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_INT2FLOAT,    ADRVAR(adr1, E1),                   ADRVAR(adr2, E1),       undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_INT2FLOAT,    ADRVAR(adr1, E2),                   ADRVAR(adr2, E2),       undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFNEQ,     ADRLAB(adr1, lab, "opint"),           ADRVAR(adr2, E2),   ADRFLT(adr3, 0));   ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_EXIT,         ADRINT(adr1, 9),                    undef,                  undef);                 ILInsertLast(L, inst, inWhile);

    // Before division check whether second operand is nonzero
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "checkfloat"),           undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "exit"),           ADRVAR(adr2, E2),   ADRFLT(adr3, 0));   ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_DIV,          ADRVAR(adr1, E3),                   ADRVAR(adr2, E1),       ADRVAR(adr3, E2));      ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "end"),           undef,                  undef);                 ILInsertLast(L, inst, inWhile);
        
    // Divides variables that were previously integers, converts them back to int and finishes
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "opint"),           undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_DIV,          ADRVAR(adr1, E3),                   ADRVAR(adr2, E1),       ADRVAR(adr3, E2));      ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_FLOAT2INT,    ADRVAR(adr1, E3),                   ADRVAR(adr2, E3),       undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "end"),           undef,                  undef);                 ILInsertLast(L, inst, inWhile);

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

void genCmp(TOperation op, TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile){
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
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E3),                   undef,                  undef);                 ILPostActInsert(L, inst);
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E1type),               undef,                  undef);                 ILPostActInsert(L, inst);
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E2type),               undef,                  undef);                 ILPostActInsert(L, inst);

    // Get types of operands
    inst = getInst(OP_TYPE,         ADRVAR(adr1, E1type),               ADRVAR(adr2, E1),       undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_TYPE,         ADRVAR(adr1, E2type),               ADRVAR(adr2, E2),       undef);                 ILInsertLast(L, inst, inWhile);
    
    // If types are equal, skip to typeeq; if one of them is float, skip to conversion; else exit
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "typeeq"),        ADRVAR(adr2, E1type),   ADRVAR(adr3, E2type));  ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "floatfirst"),    ADRVAR(adr2, E1type),   ADRSTR(adr3, "float")); ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "floatsecond"),   ADRVAR(adr2, E2type),   ADRSTR(adr3, "float")); ILInsertLast(L, inst, inWhile);

    // Exit 4
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "exit"),          undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_EXIT,         ADRINT(adr1, 4),                    undef,                  undef);                 ILInsertLast(L, inst, inWhile);

    // First operand is float, if second is integer, convert it to float and skip to addition, else exit
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "floatfirst"),    undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFNEQ,    ADRLAB(adr1, lab, "exit"),          ADRVAR(adr2, E2type),   ADRSTR(adr3, "int"));   ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_INT2FLOAT,    ADRVAR(adr1, E2),                   ADRVAR(adr2, E2),       undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "cmp"),           undef,                  undef);                 ILInsertLast(L, inst, inWhile);

    // Second operand is float, if first is integer, convert it to float and skip to addition, else exit
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "floatsecond"),   undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFNEQ,    ADRLAB(adr1, lab, "exit"),          ADRVAR(adr2, E1type),   ADRSTR(adr3, "int"));   ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_INT2FLOAT,    ADRVAR(adr1, E1),                   ADRVAR(adr2, E1),       undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "cmp"),           undef,                  undef);                 ILInsertLast(L, inst, inWhile);

    // Types are equal: if they are both float or integer or strings, cmp them; else exit
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "typeeq"),        undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "cmp"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "int"));   ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "cmp"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "float")); ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "cmp"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "string"));ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_EXIT,         ADRINT(adr1, 4),                    undef,                  undef);                 ILInsertLast(L, inst, inWhile);

    // Compares variables and finishes
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "cmp"),           undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    switch (op){
        case OP_LT:
            inst = getInst(OP_LT,   ADRVAR(adr1, E3),                   ADRVAR(adr2, E1),       ADRVAR(adr3, E2));      ILInsertLast(L, inst, inWhile);
            break;
        case OP_GT:
            inst = getInst(OP_GT,   ADRVAR(adr1, E3),                   ADRVAR(adr2, E1),       ADRVAR(adr3, E2));      ILInsertLast(L, inst, inWhile);
            break;
        case OP_LEQ:
            inst = getInst(OP_GT,   ADRVAR(adr1, E3),                   ADRVAR(adr2, E1),       ADRVAR(adr3, E2));      ILInsertLast(L, inst, inWhile);
            inst = getInst(OP_NOT,  ADRVAR(adr1, E3),                   ADRVAR(adr2, E3),       undef);                 ILInsertLast(L, inst, inWhile);
            break;
        case OP_GEQ:
            inst = getInst(OP_LT,   ADRVAR(adr1, E3),                   ADRVAR(adr2, E1),       ADRVAR(adr3, E2));      ILInsertLast(L, inst, inWhile);
            inst = getInst(OP_NOT,  ADRVAR(adr1, E3),                   ADRVAR(adr2, E3),       undef);                 ILInsertLast(L, inst, inWhile);
            break;
    }

    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "end"),           undef,                  undef);                 ILInsertLast(L, inst, inWhile);

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

void genLT(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile){
    genCmp(OP_LT, L, psa, res, var1, var2, inWhile);
}
void genGT(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile){
    genCmp(OP_GT, L, psa, res, var1, var2, inWhile);
}
void genLEQ(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile){
    genCmp(OP_LEQ, L, psa, res, var1, var2, inWhile);
}
void genGEQ(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile){
    genCmp(OP_GEQ, L, psa, res, var1, var2, inWhile);
}

void genEQorNEQ(TOperation op, TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile){
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
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E3),                   undef,                  undef);                 ILPostActInsert(L, inst);
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E1type),               undef,                  undef);                 ILPostActInsert(L, inst);
    inst = getInst(OP_DEFVAR,       ADRVAR(adr1, E2type),               undef,                  undef);                 ILPostActInsert(L, inst);

    // Get types of operands
    inst = getInst(OP_TYPE,         ADRVAR(adr1, E1type),               ADRVAR(adr2, E1),       undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_TYPE,         ADRVAR(adr1, E2type),               ADRVAR(adr2, E2),       undef);                 ILInsertLast(L, inst, inWhile);
    
    // If types are equal, skip to typeeq; if one of them is float, skip to conversion; else exit
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "typeeq"),        ADRVAR(adr2, E1type),   ADRVAR(adr3, E2type));  ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "floatfirst"),    ADRVAR(adr2, E1type),   ADRSTR(adr3, "float")); ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "floatsecond"),   ADRVAR(adr2, E2type),   ADRSTR(adr3, "float")); ILInsertLast(L, inst, inWhile);

    // Different types => always false
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "false"),          undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    //inst = getInst(OP_EXIT,         ADRINT(adr1, 4),                    undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_MOVE,         ADRVAR(adr1, E3),                   ADRBOOL(adr2, "true"),  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "end"),           undef,                  undef);                 ILInsertLast(L, inst, inWhile);

    // First operand is float, if second is integer, convert it to float and skip to addition, else false
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "floatfirst"),    undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFNEQ,    ADRLAB(adr1, lab, "false"),          ADRVAR(adr2, E2type),   ADRSTR(adr3, "int"));   ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_INT2FLOAT,    ADRVAR(adr1, E2),                   ADRVAR(adr2, E2),       undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "cmp"),           undef,                  undef);                 ILInsertLast(L, inst, inWhile);

    // Second operand is float, if first is integer, convert it to float and skip to addition, else false
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "floatsecond"),   undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFNEQ,    ADRLAB(adr1, lab, "false"),          ADRVAR(adr2, E1type),   ADRSTR(adr3, "int"));   ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_INT2FLOAT,    ADRVAR(adr1, E1),                   ADRVAR(adr2, E1),       undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "cmp"),           undef,                  undef);                 ILInsertLast(L, inst, inWhile);

    // Types are equal: if they are both float or integer or strings, cmp them; else false
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "typeeq"),        undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "cmp"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "int"));   ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "cmp"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "float")); ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMPIFEQ,     ADRLAB(adr1, lab, "cmp"),           ADRVAR(adr2, E1type),   ADRSTR(adr3, "string"));ILInsertLast(L, inst, inWhile);
    
    // Both operands are nil => true
    inst = getInst(OP_MOVE,         ADRVAR(adr1, E3),                   ADRBOOL(adr2, "true"),  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_JUMP,         ADRLAB(adr1, lab, "end"),           undef,                  undef);                 ILInsertLast(L, inst, inWhile);

    // Compares variables and finishes
    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "cmp"),           undef,                  undef);                 ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_EQ,   ADRVAR(adr1, E3),                   ADRVAR(adr2, E1),       ADRVAR(adr3, E2));      ILInsertLast(L, inst, inWhile);

    inst = getInst(OP_LABEL,        ADRLAB(adr1, lab, "end"),           undef,                  undef);                 ILInsertLast(L, inst, inWhile);

    if (op == OP_NEQ){
        inst = getInst(OP_NOT,  ADRVAR(adr1, E3),                   ADRVAR(adr2, E3),       undef);                 ILInsertLast(L, inst, inWhile);
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

void genEQ(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile){
    genEQorNEQ(OP_EQ, L, psa, res, var1, var2, inWhile);
}
void genNEQ(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile){
    genEQorNEQ(OP_NEQ, L, psa, res, var1, var2, inWhile);
}
/*
int main(){
    TInstrList L;
    ILInit(&L);
    genPrgBegin(&L);
    /*genAdd(&L, 5, 3, 1, 2, true);
    genDefVar(&L, "yoo", true);
    genFunCallBegin(&L, "inputs", true);
    
    /*TAdr var;
    var.type = ADRTYPE_INT;
    var.val.i = 2;
    genFunCallPar(&L, "print", 1, var, true);
    
    var.type = ADRTYPE_STRING;
    var.val.s = "ahoj svete\n :)";
    genFunCallPar(&L, "print", 2, var, true);
    
    var.type = ADRTYPE_FLOAT;
    var.val.f = 3.14;
    genFunCallPar(&L, "print", 3, var, true);

    var.type = ADRTYPE_NIL;
    genFunCallPar(&L, "print", 3, var, true);

    var.type = ADRTYPE_VAR;
    var.val.s = "variable";
    genFunCallPar(&L, "print", 4, var, true);*/
    
    //genFunCallEnd(&L, "inputs", true);
    
    /*genWhileBegin(&L, 6, false);
    genWhileCond(&L, 6, 5, 8, false);
    genWhileEnd(&L, 6, false);

    genE(&L, 10, 1, var, true);
    genDefVar(&L, "abc", true); 
    genAssign(&L, "bca", 11, 60, true);*/

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
    /*genPrgEnd(&L);
    ILPrintAllInst(L);
    ILDisposeList(&L);
}*/
