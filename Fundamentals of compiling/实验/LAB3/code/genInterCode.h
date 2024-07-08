#ifndef INTER_H
#define INTER_H
#include "node.h"
#include "semantic.h"

typedef struct _operand* pOperand;
typedef struct _interCode* pInterCode;
typedef struct _interCodes* pInterCodes;
typedef struct _arg* pArg;
typedef struct _argList* pArgList;
typedef struct _interCodeList* pInterCodeList;
//操作数
typedef struct _operand {
    enum {
        //存储值value
        OP_CONSTANT, //常量
        //存储标识符name
        OP_VARIABLE, //变量
        OP_ADDRESS, //地址
        OP_LABEL, //标签，跳转目标或者标记特定的位置
        OP_FUNCTION,//函数
        OP_RELOP, //关系运算
    } kind;

    union {
        //值 or 标识符
        int value; 
        char* name;
    } u;

} Operand;

typedef struct _interCode {
    enum {
        //oneOp
        IR_LABEL, 
        IR_FUNCTION, 
        IR_IF_GOTO,//条件跳转
        IR_RETURN,
        IR_PARAM, //形参列表 ，被调用函数体内
        IR_ARG, //时参列表 ，调用函数体内
        IR_READ, 
        IR_WRITE,
        //assign
        IR_ASSIGN,
        IR_GET_ADDR,
        IR_READ_ADDR,
        IR_WRITE_ADDR,
        IR_CALL, //调用
        //binOp
        IR_ADD,
        IR_SUB,
        IR_MUL,
        IR_DIV,

        //ifGoto
        IR_GOTO, 

        //dec
        IR_DEC, //声明

    } kind;

    union {
        struct {
            pOperand op;
        } oneOp; //一目运算
        struct {
            pOperand right, left;
        } assign; //赋值运算
        struct {
            pOperand result, op1, op2;
        } binOp;//二目运算
        struct {
            pOperand x, relop, y, z;
        } ifGoto;//条件跳转
        struct { //DEC x [size] 内存空间申请，大小为4的倍数。
            pOperand op;
            int size;
        } dec; //数组声明语句
    } u;
} InterCode;

typedef struct _interCodes {
    pInterCode code;
    pInterCodes *prev, *next;
} InterCodes;//双向链表

typedef struct _arg {
    pOperand op;
    pArg next;
} Arg;
//typedef struct _arg* pArg;
typedef struct _argList {
    pArg head;
    pArg cur;
} ArgList;

typedef struct _interCodeList {
    pInterCodes head;
    pInterCodes cur;
    int tempVarNum;
    int labelNum;
} InterCodeList;

extern boolean interError;
extern pInterCodeList interCodeList;

// Operand func
pOperand newOperand(int kind, ...);
void deleteOperand(pOperand p);
void setOperand(pOperand p, int kind, void* val);
void printOp(FILE* fp, pOperand op);

// InterCode func
pInterCode newInterCode(int kind, ...);
void deleteInterCode(pInterCode p);
void printInterCode(FILE* fp, pInterCodeList interCodeList);

// InterCodes func
pInterCodes newInterCodes(pInterCode code);
void deleteInterCodes(pInterCodes p);

// Arg and ArgList func
pArg newArg(pOperand op);
pArgList newArgList();
void deleteArg(pArg p);
void deleteArgList(pArgList p);
void addArg(pArgList argList, pArg arg);

// InterCodeList func
pInterCodeList newInterCodeList();
void deleteInterCodeList(pInterCodeList p);
void addInterCode(pInterCodeList interCodeList, pInterCodes newCode);

// traverse func
pOperand newTemp();
pOperand newLabel();
int getSize(pType type);
void genInterCodes(pNode node);
void genInterCode(int kind, ...);
void translateExp(pNode node, pOperand place);
void translateArgs(pNode node, pArgList argList);
void translateCond(pNode node, pOperand labelTrue, pOperand labelFalse);
void translateVarDec(pNode node, pOperand place);
void translateDec(pNode node);
void translateDecList(pNode node);
void translateDef(pNode node);
void translateDefList(pNode node);
void translateCompSt(pNode node);
void translateStmt(pNode node);
void translateStmtList(pNode node);
void translateFunDec(pNode node);
void translateExtDef(pNode node);
void translateExtDefList(pNode node);
#endif