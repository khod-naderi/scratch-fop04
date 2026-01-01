/*
This header file is for handling block category column
*/
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#ifndef CATEGORYCOLUMN_HEADER
#define CATEGORYCOLUMN_HEADER

extern int selectedCategoryId;
extern const int categoryCount;

struct Category
{
    const char *name;
    SDL_Color color;

    Category(const char *f_name, SDL_Color f_color);
};

extern const SDL_Texture *categoriesText[];

// all categories will be here
extern const Category categories[];

int categoryColumnInit(SDL_Renderer *renderer, TTF_Font *font);
void drawCatagoryColumn(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY);

#endif