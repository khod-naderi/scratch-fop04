

#pragma once

#include "block.h"
#include "value.h"
#include "execution_context.h"

Value execMoveSteps(ExecutionContext &ctx, Value inputs[], int inputCount);
Value execTurnAngleDegree(ExecutionContext &ctx, Value inputs[], int inputCount);

extern Block blocksLibrary[];
extern int blocksLibraryCount;

#define BLOCKS_COUNT blocksLibraryCount // for backward compatibaly