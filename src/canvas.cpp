/*
-------------------------------------------
This CPP file is for managing stage view named canvas
-------------------------------------------
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "canvas.h"
#include "color.h"
#include "ui.h"
#include "spritebox.h"
#include <vector>

std::vector<SprintBody> aliveSprints;

/*
-------------------------------------------------
This function is for drawing Canvas Screen.
this function will run on every frame.
-------------------------------------------------
*/
void drawCanvasScreen(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY)
{
    // Draw border of Canvas
    SDL_SetRenderDrawColor(renderer, color_black);
    SDL_RenderDrawRect(renderer, &CANVAS_BOX);

    /*
    ------------------
    Draw Sprints on screen
    ------------------
    */
    for (SprintBody thisSprint : aliveSprints)
    {
        SDL_Rect imageRect = {
            CANVAS_BOX.x + (thisSprint.posX - (SPRINT_IMG_WITDH / 2)),
            CANVAS_BOX.y + (thisSprint.posY - (SPRINT_IMG_HEIGHT / 2)),
            SPRINT_IMG_WITDH,
            SPRINT_IMG_HEIGHT,
        };
        SDL_RenderCopy(renderer, thisSprint.nowCustome, NULL, &imageRect);
    }
}