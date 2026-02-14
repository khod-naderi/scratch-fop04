/*
-------------------------------------------
This CPP file is for managing code view named workspace
-------------------------------------------
*/
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "workspace.h"
#include "color.h"
#include "blocks_library.h"
#include "ui.h"
#include "block.h"
#include "codespace.h"
#include "block_instance.h"
#include <cmath>
#include <iostream>
#include <vector>

CodeSpace workspaceCodeSpace;

// moving items properties
bool isMovingItem = false;
int movingInstanceId = -1;
std::vector<int> movingChainIds;

// Scroll workspace properties
bool isScrollingWorkspace = false;
int mouseX0, mouseY0;
int scrollOffsetX = 0;
int scrollOffsetY = 0;

/*
This functions will calulate width of static labels
used to find correct position of textboxes
*/
#define INLINE_LABEL_TEXBOX_MARGIN 2
std::vector<int> calcLabelSizeByPart(const char *label);
std::vector<int> calcLabelSizeByPartCamulative(const char *label);

/*
Helper function to calculate dynamic body height based on content
*/
int calculateBodyHeight(const BlockInstance &inst, int bodyIndex);

/*
Helper function to update cached height including body heights
*/
void updateBlockInstanceCachedHeight(BlockInstance &inst);

/*
-------------------------------
This function check if any BlockInstance that can have bottom connection is near the top of this item.
-------------------------------
@input:
TL_X: x position of top left corner of this item (related to workspace cordinate).
TL_Y: y position of top left corner of this item (related to workspace cordinate).
@output:
if yes, it will return instanceId of that item,
if not, it will return -1
*/
int isItemNearTopToConnect(const int TL_X, const int TL_Y)
{
    for (int i = 0; i < MAX_INSTANCES; i++)
    {
        if (!workspaceCodeSpace.instanceUsed[i])
            continue;

        BlockInstance inst = workspaceCodeSpace.instances[i];
        Block def = blocksLibrary[inst.defenitionId];

        // Skip if this block already have a next connection or can't have bottom connection
        if (inst.nextId != -1 || !def.canHaveBottomConnection)
            continue;

        // Calculate total height including bodies
        int totalHeight = inst.cachedHeight;
        for (int bodyIdx = 0; bodyIdx < inst.bodyCount; bodyIdx++)
        {
            totalHeight += calculateBodyHeight(inst, bodyIdx) + 15; // Add body height + bottom margin
        }

        // Connection point is at the bottom-center of the target block
        const int blc_x = inst.posX + inst.cachedWidth / 2; // Center horizontally
        const int blc_y = inst.posY + totalHeight;
        if (std::sqrt(std::pow((blc_x - TL_X), 2) + std::pow((blc_y - TL_Y), 2)) <= CONNECTION_MINIMUM_DISTANCE)
        {
            return inst.instanceId;
        }
    }

    return -1;
}

/*
Helper function to calculate dynamic body height based on content
*/
int calculateBodyHeight(const BlockInstance &inst, int bodyIndex)
{
    if (bodyIndex < 0 || bodyIndex >= inst.bodyCount)
        return 60; // Default minimum height

    int bodyContentHeight = 0;
    if (inst.bodies[bodyIndex] != -1)
    {
        // Calculate height of all blocks in this body
        int currentBlockId = inst.bodies[bodyIndex];
        while (currentBlockId != -1)
        {
            BlockInstance *bodyBlock = findBlockInstanceById(currentBlockId);
            if (!bodyBlock)
                break;

            Block bodyBlockDef = blocksLibrary[bodyBlock->defenitionId];
            bodyContentHeight += bodyBlockDef.baseHeight;

            currentBlockId = bodyBlock->nextId;
        }
    }

    // Minimum body height when empty, with padding for content
    return std::max(bodyContentHeight + 20, 60); // 20px padding, minimum 60px
}

void updateBlockInstanceCachedHeight(BlockInstance &inst)
{
    Block def = blocksLibrary[inst.defenitionId];
    // Only use base height for the block head, not including bodies
    inst.cachedHeight = def.baseHeight;
}

/*
-------------------------------
This function checks if any BlockInstance with bodies is near the given position
and can accept a child block in one of its bodies.
-------------------------------
@input:
TL_X: x position of top left corner of the dragged item (relative to workspace coordinate).
TL_Y: y position of top left corner of the dragged item (relative to workspace coordinate).
outBodyIndex: pointer to store which body index is near
@output:
if yes, it will return instanceId of that item and set outBodyIndex,
if not, it will return -1
*/
int isItemNearBodyToConnect(const int TL_X, const int TL_Y, int &outBodyIndex)
{
    for (int i = 0; i < MAX_INSTANCES; i++)
    {
        if (!workspaceCodeSpace.instanceUsed[i])
            continue;

        BlockInstance inst = workspaceCodeSpace.instances[i];
        Block def = blocksLibrary[inst.defenitionId];

        // Skip if this block has no bodies
        if (def.bodyCount == 0)
            continue;

        // Check each body area
        for (int bodyIdx = 0; bodyIdx < def.bodyCount; bodyIdx++)
        {
            if (isPointInBodyArea(TL_X, TL_Y, inst, bodyIdx))
            {
                outBodyIndex = bodyIdx;
                return inst.instanceId;
            }
        }
    }

    return -1;
}

/*
-------------------------------
This function checks if a point is inside a specific body area of a block instance
-------------------------------
*/
bool isPointInBodyArea(const int pointX, const int pointY, const BlockInstance &inst, const int bodyIndex)
{
    Block def = blocksLibrary[inst.defenitionId];

    if (bodyIndex < 0 || bodyIndex >= def.bodyCount)
        return false;

    // Calculate body area bounds
    // Body areas start after the main block and are stacked vertically
    int bodyStartY = inst.posY + inst.cachedHeight;

    // Calculate accumulated height of previous bodies using dynamic heights
    int accumulatedHeight = 0;
    for (int i = 0; i < bodyIndex; i++)
    {
        accumulatedHeight += calculateBodyHeight(inst, i) + 15; // Add bottom margin
    }

    int bodyHeight = calculateBodyHeight(inst, bodyIndex);
    int bodyY = bodyStartY + accumulatedHeight;

    // Body area rectangle - make it more accessible
    int bodyLeft = inst.posX + 15;                     // Margin from left edge for indentation
    int bodyRight = inst.posX + inst.cachedWidth - 15; // Use cached width
    int bodyTop = bodyY + 5;                           // Small margin from top
    int bodyBottom = bodyY + bodyHeight - 5;           // Small margin from bottom

    return (pointX >= bodyLeft && pointX <= bodyRight &&
            pointY >= bodyTop && pointY <= bodyBottom);
}

