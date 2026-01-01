#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "category.h"
#include "color.h"
#include "ui.h"
#include "generaldef.h"

Category::Category(const char *f_name, SDL_Color f_color)
{
    name = f_name;
    color = f_color;
}

int selectedCategoryId = 0;

const int categoryCount = 4;
const SDL_Texture *categoriesText[categoryCount];

const Category categories[] = {
    Category("Event", colort_eventBlocks),
    Category("Control", colort_controlBlocks),
    Category("Motion", colort_motionBlocks),
    Category("Sound", colort_soundBlocks),
};

int categoryColumnInit(SDL_Renderer *renderer, TTF_Font *font)
{
    for (int i = 0; i < categoryCount; i++)
        categoriesText[i] = renderText(renderer, font, categories[i].name, categories[i].color);

    return 0;
}

void drawCatagoryColumn(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY)
{
}