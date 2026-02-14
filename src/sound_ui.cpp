#include "sound_ui.h"
#include "ui.h"
#include "generaldef.h"
#include "color.h"

void drawSoundEditor(SDL_Renderer *renderer, TTF_Font *font, int mouseX, int mouseY)
{
    // Editor tabs height (same as in ui.cpp)
    const int TAB_HEIGHT = 30;

    // Main sound editor area (same column as workspace)
    SDL_Rect editorArea = {
        WORKSPACE_COLUMN.x,
        MENUBAR_HEIGHT + TAB_HEIGHT,
        WORKSPACE_COLUMN.w,
        MAIN_WINDOW_HEIGHT - (MENUBAR_HEIGHT + TAB_HEIGHT)};

    // Background
    SDL_SetRenderDrawColor(renderer, 245, 245, 245, 255);
    SDL_RenderFillRect(renderer, &editorArea);

    // Title: "Sounds"
    SDL_Texture *title = renderText(renderer, font, "Sounds", color_black);
    if (title)
    {
        int w, h;
        SDL_QueryTexture(title, nullptr, nullptr, &w, &h);
        SDL_Rect titleRect = {
            editorArea.x + 15,
            editorArea.y + 10,
            w, h};
        SDL_RenderCopy(renderer, title, nullptr, &titleRect);
        SDL_DestroyTexture(title);
    }

    // Placeholder panel (future: sound list / waveform / mixer)
    SDL_Rect panel = {
        editorArea.x + 15,
        editorArea.y + 45,
        editorArea.w - 30,
        editorArea.h - 60};

    SDL_SetRenderDrawColor(renderer, 230, 230, 230, 255);
    SDL_RenderFillRect(renderer, &panel);

    SDL_SetRenderDrawColor(renderer, 160, 160, 160, 255);
    SDL_RenderDrawRect(renderer, &panel);

    // Hint text
    SDL_Texture *hint = renderText(
        renderer,
        font,
        "Sound editor",
        {120, 120, 120, 255});

    if (hint)
    {
        int w, h;
        SDL_QueryTexture(hint, nullptr, nullptr, &w, &h);
        SDL_Rect hintRect = {
            panel.x + (panel.w - w) / 2,
            panel.y + (panel.h - h) / 2,
            w, h};
        SDL_RenderCopy(renderer, hint, nullptr, &hintRect);
        SDL_DestroyTexture(hint);
    }
}
