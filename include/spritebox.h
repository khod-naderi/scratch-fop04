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
    int id;           // sprint id
    std::string name; // sprint name, like "bob".
    int posX, posY;   // sprint position related to Canvas Screen.
    SDL_Texture *nowCustome;
    // SDL_Texture **allCustome // TODO: Add this later for multicostume sprints
    // unsigned int selectedCustome = 0;
    // int customeCount;
    double angleRotation; // rotation of the sprint. ROTATION IS CLOCKWISE
    int scale = 100;        // scale of the sprint in percentage. 100 means original size.
    bool isVisible = true; // if fale, not available on canvas, there will be an option on top bar to handle visibility.
};

extern int SelectedSpriteID; // this variable will save selected sprite in sprite box


// this struct&enum will handle sprite choosing logic and menu bar 
enum AddSpriteMenuState {
    Menu_Closed, // not shown
    Menu_Hover_Main, // hovering on main button
    Menu_Open // menu is open, showing 4 options: add, random, upload , paint
};

struct MenuOption {
    const char* icon; 
    std::string name; // "choose a sprite", "paint", "upload", "random"
    SDL_Rect rect; // position and size of option
    bool isHovered; 
};

/*
----------------------------------------------------
this part is for handling sprite choosing screen.
----------------------------------------------------
*/
const int AVAILABLE_SPRITES_COUNT = 9; 
struct availableSprite {
    //std::string id;
    std::string name;
    std::string imagePath;
    SDL_Texture* texture;
};

enum ScreenState {
    State_MainEditor,
    State_Sprite_Picker,
};

extern ScreenState currentState; // this variable will save current screen state, like main editor or sprite picker

// Sprint image dimenstion
#define SPRINT_IMG_WITDH 100
#define SPRINT_IMG_HEIGHT 100

int sprintBoxInit(SDL_Renderer *renderer, TTF_Font *font);
int addSprintToScreen(SDL_Renderer *renderer, std::string name, const char *imgPath);
void drawSpriteBoxScreen(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY);
void SpriteBoxClick(const int mouseX, const int mouseY);
void DrawTopBarOfSpriteBox(SDL_Renderer *renderer, TTF_Font *font, SDL_Rect topBar);
bool loadSpriteBoxMenuIcons(SDL_Renderer *renderer);
void DrawAddSpriteMenu ( SDL_Renderer *renderer, TTF_Font *font, SDL_Rect SpriteArea , const int mouseX, const int mouseY );
void DrawSpritePickerScreen(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY);
bool HandleSpritePickerClick(SDL_Renderer * renderer, const int mouseX, const int mouseY );
#endif
