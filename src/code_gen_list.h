/*
 * file name:       code_gen_list.h
 * project:         VUT-FIT-IFJ-2018
 * created:         1.12.2018
 * last modified:   1.12.2018
 * 
 * created by:      Jan Havlín xhavli47@stud.fit.vutbr.cz 
 * modification:
 * 
 * description: Functions and datatypes for working with a doubly linked list of instructions. 
 */

#ifndef CODE_GEN_LIST_H
#define CODE_GEN_LIST_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include "ifj_error.h"

#define FUN_LENGTH "LABEL $length\n\
DEFVAR LF@&1type\n\
TYPE LF@&1type LF@&1\n\
JUMPIFEQ $lengthcmp LF@&1type string@string\n\
EXIT int@4\n\
LABEL $lengthcmp\n\
STRLEN LF@&RETVAL LF@&1\n\
RETURN\n"

#define FUN_ORD "LABEL $ord\n\
DEFVAR LF@&1type\n\
DEFVAR LF@&1len\n\
TYPE LF@&1type LF@&1\n\
JUMPIFEQ $ordlengthcmp LF@&1type string@string\n\
EXIT int@4\n\
LABEL $ordlengthcmp\n\
STRLEN LF@&1len LF@&1\n\
\
DEFVAR LF@&2lencheck\n\
LT LF@&2lencheck LF@&2 int@0\n\
JUMPIFEQ $ordoutbounds LF@&2lencheck bool@true\n\
LT LF@&2lencheck LF@&2 LF@&1len\n\
JUMPIFEQ $ordoutbounds LF@&2lencheck bool@false\n\
\
WRITE string@secondaswell\n\
STRI2INT LF@&RETVAL LF@&1 LF@&2\n\
RETURN\n\
LABEL $ordoutbounds\n\
MOVE LF@&RETVAL nil@nil\n\
RETURN\n"

#define ADRCAT(adr, var1, var2) (adr.val.s = getStr(2, var1, var2), adr)

#define ADRNIL(adr)      (adr.type = ADRTYPE_NIL, adr)

                                // Usage of comma operator like this: (A,(B,C)) => A, B - execution of commands; C - parameter to a function
#define ADRINT(adr, var) (  (   adr.val.i = var,                        \
                                (adr.type = ADRTYPE_INT, adr)   )  )

#define ADRFLT(adr, var) (  (   adr.val.f = var,                        \
                                (adr.type = ADRTYPE_FLOAT, adr)   )  )

#define ADRSTR(adr, var) (  (   adr.val.s = getIfjCodeStr(var),             \
                                (adr.type = ADRTYPE_STRING, adr)   )  )

#define ADRBOOL(adr, var) (  (   adr.val.s = getStr(1, var),             \
                                (adr.type = ADRTYPE_BOOL, adr)   )  )

#define ADRVAR(adr, var) (  (   adr.val.s = getStr(1, var),             \
                                (adr.type = ADRTYPE_VAR, adr)   )  )

#define ADRVARTMP(adr, var) (  (   adr.val.s = getStr(1, var),             \
                                (adr.type = ADRTYPE_VAR_TMP, adr)   )  )

#define ADRLAB(adr, var1, var2) (  (   adr.val.s = getStr(2, var1, var2),             \
                                      (adr.type = ADRTYPE_LABEL, adr)   )  )

typedef enum {      // Adresses
    OP_DEFVAR,      // 1
    OP_MOVE,        // 2
    OP_TYPE,        // 2
    OP_JUMPIFEQ,    // 3
    OP_LABEL,       // 1
    OP_EXIT,        // 1
    OP_JUMPIFNEQ,   // 3
    OP_INT2FLOAT,   // 2
    OP_FLOAT2INT,   // 2
    OP_JUMP,        // 1
    OP_CONCAT,      // 3
    OP_PUSHFRAME,   // 0
    OP_CREATEFRAME, // 0
    OP_POPFRAME,    // 0
    OP_CALL,        // 1
    OP_ADD,         // 3
    OP_SUB,         // 3
    OP_MUL,         // 3
    OP_DIV,         // 3
    OP_LT,          // 3
    OP_GT,          // 3
    OP_LEQ,         // 3
    OP_GEQ,         // 3
    OP_EQ,          // 3
    OP_NEQ,         // 3
    OP_NOT,         // 2 TODO: Add NOT to .c
    OP_WRITE,       // 1
    OP_READ         // 1
} TOperation;

typedef enum {
    ADRTYPE_VAR,
    ADRTYPE_VAR_TMP,
    ADRTYPE_INT,
    ADRTYPE_FLOAT,
    ADRTYPE_STRING,
    ADRTYPE_BOOL,
    ADRTYPE_NIL,
    ADRTYPE_LABEL
} TAdrVal;

typedef struct {
    TAdrVal type;
    union {
        int i;
        double f;
        char *s;
    } val;
} TAdr;

typedef struct {
    TOperation op;
    TAdr adr1;
    TAdr adr2;
    TAdr adr3;
} TInst;

typedef struct TILElem {
    TInst inst;
    struct TILElem *lptr;
    struct TILElem *rptr;
} *TILElemPtr;

typedef struct {
    TILElemPtr First;
    TILElemPtr Act;
    TILElemPtr ActFun;
    TILElemPtr Last;
} TInstrList;


void ILInit (TInstrList *L);
void ILSetActLast (TInstrList *L);
void ILSetActFunFirst (TInstrList *L);
void ILDisposeList(TInstrList *L);
void ILInsertLast(TInstrList *L, TInst inst, bool inWhile);
void ILInsertFirst (TInstrList *L, TInst inst);
void ILPostActInsert (TInstrList *L, TInst inst);
void ILPreActFunInsert (TInstrList *L, TInst inst);
void ILPrintAllInst(TInstrList L);
char *getStr(int n, ...);
char *getIfjCodeStr(char *s);
TInst getInst(TOperation op, TAdr adr1, TAdr adr2, TAdr adr3);

#endif