/*
-------------------------------
This function checks if any BlockInstance with input slots is near the given position
and can accept an expression block in one of its input slots.
-------------------------------
@input:
TL_X: x position of top left corner of the dragged item (relative to workspace coordinate).
TL_Y: y position of top left corner of the dragged item (relative to workspace coordinate).
outSlotIndex: reference to store which slot index is near
@output:
if yes, it will return instanceId of that item and set outSlotIndex,
if not, it will return -1
*/
int isItemNearInputToConnect(const int TL_X, const int TL_Y, int &outSlotIndex)
{
    for (int i = 0; i < MAX_INSTANCES; i++)
    {
        if (!workspaceCodeSpace.instanceUsed[i])
            continue;

        BlockInstance inst = workspaceCodeSpace.instances[i];
        Block def = blocksLibrary[inst.defenitionId];

        // Skip if this block has no input slots
        if (def.slotCount == 0)
            continue;

        // Check each input slot area
        for (int slotIdx = 0; slotIdx < def.slotCount; slotIdx++)
        {
            // Skip if slot is already occupied by a block
            if (inst.inputs[slotIdx].isBlock)
                continue;

            if (isPointInInputSlot(TL_X, TL_Y, inst, slotIdx))
            {
                outSlotIndex = slotIdx;
                return inst.instanceId;
            }
        }
    }

    return -1;
}

/*
-------------------------------
This function checks if a point is inside a specific input slot area of a block instance
-------------------------------
*/
bool isPointInInputSlot(const int pointX, const int pointY, const BlockInstance &inst, const int slotIndex)
{
    Block def = blocksLibrary[inst.defenitionId];

    if (slotIndex < 0 || slotIndex >= def.slotCount)
        return false;

    // Calculate input slot position based on textbox positions
    // This uses same logic as the textbox positioning in drawWorkspaceScreen
    std::vector<int> wst = calcLabelSizeByPart(def.label);
    int totalWidth = 0;
    for (int i = 0; i < slotIndex; i++)
    {
        totalWidth += wst[i];
        if (i < inst.inputCount && inst.textboxes[i])
            totalWidth += inst.textboxes[i]->cachedWidth;
    }
    totalWidth += wst[slotIndex];

    // Input slot area rectangle (around where the textbox would be)
    int slotLeft = inst.posX + totalWidth + BLOCKINSTANCE_RL_MARGIN / 2;
    int slotWidth = (inst.textboxes[slotIndex] ? inst.textboxes[slotIndex]->cachedWidth : 50);
    int slotRight = slotLeft + slotWidth;
    int slotTop = inst.posY + 5;                        // Small margin from top
    int slotBottom = inst.posY + inst.cachedHeight - 5; // Small margin from bottom

    return (pointX >= slotLeft && pointX <= slotRight &&
            pointY >= slotTop && pointY <= slotBottom);
}

/*
-------------------------------
Connection validation helpers
-------------------------------
*/
bool canConnectSequentially(int topId, int bottomId)
{
    BlockInstance *topInst = codespaceGetInstance(workspaceCodeSpace, topId);
    BlockInstance *bottomInst = codespaceGetInstance(workspaceCodeSpace, bottomId);

    if (!topInst || !bottomInst)
        return false;

    Block topDef = blocksLibrary[topInst->defenitionId];
    Block bottomDef = blocksLibrary[bottomInst->defenitionId];

    // Top block must be able to have bottom connection
    if (!topDef.canHaveBottomConnection)
        return false;

    // Bottom block must be able to have top connection
    if (!bottomDef.canHaveTopConnection)
        return false;

    // Top block must not already have a next connection
    if (topInst->nextId != -1)
        return false;

    // Bottom block must not already have a parent
    if (bottomInst->parentId != -1)
        return false;

    return true;
}

bool canConnectToBody(int parentId, int bodyIndex, int childId)
{
    BlockInstance *parentInst = codespaceGetInstance(workspaceCodeSpace, parentId);
    BlockInstance *childInst = codespaceGetInstance(workspaceCodeSpace, childId);

    if (!parentInst || !childInst)
        return false;

    Block parentDef = blocksLibrary[parentInst->defenitionId];
    Block childDef = blocksLibrary[childInst->defenitionId];

    // Parent must have bodies
    if (parentDef.bodyCount == 0)
        return false;

    // Body index must be valid
    if (bodyIndex < 0 || bodyIndex >= parentDef.bodyCount)
        return false;

    // Body must not already have a child (for now, assuming single child per body)
    if (parentInst->bodies[bodyIndex] != -1)
        return false;

    // Child must not already have a parent
    if (childInst->parentId != -1)
        return false;

    // Child must can have top connection
    if (!childDef.canHaveTopConnection)
        return false;

    return true;
}

bool canConnectToInput(int hostId, int slotIndex, int operatorId)
{
    BlockInstance *hostInst = codespaceGetInstance(workspaceCodeSpace, hostId);
    BlockInstance *operatorInst = codespaceGetInstance(workspaceCodeSpace, operatorId);

    if (!hostInst || !operatorInst)
        return false;

    Block hostDef = blocksLibrary[hostInst->defenitionId];
    Block operatorDef = blocksLibrary[operatorInst->defenitionId];

    // Host must have input slots
    if (hostDef.slotCount == 0)
        return false;

    // Slot index must be valid
    if (slotIndex < 0 || slotIndex >= hostDef.slotCount)
        return false;

    // Slot must not already be occupied by a block
    if (hostInst->inputs[slotIndex].isBlock)
        return false;

    // Operator must be an expression block (operator or reporter)
    if (operatorDef.type != BLOCK_OPERATOR && operatorDef.type != BLOCK_REPORTER)
        return false;

    // Operator must not already have a parent
    if (operatorInst->parentId != -1)
        return false;

    return true;
}

