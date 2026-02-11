/*
For handling execution context for each block
witch sprint, witch position, etc.
*/

#ifndef EXECUTION_CONTEX_HEADER
#define EXECUTION_CONTEX_HEADER

#include "value.h"

#define MAX_VARIABLES 64

struct Variable
{
    char name[64];
    Value value;
    bool isUsed;
};

struct ExecutionContext
{
    // sprite state
    int spriteX;
    int spriteY;
    double spriteAngle;

    // user variables
    Variable variables[MAX_VARIABLES];
    int variableCount;
};

#endif