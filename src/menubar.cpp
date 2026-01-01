#include <SDL2/SDL.h>
#include "menubar.h"

// Define menu items
const char *fileMenuItems[] = {"New Project", "Load Project", "Save Project"};
const int fileMenuItemsCount = 3;
const char *editMenuItems[] = {"Help"};
const int editMenuItemsCount = 1;

// Define menu state
MenuState menuState = {false, false, -1, -1};

// Define texture and surface pointers
SDL_Surface *logoSurface = nullptr;
SDL_Texture *logoTexture = nullptr;
SDL_Texture *fileMenuText = nullptr;
SDL_Texture *editMenuText = nullptr;