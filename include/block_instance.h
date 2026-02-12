

#ifndef BLOCK_INSTANCE_HEADER
#define BLOCK_INSTANCE_HEADER

#include "value.h"
#include "block.h"

/*
for handling input value in block's slots
*/
struct InputValue
{
    bool isBlock;
    Value literal;
    int blockInstanceId;
};

struct BlockInstance
{
    int instanceId;
    int defenitionId;

    // position - only meaningful for root blocks (this cordinates are relative to workspace screen)
    int posX, posY;

    // input values - one per slots
    InputValue inputs[BLOCK_MAX_SLOTS];
    int inputCount;

    // squeancial connection
    int parentId;
    int nextId;

    // nesting - bodies[i] = instanceId of first block in body i
    int bodies[BLOCK_MAX_BODIES];
    int bodyCount;

    // -1 = squential logic, 0> = first child in body i of parent
    int parentBodyIndex;

    // cached rendering
    int cachedWidth, cachedHeight;
};

void blockInstanceConstruct(BlockInstance &inst);
void blockInstanceConstructFromDefinition(BlockInstance &inst, int defId, int instanceId, int posX, int posY);

#endif