/*
-----------------------
This function will search for BlockInstance with specific id in new system
-----------------------
return pointer to BlockInstance or NULL if not found
*/
BlockInstance *findBlockInstanceById(int id)
{
    return codespaceGetInstance(workspaceCodeSpace, id);
}

/*
-----------------------
Recursively update positions of child blocks (bodies and inputs)
-----------------------
*/
void updateChildBlockPositions(BlockInstance &parent)
{
    // Update body children positions
    for (int bodyIdx = 0; bodyIdx < parent.bodyCount; bodyIdx++)
    {
        if (parent.bodies[bodyIdx] != -1)
        {
            int bodyStartY = parent.posY + parent.cachedHeight;
            int accumulatedHeight = 0;
            for (int i = 0; i < bodyIdx; i++)
            {
                accumulatedHeight += calculateBodyHeight(parent, i) + 15;
            }

            int currentBlockId = parent.bodies[bodyIdx];
            int yOffset = 10; // Initial margin from top
            while (currentBlockId != -1)
            {
                BlockInstance *bodyBlock = findBlockInstanceById(currentBlockId);
                if (!bodyBlock)
                    break;

                bodyBlock->posX = parent.posX + 20; // Indent
                bodyBlock->posY = bodyStartY + accumulatedHeight + yOffset;

                Block bodyBlockDef = blocksLibrary[bodyBlock->defenitionId];
                // yOffset += bodyBlockDef.baseHeight;
                if (bodyBlock->bodyCount == 0)
                {
                    yOffset += bodyBlock->cachedHeight;
                }
                else
                {
                    int totalBodyHeight = bodyBlock->cachedHeight;
                    for (int i = 0; i < bodyBlock->bodyCount; i++)
                    {
                        totalBodyHeight += calculateBodyHeight(*bodyBlock, i);
                    }
                    yOffset += totalBodyHeight + 15 * bodyBlock->bodyCount; // +15px Bottom side paddind
                }

                // Recursively update this block's children
                updateChildBlockPositions(*bodyBlock);

                currentBlockId = bodyBlock->nextId;
            }
        }
    }

    // Update input slot children positions
    Block def = blocksLibrary[parent.defenitionId];
    std::vector<int> wst = calcLabelSizeByPart(def.label);
    for (int slotIdx = 0; slotIdx < parent.inputCount; slotIdx++)
    {
        if (parent.inputs[slotIdx].isBlock)
        {
            int totalWidth = 0;
            for (int i = 0; i < slotIdx; i++)
            {
                totalWidth += wst[i];
                if (i < parent.inputCount && parent.textboxes[i])
                    totalWidth += parent.textboxes[i]->cachedWidth;
            }
            totalWidth += wst[slotIdx];

            BlockInstance *inputBlock = findBlockInstanceById(parent.inputs[slotIdx].blockInstanceId);
            if (inputBlock)
            {
                Block inputDef = blocksLibrary[inputBlock->defenitionId];
                inputBlock->posX = parent.posX + totalWidth + BLOCKINSTANCE_RL_MARGIN / 2;
                inputBlock->posY = parent.posY + (parent.cachedHeight - inputDef.baseHeight) / 2;

                // Recursively update this block's children
                updateChildBlockPositions(*inputBlock);
            }
        }
    }
}

/*
-----------------------
Build a chain of connected blocks starting from the given block
-----------------------
*/
void buildMovingChain(int startId, std::vector<int> &chain)
{
    chain.clear();
    int currentId = startId;

    while (currentId != -1)
    {
        chain.push_back(currentId);
        BlockInstance *inst = findBlockInstanceById(currentId);
        if (!inst)
            break;
        currentId = inst->nextId;
    }
}

/*
-----------------------
Bring a block instance to front by moving it to the end of the array
-----------------------
*/
void bringBlockToFront(int instanceId)
{
    int sourceSlot = -1;
    for (int i = 0; i < MAX_INSTANCES; i++)
    {
        if (workspaceCodeSpace.instanceUsed[i] &&
            workspaceCodeSpace.instances[i].instanceId == instanceId)
        {
            sourceSlot = i;
            break;
        }
    }

    if (sourceSlot == -1)
        return;

    // Find last used slot
    int lastUsedSlot = -1;
    for (int i = MAX_INSTANCES - 1; i >= 0; i--)
    {
        if (workspaceCodeSpace.instanceUsed[i])
        {
            lastUsedSlot = i;
            break;
        }
    }

    // If already at end or no swap needed
    if (sourceSlot == lastUsedSlot || lastUsedSlot == -1)
        return;

    // Swap instances
    BlockInstance temp = workspaceCodeSpace.instances[sourceSlot];
    workspaceCodeSpace.instances[sourceSlot] = workspaceCodeSpace.instances[lastUsedSlot];
    workspaceCodeSpace.instances[lastUsedSlot] = temp;

    bool tempUsed = workspaceCodeSpace.instanceUsed[sourceSlot];
    workspaceCodeSpace.instanceUsed[sourceSlot] = workspaceCodeSpace.instanceUsed[lastUsedSlot];
    workspaceCodeSpace.instanceUsed[lastUsedSlot] = tempUsed;
}

/*
-----------------------
Update positions of all blocks in a chain
-----------------------
*/
void updateChainPositions(const std::vector<int> &chain, int startX, int startY)
{
    int currentY = startY;

    for (int id : chain)
    {
        BlockInstance *inst = findBlockInstanceById(id);
        if (!inst)
            continue;

        Block def = blocksLibrary[inst->defenitionId];
        inst->posX = startX;
        inst->posY = currentY;

        // Update child positions recursively
        updateChildBlockPositions(*inst);

        if (inst->bodyCount == 0)
        {
            currentY += inst->cachedHeight;
        }
        else
        {
            int totalBodyHeight = inst->cachedHeight;
            for (int i = 0; i < inst->bodyCount; i++)
            {
                totalBodyHeight += calculateBodyHeight(*inst, i);
            }
            currentY += totalBodyHeight + 15 * inst->bodyCount; // +15px Bottom side paddind
        }
    }
}

