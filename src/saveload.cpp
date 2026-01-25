/*
---------------------------------------------------
This CPP file is for save and load logic
---------------------------------------------------
*/

#include "saveload.h"
#include <vector>
#include "block.h"
#include "category.h"
#include "workspace.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "color.h"
#include "ui.h"
#include <iostream>

// to handle screen view
bool isOnLoadScreen = false;
bool isOnSaveScreen = false;

int saveAllData()
{
    return 0;
}

int newProject()
{
    return 0;
}

void drawSaveScreen(SDL_Renderer *renderer, TTF_Font *font, const int &mouseX, const int &mouseY)
{
    // blur backround
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, color_blurgray);
    SDL_RenderFillRect(renderer, &SCREEN);

    // draw save windows
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(renderer, color_white);
    SDL_RenderFillRect(renderer, &SAVELOAD_BOX);
    SDL_SetRenderDrawColor(renderer, color_black);
    SDL_RenderDrawRect(renderer, &SAVELOAD_BOX);
}

void drawLoadScreen(SDL_Renderer *renderer, TTF_Font *font, const int &mouseX, const int &mouseY)
{
    // blur backround
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, color_blurgray);
    SDL_RenderFillRect(renderer, &SCREEN);
}

void controlSaveClickDown(const int mouseX, const int mouseY)
{
    if (!isPointInRect(mouseX, mouseY, SAVELOAD_BOX))
    {
        isOnSaveScreen = false;
        isOnLoadScreen = false;
        return;
    }
}