/*
-------------------------------------------
This header file is for managing code view named workspace
-------------------------------------------
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include "codespace.h"
#include "block_instance.h"

#ifndef WORKSPACE_HEADERS
#define WORKSPACE_HEADERS

// Legacy CodeBlock struct
struct CodeBlock
{
    int id;            // identifier
    int posX, posY;    // position related to workspace cordinate
    int blockMaster;   // index of block defined in blocksLibrary
    int topId = -1;    // top block id, -1 if not connected
    int bottomId = -1; // bottom block id, -1 if not connected
};

#define CONNECTION_MINIMUM_DISTANCE 15

extern CodeSpace workspaceCodeSpace;
extern std::vector<CodeBlock> activeCodeBlocks; // Legacy

void controlWorkspaceClickUp(const int mouseX, const int mouseY);
void controlWorkspaceClickDown(const int mouseX, const int mouseY);
void drawWorkspaceScreen(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY);
int workspaceScreenInit(SDL_Renderer *renderer, TTF_Font *font);
void controlWorkspaceMouseMotion(const int mouseX, const int mouseY);

BlockInstance *findBlockInstanceById(int id);
int createBlockInstanceInWorkspace(int definitionId, int posX, int posY);
void attachBlocksSequentially(int topId, int bottomId);
void detachBlockFromWorkspace(int id);
void removeBlockFromWorkspace(int id);

#endif