/*
For handling execution context for each block
witch sprint, witch position, variables, stop intrupt and etc.
*/

#ifndef EXECUTION_CONTEX_HEADER
#define EXECUTION_CONTEX_HEADER

#include "value.h"
#include <string>
#include "spritebox.h"

#define MAX_VARIABLES 64

struct Variable
{
    std::string name;
    Value value;
    bool isUsed;
};

struct ExecutionContext
{
    // sprite pointer
    SprintBody *sprite;

    // user variables
    Variable variables[MAX_VARIABLES];
    int variableCount;

    ExecutionContext();
};

Value *executionContextGetVariable(ExecutionContext &ctx, const char *name);
Value *executionContextSetVariable(ExecutionContext &ctx, const char *name, Value value);

#endif