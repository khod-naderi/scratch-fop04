#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "category.h"
#include "color.h"

int selectedCategoryId = 0;

const int categoryCount = 4;
extern const Category categories[] = {
    Category("Event", colort_eventBlocks),
    Category("Control", colort_controlBlocks),
    Category("Motion", colort_motionBlocks),
    Category("Sound", colort_soundBlocks),
};