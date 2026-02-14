#include "sound_ui.h"
#include "ui.h"
#include "color.h"
#include "generaldef.h"

void drawSoundEditor(
    SDL_Renderer *renderer,
    TTF_Font *font,
    int mouseX,
    int mouseY)
{
    // Background (reuse workspace area)
    SDL_Rect background = {
        WORKSPACE_COLUMN.x,
        MENUBAR_HEIGHT,
        WORKSPACE_COLUMN.w,
        WORKSPACE_COLUMN.h};

    SDL_SetRenderDrawColor(renderer, color_menuDropbox);
    SDL_RenderFillRect(renderer, &background);

    SDL_Texture *title =
        renderText(renderer, font, "Sound Editor", color_black);

    if (title)
    {
        int w, h;
        SDL_QueryTexture(title, nullptr, nullptr, &w, &h);
        SDL_Rect tr = {
            background.x + 20,
            background.y + 20,
            w,
            h};
        SDL_RenderCopy(renderer, title, nullptr, &tr);
    }
}
