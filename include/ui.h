/*
This header file is for handling UI component
*/

#include <SDL2/SDL.h>
#include "menubar.h"
#include "generaldef.h"
#include "block.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "category.h"

#ifndef UI_HEADER
#define UI_HEADER

#define CATEGORY_COLUMN_WIDTH 80
#define BLOCKS_COLUMN_WIDTH 200
#define CANVAS_SCREEN_WIDTH 500
#define CANVAS_SCREEN_HEIGTH 500

#define SCROLL_Y_SENSIVITY 5

extern const SDL_Rect MENUBAR_ROW;
extern const SDL_Rect CATEGORY_COLUMN;
extern const SDL_Rect BLOCKS_COLUMN;
extern const SDL_Rect WORKSPACE_COLUMN;
extern const SDL_Rect CANVAS_BOX;
extern const SDL_Rect SPRITE_BOX;

SDL_Texture *renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color);
SDL_Texture *renderImage(SDL_Renderer *renderer, const char *imgPath);
bool isPointInRect(int x, int y, SDL_Rect rect);

void render_categories(SDL_Renderer *renderer, Category categories[], int count, int selected);
void render_blocks(SDL_Renderer *renderer, Block blocks[], int count, float scroll_offset, SDL_Rect viewport);
void render_extension_button(SDL_Renderer *renderer, SDL_Rect bounds);

// Define menu reactangles
extern const SDL_Rect fileMenuRect;
extern const SDL_Rect editMenuRect;

#endif