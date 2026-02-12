

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

int codespaceCreateInstance(CodeSpace &cs, int definitionId, int posX, int posY);
BlockInstance *codespaceGetInstance(CodeSpace &cs, int id);
void codespaceAttachNext(CodeSpace &cs, int topId, int bottomId);
void codespaceAttachToBody(CodeSpace &cs, int parentId, int bodyIndex, int childId);
void codespaceAttachToInput(CodeSpace &cs, int hostId, int slotIndex, int operatorId);
void codespaceDetach(CodeSpace &cs, int id);
void codespaceRemoveInstance(CodeSpace &cs, int id);
int codespaceFindEventBlocks(CodeSpace &cs, int *outIds, int maxOut);