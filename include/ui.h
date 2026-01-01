/*
This header file is for handling UI component
*/

#include <SDL2/SDL.h>
#include "menubar.h"
#include "generaldef.h"
#include "block.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#ifndef UI_HEADER
#define UI_HEADER

#define CATEGORY_COLUMN_WIDTH 80
#define BLOCKS_COLUMN_WIDTH 200
#define CANVAS_SCREEN_WIDTH 500
#define CANVAS_SCREEN_HEIGTH 500

const SDL_Rect MENUBAR_ROW = {0, 0, MAIN_WINDOW_WIDTH, MENUBAR_HEIGHT};
const SDL_Rect CATEGORY_COLUMN = {0, MENUBAR_HEIGHT, CATEGORY_COLUMN_WIDTH, MAIN_WINDOW_HEIGHT};
const SDL_Rect BLOCKS_COLUMN = {CATEGORY_COLUMN_WIDTH, MENUBAR_HEIGHT, CATEGORY_COLUMN_WIDTH + BLOCKS_COLUMN_WIDTH, MAIN_WINDOW_HEIGHT};
const SDL_Rect WORKSPACE_COLUMN = {CATEGORY_COLUMN_WIDTH + BLOCKS_COLUMN_WIDTH, MENUBAR_HEIGHT, MAIN_WINDOW_WIDTH - CANVAS_SCREEN_WIDTH, MAIN_WINDOW_HEIGHT};
const SDL_Rect CANVAS_BOX = {MAIN_WINDOW_WIDTH - CANVAS_SCREEN_WIDTH, MENUBAR_HEIGHT, MAIN_WINDOW_WIDTH, CANVAS_SCREEN_HEIGTH + MENUBAR_HEIGHT};
const SDL_Rect SPRITE_BOX = {MAIN_WINDOW_WIDTH - CANVAS_SCREEN_WIDTH, CANVAS_SCREEN_HEIGTH + MENUBAR_HEIGHT, MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT};

SDL_Texture *renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color);
bool isPointInRect(int x, int y, SDL_Rect rect);

void render_categories(SDL_Renderer *renderer, Category categories[], int count, int selected);
void render_blocks(SDL_Renderer *renderer, Block blocks[], int count, float scroll_offset, SDL_Rect viewport);
void render_extension_button(SDL_Renderer *renderer, SDL_Rect bounds);

/*
---------------------------------------------
Importing Image Logo
in this part we load logo image for menubar
---------------------------------------------
*/
// Import logo image
extern SDL_Surface *logoSurface;
extern SDL_Texture *logoTexture;
extern SDL_Texture *fileMenuText;
extern SDL_Texture *editMenuText;

// Define menu reactangles
const SDL_Rect fileMenuRect = {LOGO_MARGIN_LEFT + LOGO_WIDTH + 2 * MENU_MARGIN_LEFT, 5, 60, MENUBAR_HEIGHT - 10};
const SDL_Rect editMenuRect = {fileMenuRect.x + fileMenuRect.w + MENU_MARGIN_LEFT, 5, 60, MENUBAR_HEIGHT - 10};

#endif