/*
-------------------------------------------
This header file is for managing custome and backgorund setting area named sprite box
-------------------------------------------
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

#ifndef SPRITEBOX_HEADER
#define SPRITEBOX_HEADER

// This struct will handle sprint bodies
struct SprintBody
{
    int id;                  // sprint id
    std::string name;        // sprint name, like "bob".
    unsigned int posX, posY; // sprint position related to Canvas Screen.
    SDL_Texture *nowCustome;
    // SDL_Texture **allCustome // TODO: Add this later for multicostume sprints
    // unsigned int selectedCustome = 0;
    // int customeCount;
};

// This will hold live sprints
extern std::vector<SprintBody> sprints;

int sprintBoxInit(SDL_Renderer *renderer, TTF_Font *font);
int addSprintToScreen(SDL_Renderer *renderer, std::string name, const char *imgPath);
void drawSpriteBoxScreen(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY);
#endif