/*
This header file is for handling block category column
*/
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <vector>

#ifndef CATEGORYCOLUMN_HEADER
#define CATEGORYCOLUMN_HEADER

#define CATEGORY_ITEM_HEIGTH 50

extern int selectedCategoryId;

enum CategoryId
{
    CATEGORY_EVENT,
    CATEGORY_CONTROL,
    CATEGORY_MOTION,
    CATEGORY_SOUND,
    CATEGORY_LOOKS,
    CATEGORY_OPERATOR,
    CATEGORY_VARIABLES,
};

struct Category
{
    const char *name;
    SDL_Color color;

    Category(const char *f_name, SDL_Color f_color);
};

extern SDL_Texture *categoriesText[];

// all categories will be here
extern std::vector<Category> categories;
#define CATEGORIES_COUNT (categories.size())

int categoryColumnInit(SDL_Renderer *renderer, TTF_Font *font);
void drawCatagoryColumn(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY);
void controlCategoryColumnClickDown(const int mouseX, const int mouseY);

#endif