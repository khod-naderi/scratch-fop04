#include "block_instance.h"
#include "blocks_library.h"
#include "ui.h"

/*
-------------------------------
This function is for creating an empty block instance
-------------------------------
*/
void blockInstanceConstruct(BlockInstance &inst)
{
    inst.instanceId = -1;
    inst.defenitionId = -1;
    inst.posX = 0;
    inst.posY = 0;
    inst.inputCount = 0;
    inst.parentId = -1;
    inst.nextId = -1;
    inst.bodyCount = 0;
    inst.parentBodyIndex = -1;
    inst.cachedHeight = 0;
    inst.cachedWidth = 0;

    for (int i = 0; i < BLOCK_MAX_SLOTS; i++)
    {
        inst.inputs[i].isBlock = false;
        inst.inputs[i].blockInstanceId = -1;
        inst.textboxes[i] = new TextBox(DEFAULT_FONT);
    }

    for (int i = 0; i < BLOCK_MAX_BODIES; i++)
    {
        inst.bodies[i] = -1;
    }
}

void blockInstanceConstructFromDefinition(BlockInstance &inst, int defId, int instanceId, int posX, int posY)
{
    blockInstanceConstruct(inst);

    inst.instanceId = instanceId;
    inst.defenitionId = defId;
    inst.posX = posX;
    inst.posY = posY;

    Block def = blocksLibrary[defId];

    // copy default values from defenition slots
    inst.inputCount = def.slotCount;
    for (int i = 0; i < def.slotCount; i++)
    {
        inst.inputs[i].isBlock = false;
        inst.inputs[i].literal = def.slots[i].defaultValue;
        inst.inputs[i].blockInstanceId = -1;
        inst.textboxes[i] = new TextBox(DEFAULT_FONT);
        inst.textboxes[i]->setStr(inst.inputs[i].literal.asString());
    }

    // copy bodies info
    inst.bodyCount = def.bodyCount;
    for (int i = 0; i < def.bodyCount; i++)
    {
        inst.bodies[i] = -1;
    }

    // copy dimensions from definition
    inst.cachedHeight = def.baseHeight;
    inst.cachedWidth = def.baseWidth;
}