/*
--------------------------------------------
This function is for controling mouse button up on WorkSpace
--------------------------------------------
- drag & drop
*/
void controlWorkspaceClickUp(const int mouseX, const int mouseY)
{
    // check if signal is about workspace
    if (!isPointInRect(mouseX, mouseY, WORKSPACE_COLUMN))
        return;

    // relative position to workspace cordinate
    const int relX = mouseX - WORKSPACE_COLUMN.x - scrollOffsetX;
    const int relY = mouseY - WORKSPACE_COLUMN.y - scrollOffsetY;

    // Add draged item to screen
    if (isBLockDraged)
    {
        isBLockDraged = false;

        // Create new item
        int posX = relX - blocksLibrary[dragedBlockIndex].baseWidth / 2;
        int posY = relY - blocksLibrary[dragedBlockIndex].baseHeight / 2;

        int newInstanceId = codespaceCreateInstance(workspaceCodeSpace, dragedBlockIndex, posX, posY);

        if (newInstanceId != -1)
        {
            bool connected = false;

            //  sequential connection
            if (blocksLibrary[dragedBlockIndex].canHaveTopConnection)
            {
                // For sequential connections, use the top-center of the dragged block
                const int topCenterX = posX + blocksLibrary[dragedBlockIndex].baseWidth / 2;
                const int topCenterY = posY;
                int topId = isItemNearTopToConnect(topCenterX, topCenterY);
                if (topId != -1) // connect Item to it's top item
                {
                    BlockInstance *newInst = findBlockInstanceById(newInstanceId);
                    BlockInstance *topInst = findBlockInstanceById(topId);

                    if (canConnectSequentially(topId, newInstanceId))
                    {
                        // Calculate total height including bodies for proper positioning
                        int totalHeight = topInst->cachedHeight;
                        for (int bodyIdx = 0; bodyIdx < topInst->bodyCount; bodyIdx++)
                        {
                            totalHeight += calculateBodyHeight(*topInst, bodyIdx) + 15; // Add body height + bottom margin
                        }

                        newInst->posX = topInst->posX;
                        newInst->posY = topInst->posY + totalHeight;

                        codespaceAttachNext(workspaceCodeSpace, topId, newInstanceId);
                        connected = true;
                    }
                }
            }

            //  body connection if sequential connection failed
            if (!connected)
            {
                int bodyIndex = -1;
                int parentId = isItemNearBodyToConnect(posX, posY, bodyIndex);
                if (parentId != -1)
                {
                    BlockInstance *newInst = findBlockInstanceById(newInstanceId);
                    BlockInstance *parentInst = findBlockInstanceById(parentId);

                    if (canConnectToBody(parentId, bodyIndex, newInstanceId))
                    {
                        // Position the block inside the body area
                        int bodyStartY = parentInst->posY + parentInst->cachedHeight;

                        // Calculate accumulated height of previous bodies using dynamic heights
                        int accumulatedHeight = 0;
                        for (int i = 0; i < bodyIndex; i++)
                        {
                            accumulatedHeight += calculateBodyHeight(*parentInst, i) + 15; // Add bottom margin
                        }

                        newInst->posX = parentInst->posX + 20;               // Indent inside body
                        newInst->posY = bodyStartY + accumulatedHeight + 10; // Small margin from top

                        codespaceAttachToBody(workspaceCodeSpace, parentId, bodyIndex, newInstanceId);

                        // Update child positions recursively
                        updateChildBlockPositions(*parentInst);

                        // Bring connected block to front for proper z-index
                        bringBlockToFront(newInstanceId);

                        connected = true;
                    }
                }
            }

            // Try input connection if other connections failed
            if (!connected)
            {
                Block draggedDef = blocksLibrary[dragedBlockIndex];
                // Only expression blocks (operators, reporter) can be connected to inputs
                if (draggedDef.type == BLOCK_OPERATOR || draggedDef.type == BLOCK_REPORTER)
                {
                    int slotIndex = -1;
                    int hostId = isItemNearInputToConnect(posX, posY, slotIndex);
                    if (hostId != -1)
                    {
                        BlockInstance *newInst = findBlockInstanceById(newInstanceId);
                        BlockInstance *hostInst = findBlockInstanceById(hostId);

                        if (canConnectToInput(hostId, slotIndex, newInstanceId))
                        {
                            // Position the block near the input slot (for visual feedback)
                            // The actual rendering will be handled by the host block
                            Block hostDef = blocksLibrary[hostInst->defenitionId];
                            std::vector<int> wst = calcLabelSizeByPart(hostDef.label);
                            int totalWidth = 0;
                            for (int i = 0; i < slotIndex; i++)
                            {
                                totalWidth += wst[i];
                                if (i < hostInst->inputCount && hostInst->textboxes[i])
                                    totalWidth += hostInst->textboxes[i]->cachedWidth;
                            }
                            totalWidth += wst[slotIndex];

                            // Position the block inside the input slot area
                            newInst->posX = hostInst->posX + totalWidth + BLOCKINSTANCE_RL_MARGIN / 2;
                            newInst->posY = hostInst->posY + (hostInst->cachedHeight - draggedDef.baseHeight) / 2; // Center vertically

                            codespaceAttachToInput(workspaceCodeSpace, hostId, slotIndex, newInstanceId);

                            // Update host block width to accommodate the new input block
                            hostInst->cachedWidth = std::max(hostInst->cachedWidth,
                                                             totalWidth + draggedDef.baseWidth + BLOCKINSTANCE_RL_MARGIN);

                            // Update child positions recursively
                            updateChildBlockPositions(*hostInst);

                            // Bring connected block to front for proper z-index
                            bringBlockToFront(newInstanceId);

                            connected = true;
                        }
                    }
                }
            }
        }
    }

    // Fix moving chain
    if (isMovingItem && movingInstanceId != -1)
    {
        BlockInstance *movingInst = findBlockInstanceById(movingInstanceId);
        if (movingInst)
        {
            bool connected = false;

            // Try sequential connection first
            if (blocksLibrary[movingInst->defenitionId].canHaveTopConnection)
            {
                // For sequential connections, use the top-center of the moving block
                Block movingDef = blocksLibrary[movingInst->defenitionId];
                const int topCenterX = movingInst->posX + movingDef.baseWidth / 2;
                const int topCenterY = movingInst->posY;
                int topId = isItemNearTopToConnect(topCenterX, topCenterY);
                if (topId != -1)
                {
                    BlockInstance *topInst = findBlockInstanceById(topId);
                    if (canConnectSequentially(topId, movingInst->instanceId))
                    {
                        // Calculate total height including bodies for proper positioning
                        int totalHeight = topInst->cachedHeight;
                        for (int bodyIdx = 0; bodyIdx < topInst->bodyCount; bodyIdx++)
                        {
                            totalHeight += calculateBodyHeight(*topInst, bodyIdx) + 15; // Add body height + bottom margin
                        }

                        int newX = topInst->posX;
                        int newY = topInst->posY + totalHeight;

                        updateChainPositions(movingChainIds, newX, newY);
                        codespaceAttachNext(workspaceCodeSpace, topId, movingInstanceId);
                        connected = true;
                    }
                }
            }

            // Try body connection if sequential connection failed
            if (!connected)
            {
                int bodyIndex = -1;
                int parentId = isItemNearBodyToConnect(movingInst->posX, movingInst->posY, bodyIndex);
                if (parentId != -1)
                {
                    BlockInstance *parentInst = findBlockInstanceById(parentId);

                    if (canConnectToBody(parentId, bodyIndex, movingInstanceId))
                    {
                        // Position the moving chain inside the body area
                        int bodyStartY = parentInst->posY + parentInst->cachedHeight;

                        // Calculate accumulated height of previous bodies using dynamic heights
                        int accumulatedHeight = 0;
                        for (int i = 0; i < bodyIndex; i++)
                        {
                            accumulatedHeight += calculateBodyHeight(*parentInst, i) + 15; // Add bottom margin
                        }

                        int newX = parentInst->posX + 20;               // Indent inside body
                        int newY = bodyStartY + accumulatedHeight + 10; // Small margin from top

                        updateChainPositions(movingChainIds, newX, newY);
                        codespaceAttachToBody(workspaceCodeSpace, parentId, bodyIndex, movingInstanceId);

                        // Update child positions recursively
                        updateChildBlockPositions(*parentInst);

                        // Bring moved chain to front for proper z-index
                        for (int chainId : movingChainIds)
                        {
                            bringBlockToFront(chainId);
                        }

                        connected = true;
                    }
                }
            }

            // Try input connection if other connections failed
            if (!connected)
            {
                Block movingDef = blocksLibrary[movingInst->defenitionId];
                // Only expression blocks (operators, reporter) can be connected to inputs
                if (movingDef.type == BLOCK_OPERATOR || movingDef.type == BLOCK_REPORTER)
                {
                    int slotIndex = -1;
                    int hostId = isItemNearInputToConnect(movingInst->posX, movingInst->posY, slotIndex);
                    if (hostId != -1)
                    {
                        BlockInstance *hostInst = findBlockInstanceById(hostId);

                        if (canConnectToInput(hostId, slotIndex, movingInst->instanceId))
                        {
                            // Position the moving block near the input slot
                            Block hostDef = blocksLibrary[hostInst->defenitionId];
                            std::vector<int> wst = calcLabelSizeByPart(hostDef.label);
                            int totalWidth = 0;
                            for (int i = 0; i < slotIndex; i++)
                            {
                                totalWidth += wst[i];
                                if (i < hostInst->inputCount && hostInst->textboxes[i])
                                    totalWidth += hostInst->textboxes[i]->cachedWidth;
                            }
                            totalWidth += wst[slotIndex];

                            // Position the block inside the input slot area
                            int newX = hostInst->posX + totalWidth + BLOCKINSTANCE_RL_MARGIN / 2;
                            int newY = hostInst->posY + (hostInst->cachedHeight - movingDef.baseHeight) / 2; // Center vertically

                            updateChainPositions(movingChainIds, newX, newY);
                            codespaceAttachToInput(workspaceCodeSpace, hostId, slotIndex, movingInstanceId);

                            // Update host block width to accommodate the new input block
                            hostInst->cachedWidth = std::max(hostInst->cachedWidth,
                                                             totalWidth + movingDef.baseWidth + BLOCKINSTANCE_RL_MARGIN);

                            // Update child positions recursively
                            updateChildBlockPositions(*hostInst);

                            // Bring moved chain to front for proper z-index
                            for (int chainId : movingChainIds)
                            {
                                bringBlockToFront(chainId);
                            }

                            connected = true;
                        }
                    }
                }
            }
        }

        isMovingItem = false;
        movingChainIds.clear();
        movingInstanceId = -1;
    }

    if (isScrollingWorkspace) // Disable Scrolling
    {
        isScrollingWorkspace = false;
    }

    return;
}

