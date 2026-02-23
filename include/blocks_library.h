

#pragma once

#include "block.h"
#include "value.h"
#include "execution_context.h"

extern Block blocksLibrary[];
extern int blocksLibraryCount;
int doBounceWall(ExecutionContext &ctx);

#define BLOCKS_COUNT blocksLibraryCount // for backward compatibaly