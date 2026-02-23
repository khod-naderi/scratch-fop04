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

std::string lprintf(const char *format, const InputValue values[])
{
    std::string out = "";
    int j = 0;
    for (size_t i = 0; i < strlen(format); i++)
    {
        if (format[i] == '%')
        {
            i++;
            if (format[i] == 's')
            {
                if (values[j].isBlock)
                {
                    // For blocks, we show a placeholder or the block's representation
                    // The actual value will be computed at execution time
                    out += "[block]";
                }
                else
                {
                    out += values[j].literal.asString();
                }
                j++;
                continue;
            }
        }
        out.push_back(format[i]);
    }
    return out;
}

/*
This function will update input value of a blockinstace with it's textboxes.
*/
void updateBlockInstanceInputValues(BlockInstance &inst)
{
    for (int i = 0; i < inst.inputCount; i++)
    {
        if (inst.inputs[i].isBlock)
        {
            // TODO: Block connection inputs
        }
        else
        {
            inst.inputs[i].literal = fromString(inst.textboxes[i]->getStr());
        }
    }
}