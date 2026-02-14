/*
This cpp file is for handling UI component
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "ui.h"
#include "menubar.h"
#include <iostream>
#include "generaldef.h"
#include "color.h"

const SDL_Rect SCREEN = {0, 0, MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT};
const SDL_Rect MENUBAR_ROW = {0, 0, MAIN_WINDOW_WIDTH, MENUBAR_HEIGHT};
const SDL_Rect CATEGORY_COLUMN = {0, MENUBAR_HEIGHT, CATEGORY_COLUMN_WIDTH, MAIN_WINDOW_HEIGHT};
const SDL_Rect BLOCKS_COLUMN = {CATEGORY_COLUMN_WIDTH, MENUBAR_HEIGHT, CATEGORY_COLUMN_WIDTH + BLOCKS_COLUMN_WIDTH, MAIN_WINDOW_HEIGHT};
const SDL_Rect WORKSPACE_COLUMN = {CATEGORY_COLUMN_WIDTH + CATEGORY_COLUMN_WIDTH + BLOCKS_COLUMN_WIDTH, MENUBAR_HEIGHT, MAIN_WINDOW_WIDTH - (CATEGORY_COLUMN_WIDTH + CATEGORY_COLUMN_WIDTH + BLOCKS_COLUMN_WIDTH + CANVAS_SCREEN_WIDTH), MAIN_WINDOW_HEIGHT};
const SDL_Rect CANVAS_BOX = {MAIN_WINDOW_WIDTH - CANVAS_SCREEN_WIDTH, MENUBAR_HEIGHT, MAIN_WINDOW_WIDTH, CANVAS_SCREEN_HEIGTH};
const SDL_Rect SPRITE_BOX = {MAIN_WINDOW_WIDTH - CANVAS_SCREEN_WIDTH, CANVAS_SCREEN_HEIGTH + MENUBAR_HEIGHT, MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT};

const SDL_Rect fileMenuRect = {LOGO_MARGIN_LEFT + LOGO_WIDTH + 2 * MENU_MARGIN_LEFT, 5, 60, MENUBAR_HEIGHT - 10};
const SDL_Rect editMenuRect = {fileMenuRect.x + fileMenuRect.w + MENU_MARGIN_LEFT, 5, 60, MENUBAR_HEIGHT - 10};

const SDL_Rect SAVELOAD_BOX = {MAIN_WINDOW_WIDTH / 4, MAIN_WINDOW_HEIGHT / 3, MAIN_WINDOW_WIDTH / 2, MAIN_WINDOW_HEIGHT / 3};
const SDL_Rect LOGGER_Box = {
    CATEGORY_COLUMN_WIDTH + CATEGORY_COLUMN_WIDTH + BLOCKS_COLUMN_WIDTH,
    MAIN_WINDOW_HEIGHT - LOGGER_HEIGHT,
    MAIN_WINDOW_WIDTH - (CATEGORY_COLUMN_WIDTH + CATEGORY_COLUMN_WIDTH + BLOCKS_COLUMN_WIDTH + CANVAS_SCREEN_WIDTH),
    LOGGER_HEIGHT};
const SDL_Rect CODE_TAB_RECT = {
    WORKSPACE_COLUMN.x,
    MENUBAR_HEIGHT,
    90,
    30};

const SDL_Rect COSTUME_TAB_RECT = {
    WORKSPACE_COLUMN.x + 90,
    MENUBAR_HEIGHT,
    110,
    30};

const SDL_Rect SOUND_TAB_RECT = {
    WORKSPACE_COLUMN.x + 200,
    MENUBAR_HEIGHT,
    90,
    30};

/*
    This function render text
*/
SDL_Texture *renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, const SDL_Color &color)
{
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);
    if (!surface)
        return nullptr;
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

/*
    This function will check that a point is inside a rectangle or not
*/
bool isPointInRect(int x, int y, const SDL_Rect &rect)
{
    return (x >= rect.x) && (x <= (rect.x + rect.w)) && (y >= rect.y) && (y <= (rect.y + rect.h));
}

/*
-----------------------------------
Load an image from a file path
-----------------------------------
this will return texture of that photo.
*/
SDL_Texture *renderImage(SDL_Renderer *renderer, const char *imgPath)
{
    SDL_Surface *custome = IMG_Load(imgPath);
    if (!custome)
        return nullptr;

    SDL_Texture *imgText = SDL_CreateTextureFromSurface(renderer, custome);
    SDL_FreeSurface(custome);

    return imgText;
}
/*
this function draws a thick rectangle instead of the default sdl2 rectangle.
*/
void draw_thick_rect(SDL_Renderer *renderer, const SDL_Rect &rect, int thickness)
{
    SDL_Rect temp = rect;
    for (int i = 0; i < thickness; ++i)
    {
        SDL_RenderDrawRect(renderer, &temp);
        temp.x++;
        temp.y++;
        temp.w -= 2;
        temp.h -= 2;
    }
}
void drawEditorTabs(SDL_Renderer *renderer, TTF_Font *font, int mouseX, int mouseY)
{
    struct TabInfo
    {
        SDL_Rect rect;
        const char *label;
        EditorTab tab;
    };

    TabInfo tabs[] = {
        {CODE_TAB_RECT, "Code", TAB_CODE},
        {COSTUME_TAB_RECT, "Costume", TAB_COSTUME},
        {SOUND_TAB_RECT, "Sound", TAB_SOUND}};

    for (auto &t : tabs)
    {
        bool active = (currentTab == t.tab);

        if (active)
        {
            SDL_SetRenderDrawColor(renderer, color_activeTabGray);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, color_menuDropbox);
        }

        SDL_RenderFillRect(renderer, &t.rect);

        SDL_SetRenderDrawColor(renderer, color_borderMenuDropbox);
        SDL_RenderDrawRect(renderer, &t.rect);

        SDL_Texture *txt = renderText(renderer, font, t.label, color_black);
        if (txt)
        {
            int w, h;
            SDL_QueryTexture(txt, nullptr, nullptr, &w, &h);
            SDL_Rect tr = {
                t.rect.x + (t.rect.w - w) / 2,
                t.rect.y + (t.rect.h - h) / 2,
                w, h};
            SDL_RenderCopy(renderer, txt, nullptr, &tr);
        }
    }
}
void controlEditorTabClick(int mouseX, int mouseY)
{
    if (isPointInRect(mouseX, mouseY, CODE_TAB_RECT))
        currentTab = TAB_CODE;
    else if (isPointInRect(mouseX, mouseY, COSTUME_TAB_RECT))
        currentTab = TAB_COSTUME;
    else if (isPointInRect(mouseX, mouseY, SOUND_TAB_RECT))
        currentTab = TAB_SOUND;
}
