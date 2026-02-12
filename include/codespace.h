

#pragma once
#include "block_instance.h"

#define MAX_INSTANCES 1000

struct CodeSpace
{
    BlockInstance instances[MAX_INSTANCES];
    bool instanceUsed[MAX_INSTANCES];
    int nextInstanceId;
};

void codespaceInit(CodeSpace &cs);