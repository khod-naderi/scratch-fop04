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
    for ( const SprintBody &thisSprint : aliveSprints ){
        // visibility box 
        if ( !thisSprint.isVisible ) continue;
        // calculate scale size 
        int scaledW = SPRINT_IMG_WITDH * thisSprint.scale / 100;
        int scaledH = SPRINT_IMG_HEIGHT * thisSprint.scale / 100;

        if ( scaledW > CANVAS_BOX.w ) scaledW = CANVAS_BOX.w;
        if ( scaledH > CANVAS_BOX.h ) scaledH = CANVAS_BOX.h;

        scaledW = std::min(scaledW , CANVAS_BOX.w);
        scaledH = std::min(scaledH, CANVAS_BOX.h);

        SDL_Rect imageRect = {
            CANVAS_BOX.x + (thisSprint.posX - (scaledW / 2 ) ),
            CANVAS_BOX.y + (thisSprint.posY - (scaledH / 2 ) ),
            scaledW,
            scaledH,
        };
        SDL_RenderCopyEx(renderer, thisSprint.nowCustome , NULL, &imageRect, thisSprint.angleRotation, NULL, SDL_FLIP_NONE );
    }
}