/*
--------------------------------------------
This function is for controling mouse button down on WorkSpace
--------------------------------------------
- drag & drop
- move items
*/
void controlWorkspaceClickDown(const int mouseX, const int mouseY)
{
    // check if signal is about workspace
    if (!isPointInRect(mouseX, mouseY, WORKSPACE_COLUMN))
        return;

    bool isClickedOnItem = false;

    // Check BlockInstances
    for (int i = 0; i < MAX_INSTANCES && !isClickedOnItem; i++)
    {
        if (!workspaceCodeSpace.instanceUsed[i])
            continue;

        BlockInstance inst = workspaceCodeSpace.instances[i];
        Block def = blocksLibrary[inst.defenitionId];

        SDL_Rect itemRect = {
            WORKSPACE_COLUMN.x + inst.posX + scrollOffsetX,
            WORKSPACE_COLUMN.y + inst.posY + scrollOffsetY,
            def.baseWidth,
            def.baseHeight,
        };

        if (isPointInRect(mouseX, mouseY, itemRect)) // found grabbed item
        {
            isMovingItem = true;
            isClickedOnItem = true;
            movingInstanceId = inst.instanceId;

            // Detach from parent connections
            codespaceDetach(workspaceCodeSpace, inst.instanceId);

            // Build the chain of connected blocks
            buildMovingChain(movingInstanceId, movingChainIds);

            // Bring moving chain to front for proper z-index
            for (int chainId : movingChainIds)
            {
                bringBlockToFront(chainId);
            }

            break;
        }
    }

    // Scroll workspace screen
    if (!isClickedOnItem && !isScrollingWorkspace) // clicked on emtpy space
    {
        isScrollingWorkspace = true;
        mouseX0 = mouseX;
        mouseY0 = mouseY;
    }
}

