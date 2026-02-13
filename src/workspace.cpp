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

        const int blc_x = inst.posX;
        const int blc_y = inst.posY + def.baseHeight;
        if (std::sqrt(std::pow((blc_x - TL_X), 2) + std::pow((blc_y - TL_Y), 2)) <= CONNECTION_MINIMUM_DISTANCE)
        {
            return inst.instanceId;
        }
    }

    return -1;
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
    int bodyStartY = inst.posY + def.baseHeight;
    int bodyHeight = 60; // Default body height, TODO: make it dynamic
    int bodyY = bodyStartY + (bodyIndex * bodyHeight);

    // Body area rectangle
    int bodyLeft = inst.posX + 10; // Margin from left edge
    int bodyRight = inst.posX + def.baseWidth - 10;
    int bodyTop = bodyY;
    int bodyBottom = bodyY + bodyHeight;

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
        if (i < inst.inputCount)
            totalWidth += inst.textboxes[i]->cachedWidth;
    }
    totalWidth += wst[slotIndex];

    // Input slot area rectangle (around where the textbox would be)
    int slotLeft = inst.posX + totalWidth + BLOCKINSTANCE_RL_MARGIN / 2;
    int slotRight = slotLeft + (inst.textboxes[slotIndex] ? inst.textboxes[slotIndex]->cachedWidth : 50);
    int slotTop = inst.posY;
    int slotBottom = inst.posY + inst.cachedHeight;

    return (pointX >= slotLeft && pointX <= slotRight &&
            pointY >= slotTop && pointY <= slotBottom);
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
        currentY += def.baseHeight;
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
                int topId = isItemNearTopToConnect(posX, posY);
                if (topId != -1) // connect Item to it's top item
                {
                    BlockInstance *newInst = findBlockInstanceById(newInstanceId);
                    BlockInstance *topInst = findBlockInstanceById(topId);

                    if (newInst && topInst)
                    {
                        Block topDef = blocksLibrary[topInst->defenitionId];
                        newInst->posX = topInst->posX;
                        newInst->posY = topInst->posY + topDef.baseHeight;

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

                    if (newInst && parentInst)
                    {
                        // Position the block inside the body area
                        Block parentDef = blocksLibrary[parentInst->defenitionId];
                        int bodyStartY = parentInst->posY + parentDef.baseHeight;
                        int bodyHeight = 60; // Same as in isPointInBodyArea TODO: use predefined or dynamic values

                        newInst->posX = parentInst->posX + 15;                     // Indent inside body
                        newInst->posY = bodyStartY + (bodyIndex * bodyHeight) + 5; // Small margin from top TODO: use predefined values

                        codespaceAttachToBody(workspaceCodeSpace, parentId, bodyIndex, newInstanceId);
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

                        if (newInst && hostInst)
                        {
                            // Position the block near the input slot (for visual feedback)
                            // The actual rendering will be handled by the host block
                            Block hostDef = blocksLibrary[hostInst->defenitionId];
                            std::vector<int> wst = calcLabelSizeByPart(hostDef.label);
                            int totalWidth = 0;
                            for (int i = 0; i < slotIndex; i++)
                            {
                                totalWidth += wst[i];
                                if (i < hostInst->inputCount)
                                    totalWidth += hostInst->textboxes[i]->cachedWidth;
                            }
                            totalWidth += wst[slotIndex];

                            newInst->posX = hostInst->posX + totalWidth;
                            newInst->posY = hostInst->posY - draggedDef.baseHeight - 5; // Above the host block

                            codespaceAttachToInput(workspaceCodeSpace, hostId, slotIndex, newInstanceId);
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
                int topId = isItemNearTopToConnect(movingInst->posX, movingInst->posY);
                if (topId != -1)
                {
                    BlockInstance *topInst = findBlockInstanceById(topId);
                    if (topInst)
                    {
                        Block topDef = blocksLibrary[topInst->defenitionId];
                        int newX = topInst->posX;
                        int newY = topInst->posY + topDef.baseHeight;

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

                    if (parentInst)
                    {
                        // Position the moving chain inside the body area
                        Block parentDef = blocksLibrary[parentInst->defenitionId];
                        int bodyStartY = parentInst->posY + parentDef.baseHeight;
                        int bodyHeight = 60; // Same as in isPointInBodyArea

                        int newX = parentInst->posX + 15;                     // Indent inside body
                        int newY = bodyStartY + (bodyIndex * bodyHeight) + 5; // Small margin from top

                        updateChainPositions(movingChainIds, newX, newY);
                        codespaceAttachToBody(workspaceCodeSpace, parentId, bodyIndex, movingInstanceId);
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

                        if (hostInst)
                        {
                            // Position the moving block near the input slot
                            Block hostDef = blocksLibrary[hostInst->defenitionId];
                            std::vector<int> wst = calcLabelSizeByPart(hostDef.label);
                            int totalWidth = 0;
                            for (int i = 0; i < slotIndex; i++)
                            {
                                totalWidth += wst[i];
                                if (i < hostInst->inputCount)
                                    totalWidth += hostInst->textboxes[i]->cachedWidth;
                            }
                            totalWidth += wst[slotIndex];

                            int newX = hostInst->posX + totalWidth;
                            int newY = hostInst->posY - movingDef.baseHeight - 5; // Above the host block

                            updateChainPositions(movingChainIds, newX, newY);
                            codespaceAttachToInput(workspaceCodeSpace, hostId, slotIndex, movingInstanceId);
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
    // Draw shadow for connecting draged item
    if (isBLockDraged)
    {
        const int TL_X = mouseX - blocksLibrary[dragedBlockIndex].baseWidth / 2 - WORKSPACE_COLUMN.x - scrollOffsetX;
        const int TL_Y = mouseY - blocksLibrary[dragedBlockIndex].baseHeight / 2 - WORKSPACE_COLUMN.y - scrollOffsetY;

        // sequential connection shadow
        bool shadowDrawn = false;
        if (blocksLibrary[dragedBlockIndex].canHaveTopConnection)
        {
            int nearItemId = isItemNearTopToConnect(TL_X, TL_Y);
            if (nearItemId != -1) // there is a near connectable item
            {
                BlockInstance *nearInst = findBlockInstanceById(nearItemId);
                if (nearInst)
                {
                    Block nearDef = blocksLibrary[nearInst->defenitionId];

                    // Draw dragged item shadow
                    SDL_Rect dragedItemRect = {
                        WORKSPACE_COLUMN.x + nearInst->posX + scrollOffsetX,
                        WORKSPACE_COLUMN.y + nearInst->posY + nearDef.baseHeight + scrollOffsetY,
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
                if (parentInst)
                {
                    Block parentDef = blocksLibrary[parentInst->defenitionId];
                    int bodyStartY = parentInst->posY + parentDef.baseHeight;
                    int bodyHeight = 60; // Same as in isPointInBodyArea

                    // Draw dragged item shadow in body area
                    SDL_Rect dragedItemRect = {
                        WORKSPACE_COLUMN.x + parentInst->posX + 15 + scrollOffsetX,
                        WORKSPACE_COLUMN.y + bodyStartY + (bodyIndex * bodyHeight) + 5 + scrollOffsetY,
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
                    if (hostInst)
                    {
                        // Calculate input slot position
                        Block hostDef = blocksLibrary[hostInst->defenitionId];
                        std::vector<int> wst = calcLabelSizeByPart(hostDef.label);
                        int totalWidth = 0;
                        for (int i = 0; i < slotIndex; i++)
                        {
                            totalWidth += wst[i];
                            if (i < hostInst->inputCount)
                                totalWidth += hostInst->textboxes[i]->cachedWidth;
                        }
                        totalWidth += wst[slotIndex];

                        // Draw dragged item shadow near input slot
                        SDL_Rect dragedItemRect = {
                            WORKSPACE_COLUMN.x + hostInst->posX + totalWidth + scrollOffsetX,
                            WORKSPACE_COLUMN.y + hostInst->posY - draggedDef.baseHeight - 5 + scrollOffsetY,
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
                int topItemId = isItemNearTopToConnect(movingInst->posX, movingInst->posY);
                if (topItemId != -1) //  there is a near connectable item
                {
                    BlockInstance *topInst = findBlockInstanceById(topItemId);
                    if (topInst)
                    {
                        Block topDef = blocksLibrary[topInst->defenitionId];
                        int totalHeight = 0;

                        for (int id : movingChainIds)
                        {
                            BlockInstance *chainInst = findBlockInstanceById(id);
                            if (!chainInst)
                                continue;

                            Block chainDef = blocksLibrary[chainInst->defenitionId];

                            // Draw shadow for each block in chain
                            SDL_Rect shadowRect = {
                                WORKSPACE_COLUMN.x + topInst->posX + scrollOffsetX,
                                WORKSPACE_COLUMN.y + topInst->posY + topDef.baseHeight + totalHeight + scrollOffsetY,
                                chainDef.baseWidth,
                                chainDef.baseHeight,
                            };

                            totalHeight += chainDef.baseHeight;

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
                    if (parentInst)
                    {
                        Block parentDef = blocksLibrary[parentInst->defenitionId];
                        int bodyStartY = parentInst->posY + parentDef.baseHeight;
                        int bodyHeight = 60; // Same as in isPointInBodyArea
                        int totalHeight = 0;

                        for (int id : movingChainIds)
                        {
                            BlockInstance *chainInst = findBlockInstanceById(id);
                            if (!chainInst)
                                continue;

                            Block chainDef = blocksLibrary[chainInst->defenitionId];

                            // Draw shadow for each block in chain inside body area
                            SDL_Rect shadowRect = {
                                WORKSPACE_COLUMN.x + parentInst->posX + 15 + scrollOffsetX,
                                WORKSPACE_COLUMN.y + bodyStartY + (bodyIndex * bodyHeight) + 5 + totalHeight + scrollOffsetY,
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
                        if (hostInst)
                        {
                            // Calculate input slot position
                            Block hostDef = blocksLibrary[hostInst->defenitionId];
                            std::vector<int> wst = calcLabelSizeByPart(hostDef.label);
                            int totalWidth = 0;
                            for (int i = 0; i < slotIndex; i++)
                            {
                                totalWidth += wst[i];
                                if (i < hostInst->inputCount)
                                    totalWidth += hostInst->textboxes[i]->cachedWidth;
                            }
                            totalWidth += wst[slotIndex];

                            // Draw moving item shadow near input slot
                            SDL_Rect shadowRect = {
                                WORKSPACE_COLUMN.x + hostInst->posX + totalWidth + scrollOffsetX,
                                WORKSPACE_COLUMN.y + hostInst->posY - movingDef.baseHeight - 5 + scrollOffsetY,
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

    // Draw BlockInstances
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
            int bodiesTotalHeight = 0; // TODO: make it dynamic by bodies

            SDL_Rect sideRect = {
                WORKSPACE_COLUMN.x + scrollOffsetX + inst.posX,
                WORKSPACE_COLUMN.y + scrollOffsetY + inst.posY + inst.cachedHeight + totalHeightBlock,
                10,
                bodiesTotalHeight + 40,
            };
            SDL_Rect bottomRect = {
                WORKSPACE_COLUMN.x + scrollOffsetX + inst.posX,
                WORKSPACE_COLUMN.y + scrollOffsetY + inst.posY + inst.cachedHeight + sideRect.h + totalHeightBlock,
                inst.cachedWidth,
                20,
            };

            totalHeightBlock += (bottomRect.y - sideRect.y) + bottomRect.h;

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