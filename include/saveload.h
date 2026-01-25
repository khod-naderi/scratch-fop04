/*
---------------------------------------------------
This header file is for save and load logic
---------------------------------------------------
*/
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#ifndef SAVELOAD_HEADER
#define SAVELOAD_HEADER

extern bool isOnLoadScreen;
extern bool isOnSaveScreen;

void drawSaveScreen(SDL_Renderer *renderer, TTF_Font *font, const int &mouseX, const int &mouseY);
void drawLoadScreen(SDL_Renderer *renderer, TTF_Font *font, const int &mouseX, const int &mouseY);
void controlSaveClickDown(const int mouseX, const int mouseY);
int saveAllData();
int loadAllData();
int newProject();
#endif