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
#include "ui.h"
#include "block.h"

std::vector<CodeBlock> activeCodeBlocks;
int lastId = 0;

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
    const int relX = mouseX - WORKSPACE_COLUMN.x;
    const int relY = mouseY - WORKSPACE_COLUMN.y;

    // Add draged item to screen
    if (isBLockDraged)
    {
        isBLockDraged = false;

        // Create new item
        CodeBlock newItem;
        newItem.posX = relX - blocksLibrary[dragedBlockIndex].width / 2;
        newItem.posY = relY - blocksLibrary[dragedBlockIndex].height / 2;
        newItem.blockMaster = dragedBlockIndex;
        newItem.id = lastId++;
        newItem.bottomId = -1;
        newItem.topId = -1;

        activeCodeBlocks.push_back(newItem);
    }

    return;
}

void drawWorkspaceScreen(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY)
{
    for (CodeBlock item : activeCodeBlocks)
    {
        SDL_Rect itemRect = {
            WORKSPACE_COLUMN.x + item.posX,
            WORKSPACE_COLUMN.y + item.posY,
            blocksLibrary[item.blockMaster].width,
            blocksLibrary[item.blockMaster].height,
        };

        // item background
        if (isPointInRect(mouseX, mouseY, itemRect))
        {
            SDL_SetRenderDrawColor(renderer, colorDim(categories[blocksLibrary[item.blockMaster].categoryId].color));
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, categories[blocksLibrary[item.blockMaster].categoryId].color);
        }
        SDL_RenderFillRect(renderer, &itemRect);

        // item border
        SDL_SetRenderDrawColor(renderer, color_black);
        SDL_RenderDrawRect(renderer, &itemRect);

        // item text
        int tw, th;
        SDL_QueryTexture(blocksLibrary[item.blockMaster].textrue, NULL, NULL, &tw, &th);
        SDL_Rect textRect = {
            itemRect.x + (itemRect.w - tw) / 2,
            itemRect.y + (itemRect.h - th) / 2,
            tw,
            th,
        };
        SDL_RenderCopy(renderer, blocksLibrary[item.blockMaster].textrue, NULL, &textRect);
    }

    return;
}

int workspaceScreenInit(SDL_Renderer *renderer, TTF_Font *font)
{
    return 0;
}