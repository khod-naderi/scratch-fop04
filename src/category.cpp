#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "category.h"
#include "color.h"
#include "ui.h"
#include "generaldef.h"
#include "block.h"

Category::Category(const char *f_name, SDL_Color f_color)
{
    name = f_name;
    color = f_color;
}

std::vector<Category> categories = {
    Category("Event", colort_eventBlocks),
    Category("Control", colort_controlBlocks),
    Category("Motion", colort_motionBlocks),
    Category("Sound", colort_soundBlocks),
    Category("Looks", colort_soundBlocks),
    Category("Operator", colort_soundBlocks),
    Category("Variables", colort_soundBlocks),
};

int selectedCategoryId = 0;

SDL_Texture *addExtentionBtnText;

// initialize category column
int categoryColumnInit(SDL_Renderer *renderer, TTF_Font *font)
{
    for (int i = 0; i < CATEGORIES_COUNT; i++)
        categories[i].texture = renderText(renderer, font, categories[i].name, color_white);

    addExtentionBtnText = renderText(renderer, font, "Add Extra", color_white);
    return 0;
}

void drawCatagoryColumn(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY)
{

    // Draw column background
    SDL_SetRenderDrawColor(renderer, color_categoryColumneBackground);
    SDL_RenderFillRect(renderer, &CATEGORY_COLUMN);

    // Draw each item
    for (int i = 0; i < CATEGORIES_COUNT; i++)
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
        SDL_QueryTexture(categories[i].texture, NULL, NULL, &tw, &th);
        SDL_Rect textRect = {
            itemRect.x + (itemRect.w - tw) / 2,
            itemRect.y + (itemRect.h - th) / 2,
            tw,
            th,
        };
        SDL_RenderCopy(renderer, categories[i].texture, NULL, &textRect);
    }

    /*
    --------------------------------
    Draw Add extention button
    --------------------------------
    */
    SDL_Rect addExtentionBtnRect = {
        CATEGORY_COLUMN.x,
        MAIN_WINDOW_HEIGHT - CATEGORY_ITEM_HEIGTH - 10,
        CATEGORY_COLUMN_WIDTH,
        CATEGORY_ITEM_HEIGTH,
    };

    // Background
    if (isPointInRect(mouseX, mouseY, addExtentionBtnRect))
        SDL_SetRenderDrawColor(renderer, colorDim(color_addExtentionBtn));
    else
        SDL_SetRenderDrawColor(renderer, color_addExtentionBtn);

    SDL_RenderFillRect(renderer, &addExtentionBtnRect);

    // border
    SDL_SetRenderDrawColor(renderer, color_borderMenuDropbox);
    SDL_RenderDrawRect(renderer, &addExtentionBtnRect);

    // text
    int tw, th;
    SDL_QueryTexture(addExtentionBtnText, NULL, NULL, &tw, &th);
    SDL_Rect addExtentionBtnTextRect = {
        addExtentionBtnRect.x + (addExtentionBtnRect.w - tw) / 2,
        addExtentionBtnRect.y + (addExtentionBtnRect.h - th) / 2,
        tw,
        th,
    };
    SDL_RenderCopy(renderer, addExtentionBtnText, NULL, &addExtentionBtnTextRect);
}

// this will control event of click-down
void controlCategoryColumnClickDown(const int mouseX, const int mouseY)
{
    if (isPointInRect(mouseX, mouseY, CATEGORY_COLUMN))
    {
        int selectedIndex = (mouseY - CATEGORY_COLUMN.y) / CATEGORY_ITEM_HEIGTH;
        if (selectedIndex < CATEGORIES_COUNT)
        {
            selectedCategoryId = selectedIndex;
            scrollBlockColumnIndex = 0;
        }
    }
}