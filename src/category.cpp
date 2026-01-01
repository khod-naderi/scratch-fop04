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
SDL_Texture *categoriesText[categoryCount];

const Category categories[] = {
    Category("Event", colort_eventBlocks),
    Category("Control", colort_controlBlocks),
    Category("Motion", colort_motionBlocks),
    Category("Sound", colort_soundBlocks),
};

// initialize category column
int categoryColumnInit(SDL_Renderer *renderer, TTF_Font *font)
{
    for (int i = 0; i < categoryCount; i++)
        categoriesText[i] = renderText(renderer, font, categories[i].name, color_white);

    return 0;
}

void drawCatagoryColumn(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY)
{
    for (int i = 0; i < categoryCount; i++)
    {
        SDL_Rect itemRect = {
            CATEGORY_COLUMN.x,
            CATEGORY_COLUMN.y + i * CATEGORY_ITEM_HEIGTH,
            CATEGORY_COLUMN_WIDTH,
            CATEGORY_ITEM_HEIGTH,
        };

        // Background
        if (i == selectedCategoryId)
        {
            SDL_SetRenderDrawColor(renderer, colorDim(categories[i].color));
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, categories[i].color);
        }
        SDL_RenderFillRect(renderer, &itemRect);

        // border
        SDL_SetRenderDrawColor(renderer, color_borderMenuDropbox);
        SDL_RenderDrawRect(renderer, &itemRect);

        // text
        int tw, th;
        SDL_QueryTexture(categoriesText[i], NULL, NULL, &tw, &th);
        SDL_Rect textRect = {
            itemRect.x + (itemRect.w - tw) / 2,
            itemRect.y + (itemRect.h - th) / 2,
            tw,
            th,
        };
        SDL_RenderCopy(renderer, categoriesText[i], NULL, &textRect);
    }
}

// this will control event of click-down
void controlCategoryColumnClickDown(const int mouseX, const int mouseY)
{
    if (isPointInRect(mouseX, mouseY, CATEGORY_COLUMN))
    {
        int selectedIndex = (mouseY - CATEGORY_COLUMN.y) / CATEGORY_ITEM_HEIGTH;
        if (selectedIndex < categoryCount)
            selectedCategoryId = selectedIndex;
    }
}