#include "execution_context.h"
#include <string>

/*
To create an empty execution context
*/
ExecutionContext::ExecutionContext()
{
    spriteX = 0;
    spriteY = 0;
    spriteAngle = 0;
    variableCount = 0;

    for (int i = 0; i < MAX_VARIABLES; i++)
    {
        variables[i].isUsed = false;
        variables[i].name = "";
    }
}

/*
To get a value from a variable in execution context
*/
Value *executionContextGetVariable(ExecutionContext &ctx, const char *name)
{
    for (int i = 0; i < MAX_VARIABLES; i++)
    {
        if (ctx.variables[i].isUsed && ctx.variables[i].name == name)
        {
            return &ctx.variables[i].value;
        }
    }
    return nullptr;
}

/*
To set a value to a variable in an execution context
*/
Value *executionContextSetVariable(ExecutionContext &ctx, const char *name, Value value)
{
    // find existing variable
    for (int i = 0; i < MAX_VARIABLES; i++)
    {
        if (ctx.variables[i].isUsed && ctx.variables[i].name == name)
        {
            ctx.variables[i].value = value;
            return &ctx.variables[i].value;
        }
    }

    // find empty variable space
    for (int i = 0; i < MAX_VARIABLES; i++)
    {
        if (!ctx.variables[i].isUsed)
        {
            ctx.variables[i].isUsed = true;
            ctx.variables[i].name = name;
            ctx.variables[i].value = value;
            ctx.variableCount++;

            return &ctx.variables[i].value;
        }
    }

    return nullptr;
}