/*
--------------------------------------------
This function is for controling mouse motion on WorkSpace
--------------------------------------------
- move items
*/
void controlWorkspaceMouseMotion(const int mouseX, const int mouseY)
{
    // check if signal is about workspace
    if (!isPointInRect(mouseX, mouseY, WORKSPACE_COLUMN))
        return;

    // update moving chain
    if (isMovingItem && movingInstanceId != -1)
    {
        // relative position to workspace coordinate
        const int relX = mouseX - WORKSPACE_COLUMN.x - scrollOffsetX;
        const int relY = mouseY - WORKSPACE_COLUMN.y - scrollOffsetY;

        BlockInstance *movingInst = findBlockInstanceById(movingInstanceId);
        if (movingInst)
        {
            Block def = blocksLibrary[movingInst->defenitionId];
            int newX = relX - def.baseWidth / 2;
            int newY = relY - def.baseHeight / 2;

            updateChainPositions(movingChainIds, newX, newY);
        }
    }

    if (isScrollingWorkspace)
    {
        scrollOffsetX += mouseX - mouseX0;
        scrollOffsetY += mouseY - mouseY0;
        mouseX0 = mouseX;
        mouseY0 = mouseY;
    }
}

void drawWorkspaceScreen(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY)
{
    // Draw BlockInstances first (background layer)
    for (int i = 0; i < MAX_INSTANCES; i++)
    {
        if (!workspaceCodeSpace.instanceUsed[i])
            continue;

        BlockInstance inst = workspaceCodeSpace.instances[i];
        Block def = blocksLibrary[inst.defenitionId];

        updateBlockInstanceInputValues(inst);

        std::string blStr = blockInstanceUpdateLabel(inst);

        SDL_Rect itemRect = {
            WORKSPACE_COLUMN.x + inst.posX + scrollOffsetX,
            WORKSPACE_COLUMN.y + inst.posY + scrollOffsetY,
            inst.cachedWidth,
            inst.cachedHeight,
        };

        // item background
        if (!isBLockDraged && isPointInRect(mouseX, mouseY, itemRect))
        {
            SDL_SetRenderDrawColor(renderer, colorDim(categories[def.categoryId].color));
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, categories[def.categoryId].color);
        }
        SDL_RenderFillRect(renderer, &itemRect);

        // item border
        SDL_SetRenderDrawColor(renderer, color_black);
        SDL_RenderDrawRect(renderer, &itemRect);

        // item text
        int tw, th;
        SDL_Texture *texture = renderText(renderer, font, blStr.c_str(), color_white);
        SDL_QueryTexture(texture, NULL, NULL, &tw, &th);
        SDL_Rect textRect = {
            itemRect.x + (itemRect.w - tw) / 2,
            itemRect.y + (itemRect.h - th) / 2,
            tw,
            th,
        };
        SDL_RenderCopy(renderer, texture, NULL, &textRect);

        // Textboxes
        std::vector<int> wst = calcLabelSizeByPart(def.label);
        int totalWitdh = 0;
        for (int i = 0; i < inst.inputCount; i++)
        {
            totalWitdh += wst[i];
            inst.textboxes[i]->posX = WORKSPACE_COLUMN.x + scrollOffsetX + inst.posX + totalWitdh + BLOCKINSTANCE_RL_MARGIN / 2;
            inst.textboxes[i]->posY = WORKSPACE_COLUMN.y + scrollOffsetY + inst.posY + (inst.cachedHeight - inst.textboxes[i]->cachedHeight) / 2;
            inst.textboxes[i]->draw(renderer);
            totalWitdh += inst.textboxes[i]->cachedWidth;
        }

        // Bodies
        // frame
        int totalHeightBlock = 0;
        for (int i = 0; i < inst.bodyCount; i++)
        {
            // Use the helper function to calculate dynamic body height
            int bodyHeight = calculateBodyHeight(inst, i);

            SDL_Rect sideRect = {
                WORKSPACE_COLUMN.x + scrollOffsetX + inst.posX,
                WORKSPACE_COLUMN.y + scrollOffsetY + inst.posY + inst.cachedHeight + totalHeightBlock,
                15,
                bodyHeight,
            };
            SDL_Rect bottomRect = {
                WORKSPACE_COLUMN.x + scrollOffsetX + inst.posX,
                WORKSPACE_COLUMN.y + scrollOffsetY + inst.posY + inst.cachedHeight + bodyHeight + totalHeightBlock,
                inst.cachedWidth,
                15,
            };

            totalHeightBlock += bodyHeight + 15; // Add bottom rect height

            if (!isBLockDraged && isPointInRect(mouseX, mouseY, itemRect))
            {
                SDL_SetRenderDrawColor(renderer, colorDim(categories[def.categoryId].color));
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, categories[def.categoryId].color);
            }
            SDL_RenderFillRect(renderer, &sideRect);
            SDL_RenderFillRect(renderer, &bottomRect);
            // item border
            SDL_SetRenderDrawColor(renderer, color_black);
            SDL_RenderDrawRect(renderer, &sideRect);
            SDL_RenderDrawRect(renderer, &bottomRect);
        }
    }

    // Draw shadows on top of blocks for proper z-index
    // Draw shadow for connecting draged item
    if (isBLockDraged)
    {
        const int TL_X = mouseX - blocksLibrary[dragedBlockIndex].baseWidth / 2 - WORKSPACE_COLUMN.x - scrollOffsetX;
        const int TL_Y = mouseY - blocksLibrary[dragedBlockIndex].baseHeight / 2 - WORKSPACE_COLUMN.y - scrollOffsetY;

        // sequential connection shadow
        bool shadowDrawn = false;
        if (blocksLibrary[dragedBlockIndex].canHaveTopConnection)
        {
            // For sequential connections, use the top-center of the dragged block
            const int topCenterX = TL_X + blocksLibrary[dragedBlockIndex].baseWidth / 2;
            const int topCenterY = TL_Y;
            int nearItemId = isItemNearTopToConnect(topCenterX, topCenterY);
            if (nearItemId != -1) // there is a near connectable item
            {
                BlockInstance *nearInst = findBlockInstanceById(nearItemId);
                if (nearInst && blocksLibrary[dragedBlockIndex].canHaveTopConnection)
                {
                    // Calculate total height including bodies for proper shadow positioning
                    int totalHeight = nearInst->cachedHeight;
                    for (int bodyIdx = 0; bodyIdx < nearInst->bodyCount; bodyIdx++)
                    {
                        totalHeight += calculateBodyHeight(*nearInst, bodyIdx) + 15; // Add body height + bottom margin
                    }

                    // Draw dragged item shadow
                    SDL_Rect dragedItemRect = {
                        WORKSPACE_COLUMN.x + nearInst->posX + scrollOffsetX,
                        WORKSPACE_COLUMN.y + nearInst->posY + totalHeight + scrollOffsetY,
                        blocksLibrary[dragedBlockIndex].baseWidth,
                        blocksLibrary[dragedBlockIndex].baseHeight,
                    };

                    // Fill item background
                    SDL_SetRenderDrawColor(renderer, color_softGray);
                    SDL_RenderFillRect(renderer, &dragedItemRect);
                    shadowDrawn = true;
                }
            }
        }

        // body connection shadow if sequential shadow not drawn
        if (!shadowDrawn)
        {
            int bodyIndex = -1;
            int parentId = isItemNearBodyToConnect(TL_X, TL_Y, bodyIndex);
            if (parentId != -1)
            {
                BlockInstance *parentInst = findBlockInstanceById(parentId);
                // std::cout << parentInst->bodies[bodyIndex] << std::endl;
                if (parentInst && parentInst->bodies[bodyIndex] == -1 && blocksLibrary[dragedBlockIndex].canHaveTopConnection)
                {
                    int bodyStartY = parentInst->posY + parentInst->cachedHeight;

                    // Calculate accumulated height of previous bodies using dynamic heights
                    int accumulatedHeight = 0;
                    for (int i = 0; i < bodyIndex; i++)
                    {
                        accumulatedHeight += calculateBodyHeight(*parentInst, i) + 15; // Add bottom margin
                    }

                    // Draw dragged item shadow in body area
                    SDL_Rect dragedItemRect = {
                        WORKSPACE_COLUMN.x + parentInst->posX + 20 + scrollOffsetX,
                        WORKSPACE_COLUMN.y + bodyStartY + accumulatedHeight + 10 + scrollOffsetY,
                        blocksLibrary[dragedBlockIndex].baseWidth,
                        blocksLibrary[dragedBlockIndex].baseHeight,
                    };

                    // Fill item background with different color for body connections
                    SDL_SetRenderDrawColor(renderer, color_softGray);
                    SDL_RenderFillRect(renderer, &dragedItemRect);
                    shadowDrawn = true;
                }
            }
        }

        // Try input connection shadow if other shadows not drawn
        if (!shadowDrawn)
        {
            Block draggedDef = blocksLibrary[dragedBlockIndex];
            // Only expression blocks (operators, reporter) can be connected to inputs
            if (draggedDef.type == BLOCK_OPERATOR || draggedDef.type == BLOCK_REPORTER)
            {
                int slotIndex = -1;
                int hostId = isItemNearInputToConnect(TL_X, TL_Y, slotIndex);
                if (hostId != -1)
                {
                    BlockInstance *hostInst = findBlockInstanceById(hostId);
                    if (hostInst && !hostInst->inputs[slotIndex].isBlock)
                    {
                        // Calculate input slot position
                        Block hostDef = blocksLibrary[hostInst->defenitionId];
                        std::vector<int> wst = calcLabelSizeByPart(hostDef.label);
                        int totalWidth = 0;
                        for (int i = 0; i < slotIndex; i++)
                        {
                            totalWidth += wst[i];
                            if (i < hostInst->inputCount && hostInst->textboxes[i])
                                totalWidth += hostInst->textboxes[i]->cachedWidth;
                        }
                        totalWidth += wst[slotIndex];

                        // Draw dragged item shadow inside input slot area
                        SDL_Rect dragedItemRect = {
                            WORKSPACE_COLUMN.x + hostInst->posX + totalWidth + BLOCKINSTANCE_RL_MARGIN / 2 + scrollOffsetX,
                            WORKSPACE_COLUMN.y + hostInst->posY + (hostInst->cachedHeight - draggedDef.baseHeight) / 2 + scrollOffsetY,
                            draggedDef.baseWidth,
                            draggedDef.baseHeight,
                        };

                        // Fill item background with input connection color
                        SDL_SetRenderDrawColor(renderer, color_blurgray);
                        SDL_RenderFillRect(renderer, &dragedItemRect);
                        shadowDrawn = true;
                    }
                }
            }
        }
    }

    // Draw shadow for connecting moving chain
    if (isMovingItem && movingInstanceId != -1)
    {
        BlockInstance *movingInst = findBlockInstanceById(movingInstanceId);
        if (movingInst)
        {
            bool movingShadowDrawn = false;

            // Try sequential connection shadow first
            if (blocksLibrary[movingInst->defenitionId].canHaveTopConnection)
            {
                // For sequential connections, use the top-center of the moving block
                Block movingDef = blocksLibrary[movingInst->defenitionId];
                const int topCenterX = movingInst->posX + movingDef.baseWidth / 2;
                const int topCenterY = movingInst->posY;
                int topItemId = isItemNearTopToConnect(topCenterX, topCenterY);
                if (topItemId != -1) //  there is a near connectable item
                {
                    BlockInstance *topInst = findBlockInstanceById(topItemId);
                    if (canConnectSequentially(topItemId, movingInst->instanceId))
                    {
                        // Calculate total height including bodies for proper shadow positioning
                        int topTotalHeight = topInst->cachedHeight;
                        for (int bodyIdx = 0; bodyIdx < topInst->bodyCount; bodyIdx++)
                        {
                            topTotalHeight += calculateBodyHeight(*topInst, bodyIdx) + 15; // Add body height + bottom margin
                        }

                        int chainHeight = 0;

                        for (int id : movingChainIds)
                        {
                            BlockInstance *chainInst = findBlockInstanceById(id);
                            if (!chainInst)
                                continue;

                            Block chainDef = blocksLibrary[chainInst->defenitionId];

                            // Draw shadow for each block in chain
                            SDL_Rect shadowRect = {
                                WORKSPACE_COLUMN.x + topInst->posX + scrollOffsetX,
                                WORKSPACE_COLUMN.y + topInst->posY + topTotalHeight + chainHeight + scrollOffsetY,
                                chainDef.baseWidth,
                                chainDef.baseHeight,
                            };

                            chainHeight += chainDef.baseHeight;

                            // Fill item background
                            SDL_SetRenderDrawColor(renderer, color_softGray);
                            SDL_RenderFillRect(renderer, &shadowRect);
                        }
                        movingShadowDrawn = true;
                    }
                }
            }

            // Try body connection shadow if sequential shadow not drawn
            if (!movingShadowDrawn)
            {
                int bodyIndex = -1;
                int parentId = isItemNearBodyToConnect(movingInst->posX, movingInst->posY, bodyIndex);
                if (parentId != -1)
                {
                    BlockInstance *parentInst = findBlockInstanceById(parentId);
                    if (canConnectToBody(parentId, bodyIndex, movingInst->instanceId))
                    {
                        int bodyStartY = parentInst->posY + parentInst->cachedHeight;

                        // Calculate accumulated height of previous bodies using dynamic heights
                        int accumulatedHeight = 0;
                        for (int i = 0; i < bodyIndex; i++)
                        {
                            accumulatedHeight += calculateBodyHeight(*parentInst, i) + 15; // Add bottom margin
                        }

                        int totalHeight = 0;

                        for (int id : movingChainIds)
                        {
                            BlockInstance *chainInst = findBlockInstanceById(id);
                            if (!chainInst)
                                continue;

                            Block chainDef = blocksLibrary[chainInst->defenitionId];

                            // Draw shadow for each block in chain inside body area
                            SDL_Rect shadowRect = {
                                WORKSPACE_COLUMN.x + parentInst->posX + 20 + scrollOffsetX,
                                WORKSPACE_COLUMN.y + bodyStartY + accumulatedHeight + 10 + totalHeight + scrollOffsetY,
                                chainDef.baseWidth,
                                chainDef.baseHeight,
                            };

                            totalHeight += chainDef.baseHeight;

                            // Fill item background with body connection color
                            SDL_SetRenderDrawColor(renderer, color_softGray);
                            SDL_RenderFillRect(renderer, &shadowRect);
                        }
                        movingShadowDrawn = true;
                    }
                }
            }

            // Try input connection shadow if other shadows not drawn
            if (!movingShadowDrawn)
            {
                Block movingDef = blocksLibrary[movingInst->defenitionId];
                // Only expression blocks (operators, reporter) can be connected to inputs
                if (movingDef.type == BLOCK_OPERATOR || movingDef.type == BLOCK_REPORTER)
                {
                    int slotIndex = -1;
                    int hostId = isItemNearInputToConnect(movingInst->posX, movingInst->posY, slotIndex);
                    if (hostId != -1)
                    {
                        BlockInstance *hostInst = findBlockInstanceById(hostId);
                        if (canConnectToInput(hostId, slotIndex, movingInst->instanceId))
                        {
                            // Calculate input slot position
                            Block hostDef = blocksLibrary[hostInst->defenitionId];
                            std::vector<int> wst = calcLabelSizeByPart(hostDef.label);
                            int totalWidth = 0;
                            for (int i = 0; i < slotIndex; i++)
                            {
                                totalWidth += wst[i];
                                if (i < hostInst->inputCount && hostInst->textboxes[i])
                                    totalWidth += hostInst->textboxes[i]->cachedWidth;
                            }
                            totalWidth += wst[slotIndex];

                            // Draw moving item shadow inside input slot area
                            SDL_Rect shadowRect = {
                                WORKSPACE_COLUMN.x + hostInst->posX + totalWidth + BLOCKINSTANCE_RL_MARGIN / 2 + scrollOffsetX,
                                WORKSPACE_COLUMN.y + hostInst->posY + (hostInst->cachedHeight - movingDef.baseHeight) / 2 + scrollOffsetY,
                                movingDef.baseWidth,
                                movingDef.baseHeight,
                            };

                            // Fill item background with input connection color
                            SDL_SetRenderDrawColor(renderer, color_blurgray);
                            SDL_RenderFillRect(renderer, &shadowRect);
                            movingShadowDrawn = true;
                        }
                    }
                }
            }
        }
    }

    return;
}

