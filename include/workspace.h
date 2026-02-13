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

#define CONNECTION_MINIMUM_DISTANCE 15

// All on code block will live here
extern CodeSpace workspaceCodeSpace;

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

// Body connection detection
int isItemNearBodyToConnect(const int TL_X, const int TL_Y, int &outBodyIndex);
bool isPointInBodyArea(const int pointX, const int pointY, const BlockInstance &inst, const int bodyIndex);

std::string blockInstanceUpdateLabel(BlockInstance &inst);

#endif