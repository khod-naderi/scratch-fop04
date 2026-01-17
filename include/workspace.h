/*
-------------------------------------------
This header file is for managing code view named workspace
-------------------------------------------
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#ifndef WORKSPACE_HEADERS
#define WORKSPACE_HEADERS

void controlWorkspaceClickUp(const int mouseX, const int mouseY);
void drawWorkspaceScreen(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY);
int workspaceScreenInit(SDL_Renderer *renderer, TTF_Font *font);

#endif