int workspaceScreenInit(SDL_Renderer *renderer, TTF_Font *font)
{
    // Initialize the CodeSpace system
    codespaceInit(workspaceCodeSpace);
    return 0;
}

// Helper functions for CodeSpace integration

int createBlockInstanceInWorkspace(int definitionId, int posX, int posY)
{
    return codespaceCreateInstance(workspaceCodeSpace, definitionId, posX, posY);
}

void attachBlocksSequentially(int topId, int bottomId)
{
    codespaceAttachNext(workspaceCodeSpace, topId, bottomId);
}

void detachBlockFromWorkspace(int id)
{
    codespaceDetach(workspaceCodeSpace, id);
}

void removeBlockFromWorkspace(int id)
{
    codespaceRemoveInstance(workspaceCodeSpace, id);
}

std::string blockInstanceUpdateLabel(BlockInstance &inst)
{
    const char *format = blocksLibrary[inst.defenitionId].label;
    std::string out = lprintf(format, inst.inputs);

    TTF_SizeText(inst.textboxes[0]->font, out.c_str(), &inst.cachedWidth, nullptr);
    inst.cachedWidth += BLOCKINSTANCE_RL_MARGIN;

    return out;
}

std::vector<int> calcLabelSizeByPart(const char *label)
{
    std::vector<int> out;
    std::string temp = "";
    for (int i = 0; i < strlen(label); i++)
    {
        if (label[i] == '%')
        {
            i++;
            if (label[i] == 's')
            {
                int tw;
                TTF_SizeText(DEFAULT_FONT, temp.c_str(), &tw, nullptr);
                out.push_back(tw);
                temp.clear();
            }
            continue;
        }

        temp.push_back(label[i]);
    }

    if (!temp.empty())
    {
        int tw;
        TTF_SizeText(DEFAULT_FONT, temp.c_str(), &tw, nullptr);
        out.push_back(tw);
        temp.clear();
    }

    return out;
}

std::vector<int> calcLabelSizeByPartCamulative(const char *label)
{
    std::vector<int> sizes = calcLabelSizeByPart(label);
    if (sizes.size() <= 1)
        return sizes;

    for (auto it = sizes.begin() + 1; it != sizes.end(); ++it)
    {
        *it = *it + *(it - 1);
    }
    return sizes;
}