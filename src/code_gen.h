/*
 * file name:       code_gen.h
 * project:         VUT-FIT-IFJ-2018
 * created:         23.11.2018
 * last modified:   30.11.2018
 * 
 * created by:      Jan Havlín xhavli47@stud.fit.vutbr.cz 
 * modification:
 * 
 * description: Functions for converting specific block of code into three address code. 
 */

#ifndef CODE_GEN_H
#define CODE_GEN_H

#include <stdbool.h>



#define ADRCAT(adr, var1, var2) (adr.val.s = getStr(2, var1, var2), adr)

#define ADRNIL(adr)      (adr.type = ADRTYPE_NIL, adr)

                                // Usage of comma operator like this: (A,(B,C)) => A, B - execution of commands; C - parameter to a function
#define ADRINT(adr, var) (  (   adr.val.i = var,                        \
                                (adr.type = ADRTYPE_INT, adr)   )  )

#define ADRFLT(adr, var) (  (   adr.val.f = var,                        \
                                (adr.type = ADRTYPE_FLOAT, adr)   )  )

#define ADRSTR(adr, var) (  (   adr.val.s = getStr(1, var),             \
                                (adr.type = ADRTYPE_STRING, adr)   )  )

#define ADRBOOL(adr, var) (  (   adr.val.s = getStr(1, var),             \
                                (adr.type = ADRTYPE_BOOL, adr)   )  )

#define ADRVAR(adr, var) (  (   adr.val.s = getStr(1, var),             \
                                (adr.type = ADRTYPE_VAR, adr)   )  )

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
    OP_JUMP,        // 1
    OP_CONCAT,      // 3
    OP_ADD,         // 3
    OP_PUSHFRAME,   // 0
    OP_CREATEFRAME, // 0
    OP_CALL         // 1
} TOperation;

typedef enum {
    ADRTYPE_VAR,
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

typedef struct tDLElem {
    TInst inst;
    struct tDLElem *lptr;
    struct tDLElem *rptr;
} *tDLElemPtr;

typedef struct {
    tDLElemPtr First;
    tDLElemPtr Act;
    tDLElemPtr Last;
} tDLList;

/**
 * @brief Creates a frame for a new function call
 * 
 * @param inWhile Flag whether it's called from inside a while
 */
void genFunCallBegin(bool inWhile);

/**
 * @brief Defines a parameter of the function and moves it to temporary frame
 * 
 * @param var Name of the parameter
 * @param inWhile Flag whether it's called from inside a while
 */
void genFunCallPar(char *var, bool inWhile);

/**
 * @brief Pushes the temporary frame and calls the function
 * 
 * @param fun Name of the function
 * @param inWhile Flag whether it's called from inside a while
 */
void genFunCallEnd(char *fun, bool inWhile);

/**
 * @brief Creates 'begin' label of a while
 * 
 * @param whileCnt Index of current while
 * @param inWhile Flag whether it's called from inside a while
 */
void genWhileBegin(unsigned whileCnt, bool inWhile);

/**
 * @brief Decides whether to skip body of the while or not
 * 
 * @param whileCnt Index of current while
 * @param psa Index of PSA that evaluated condition 
 * @param res Index of temporary variable of PSA containing result of the PSA
 * @param inWhile Flag whether it's called from inside a while
 */
void genWhileCond(unsigned whileCnt, unsigned psa, unsigned res, bool inWhile);

/**
 * @brief Jumps to the beginning of the while and creates label for ending
 * 
 * @param whileCnt  Index of current while
 * @param inWhile Flag whether it's called from inside a while
 */
void genWhileEnd(unsigned whileCnt, bool inWhile);

/**
 * @brief Function for evaluating PSA 'E -> i' rule
 * 
 * @param psa Index of current PSA
 * @param res Index of 'E' to store the result
 * @TODO: support constants (needs union)
 * @param var Name of the variable to be converted to E
 * @param inWhile Flag whether it's called from inside a while
 */
void genE(unsigned psa, unsigned res, char *var, bool inWhile);

/**
 * @brief Function for evaluating PSA 'E -> E + E' rule
 * 
 * @param psa Index of current PSA
 * @param res Index of 'E' to store the result
 * @param var1 Index of the first 'E' operand
 * @param var2 Index of the second 'E' operand
 * @param inWhile Flag whether it's called from inside a while
 */
void genAdd(unsigned psa, unsigned res, unsigned var1, unsigned var2, bool inWhile);

/**
 * @brief Defines a new variable and assigns nil to it
 * 
 * @param var Name of the variable
 * @param inWhile Flag whether it's called from inside a while
 */
void genDefVar(char *var, bool inWhile);

#endif  // CODE_GEN_H
