#include "codespace.h"
#include "block_instance.h"
#include "block.h"
#include "blocks_library.h"
/*
To initialize an empty codespace
*/
void codespaceInit(CodeSpace &cs)
{
    cs.nextInstanceId = 0;
    for (int i = 0; i < MAX_INSTANCES; i++)
    {
        cs.instanceUsed[i] = false;
        blockInstanceConstruct(cs.instances[i]);
    }
}

int findSlotById(CodeSpace &cs, int id)
{
    for (int i = 0; i < MAX_INSTANCES; i++)
    {
        if (cs.instanceUsed[i] &&
            cs.instances[i].instanceId == id)
            return i;
    }
    return -1;
}

BlockInstance *codespaceGetInstance(CodeSpace &cs, int id)
{
    int slot = findSlotById(cs, id);
    if (slot == -1)
        return NULL;
    return &cs.instances[slot];
}

int codespaceCreateInstance(CodeSpace &cs, int definitionId, int posX, int posY)
{
    // find free slot
    int slot = -1;
    for (int i = 0; i < MAX_INSTANCES; i++)
    {
        if (!cs.instanceUsed[i])
        {
            slot = i;
            break;
        }
    }
    if (slot == -1)
        return -1;

    int id = cs.nextInstanceId++;
    cs.instanceUsed[slot] = true;
    blockInstanceConstructFromDefinition(cs.instances[slot], definitionId, id, posX, posY);
    return id;
}

void codespaceAttachNext(CodeSpace &cs, int topId, int bottomId)
{
    BlockInstance *top = codespaceGetInstance(cs, topId);
    BlockInstance *bottom = codespaceGetInstance(cs, bottomId);
    if (!top || !bottom)
        return;

    top->nextId = bottomId;
    bottom->parentId = topId;
    bottom->parentBodyIndex = -1;
}

void codespaceAttachToBody(CodeSpace &cs, int parentId,
                           int bodyIndex, int childId)
{
    BlockInstance *parent = codespaceGetInstance(cs, parentId);
    BlockInstance *child = codespaceGetInstance(cs, childId);
    if (!parent || !child)
        return;
    if (bodyIndex < 0 || bodyIndex >= parent->bodyCount)
        return;

    parent->bodies[bodyIndex] = childId;
    child->parentId = parentId;
    child->parentBodyIndex = bodyIndex;
}

void codespaceAttachToInput(CodeSpace &cs, int hostId,
                            int slotIndex, int operatorId)
{
    BlockInstance *host = codespaceGetInstance(cs, hostId);
    if (!host)
        return;
    if (slotIndex < 0 || slotIndex >= host->inputCount)
        return;

    host->inputs[slotIndex].isBlock = true;
    host->inputs[slotIndex].blockInstanceId = operatorId;

    BlockInstance *op = codespaceGetInstance(cs, operatorId);
    if (op)
    {
        op->parentId = hostId;
        op->parentBodyIndex = -1;
    }
}

void codespaceDetach(CodeSpace &cs, int id)
{
    BlockInstance *block = codespaceGetInstance(cs, id);
    if (!block)
        return;

    if (block->parentId != -1)
    {
        BlockInstance *parent = codespaceGetInstance(cs, block->parentId);
        if (parent)
        {
            // sequential connection
            if (parent->nextId == id)
                parent->nextId = -1;

            // body connection
            for (int i = 0; i < parent->bodyCount; i++)
            {
                if (parent->bodies[i] == id)
                    parent->bodies[i] = -1;
            }

            // input slot connection
            for (int i = 0; i < parent->inputCount; i++)
            {
                if (parent->inputs[i].isBlock &&
                    parent->inputs[i].blockInstanceId == id)
                {
                    Block *def = &blocksLibrary[parent->defenitionId];
                    parent->inputs[i].isBlock = false;
                    parent->inputs[i].literal =
                        def->slots[i].defaultValue;
                    parent->inputs[i].blockInstanceId = -1;
                }
            }
        }
    }

    block->parentId = -1;
    block->parentBodyIndex = -1;
}

void codespaceRemoveInstance(CodeSpace &cs, int id)
{
    codespaceDetach(cs, id);
    int slot = findSlotById(cs, id);
    if (slot != -1)
    {
        cs.instanceUsed[slot] = false;
        blockInstanceConstruct(cs.instances[slot]);
    }
}

int codespaceFindEventBlocks(CodeSpace &cs, int *outIds, int maxOut)
{
    int count = 0;
    for (int i = 0; i < MAX_INSTANCES && count < maxOut; i++)
    {
        if (!cs.instanceUsed[i])
            continue;

        BlockInstance *inst = &cs.instances[i];
        Block *def = &blocksLibrary[inst->defenitionId];

        if (def->type == BLOCK_EVENT && inst->parentId == -1)
        {
            outIds[count++] = inst->instanceId;
        }
    }
    return count;
}