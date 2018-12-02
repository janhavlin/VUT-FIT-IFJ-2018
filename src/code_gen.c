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
    TAdr adr1, undef;
    inst = getInst(OP_LABEL, ADRLAB(adr1, "main", ""), undef, undef); ILInsertLast(L, inst, false);
    inst = getInst(OP_CREATEFRAME, undef, undef, undef); ILInsertLast(L, inst, false);
    inst = getInst(OP_PUSHFRAME, undef, undef, undef); ILInsertLast(L, inst, false);
    ILSetActFunFirst(L);
}

void genPrgEnd(TInstrList *L){
    TInst inst;
    TAdr adr1, undef;
    inst = getInst(OP_JUMP, ADRLAB(adr1, "main", ""), undef, undef); ILInsertFirst(L, inst);
}

void genFunCallBegin(TInstrList *L, bool inWhile){
    TInst inst;
    TAdr undef;
    funPara = 0;
    inst = getInst(OP_CREATEFRAME, undef, undef, undef); ILInsertLast(L, inst, inWhile);
}

void genFunCallPar(TInstrList *L, unsigned parNum, TAdr var, bool inWhile){
    TInst inst;
    TAdr adr1, adr2, undef;
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

void genFunCallEnd(TInstrList *L, char *fun, bool inWhile){
    TInst inst;
    TAdr adr1, undef;
    inst = getInst(OP_PUSHFRAME,    undef,                  undef, undef); ILInsertLast(L, inst, inWhile);
    inst = getInst(OP_CALL,         ADRLAB(adr1, fun, ""),  undef, undef); ILInsertLast(L, inst, inWhile);
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

void genWhileCond(TInstrList *L, unsigned whileCnt, unsigned psa, unsigned res, bool inWhile){
    TInst inst;
    TAdr adr1, adr2, adr3, undef;
    char *whileStr = convIntToStr(whileCnt);
    char *psaStr = convIntToStr(psa);
    char *resStr = convIntToStr(res);
    char *EStr = getStr(4, "psa", psaStr, "E", resStr);
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

void genE(TInstrList *L, unsigned psa, unsigned res, TAdr var, bool inWhile){
    TInst inst;
    TAdr adr1, adr2, undef;
    char *psaStr = convIntToStr(psa);
    char *resEStr = convIntToStr(res);
    char *resVarStr = getStr(4, "psa", psaStr, "E", resEStr);
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

void genAssign(TInstrList *L, char *var, unsigned psa, unsigned E, bool inWhile){
    TInst inst;
    TAdr adr1, adr2, undef;
    printf("psa: %d, E: %d\n", psa, E);
    char *psaStr = convIntToStr(psa);
    char *EStr = convIntToStr(E);
    char *varStr = getStr(4, "psa", psaStr, "E", EStr);
    inst = getInst(OP_MOVE, ADRVAR(adr1, var), ADRVAR(adr2, varStr), undef); ILInsertLast(L, inst, inWhile);
    free(psaStr);
    free(EStr);
    free(varStr);
}
void genAdd(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile){
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

void genCmp(TOperation op, TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile){
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
        case OP_LE:
            inst = getInst(OP_GT,   ADRVAR(adr1, E3),                   ADRVAR(adr2, E1),       ADRVAR(adr3, E2));      ILInsertLast(L, inst, inWhile);
            inst = getInst(OP_NOT,  ADRVAR(adr1, E3),                   ADRVAR(adr1, E3),       undef);                 ILInsertLast(L, inst, inWhile);
            break;
        case OP_GE:
            inst = getInst(OP_LT,   ADRVAR(adr1, E3),                   ADRVAR(adr2, E1),       ADRVAR(adr3, E2));      ILInsertLast(L, inst, inWhile);
            inst = getInst(OP_NOT,  ADRVAR(adr1, E3),                   ADRVAR(adr1, E3),       undef);                 ILInsertLast(L, inst, inWhile);
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
void genLE(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile){
    genCmp(OP_LE, L, psa, res, var1, var2, inWhile);
}
void genGE(TInstrList *L, unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile){
    genCmp(OP_GE, L, psa, res, var1, var2, inWhile);
}

/*
int main(){
    TInstrList L;
    ILInit(&L);
    genPrgBegin(&L);
    genAdd(&L, 5, 3, 1, 2, true);
    genDefVar(&L, "yoo", true);
    genFunCallBegin(&L, true);
    
    TAdr var;
    var.type = ADRTYPE_INT;
    var.val.i = 2;
    genFunCallPar(&L, 1, var, true);
    
    var.type = ADRTYPE_STRING;
    var.val.s = "asd";
    genFunCallPar(&L, 2, var, true);
    
    var.type = ADRTYPE_FLOAT;
    var.val.f = 3.14;
    genFunCallPar(&L, 3, var, true);
    
    var.type = ADRTYPE_VAR;
    var.val.s = "variable";
    genFunCallPar(&L, 4, var, true);
    
    genFunCallEnd(&L, "foo", true);
    genWhileBegin(&L, 6, false);
    genWhileCond(&L, 6, 5, 8, false);
    genWhileEnd(&L, 6, false);

    genE(&L, 10, 1, var, true);
    genDefVar(&L, "abc", true); 
    genAssign(&L, "bca", 11, 60, true);

    genPrgEnd(&L);
    ILPrintAllInst(L);
    ILDisposeList(&L);
}*/