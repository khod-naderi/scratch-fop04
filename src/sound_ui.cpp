#include "sound_ui.h"
#include "ui.h"
#include "generaldef.h"
#include "color.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/* -------------------------------------------------
   Temporary sound data
------------------------------------------------- */
static const char *soundNames[] = {
    "pop.wav",
    "meow.wav",
    "laser.wav"};

static const int SOUND_COUNT = 3;

static int selectedSound = 0;
static int hoveredSound = -1;
static int volumeValue = 50;

/* -------------------------------------------------
   Helper: draw a simple button
------------------------------------------------- */
static void drawButton(
    SDL_Renderer *renderer,
    TTF_Font *font,
    const SDL_Rect &rect,
    const char *label,
    bool hovered)
{
    if (hovered)
        SDL_SetRenderDrawColor(renderer, color_hoveredMenuOption);
    else
        SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);

    SDL_RenderFillRect(renderer, &rect);

    SDL_SetRenderDrawColor(renderer, color_borderMenuDropbox);
    SDL_RenderDrawRect(renderer, &rect);

    SDL_Texture *txt = renderText(renderer, font, label, color_black);
    if (txt)
    {
        int w, h;
        SDL_QueryTexture(txt, nullptr, nullptr, &w, &h);
        SDL_Rect tr = {
            rect.x + (rect.w - w) / 2,
            rect.y + (rect.h - h) / 2,
            w,
            h};
        SDL_RenderCopy(renderer, txt, nullptr, &tr);
        SDL_DestroyTexture(txt);
    }
}

/* -------------------------------------------------
   Main Sound Editor UI
------------------------------------------------- */
void drawSoundEditor(
    SDL_Renderer *renderer,
    TTF_Font *font,
    int mouseX,
    int mouseY)
{
    const int TAB_BAR_HEIGHT = 30;
    const int SOUND_LIST_WIDTH = 180;
    const int BUTTON_HEIGHT = 32;
    const int ITEM_HEIGHT = 28;

    /* REQUIRED editor area — unchanged geometry */
    SDL_Rect editorArea = {
        0,
        MENUBAR_HEIGHT + TAB_BAR_HEIGHT,
        MAIN_WINDOW_WIDTH - CANVAS_SCREEN_WIDTH,
        MAIN_WINDOW_HEIGHT - MENUBAR_HEIGHT - TAB_BAR_HEIGHT};

    SDL_SetRenderDrawColor(renderer, color_menuDropbox);
    SDL_RenderFillRect(renderer, &editorArea);

    /* ---------------------------
       Sound list (LEFT)
    --------------------------- */
    SDL_Rect soundList = {
        editorArea.x,
        editorArea.y,
        SOUND_LIST_WIDTH,
        editorArea.h};

    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    SDL_RenderFillRect(renderer, &soundList);

    SDL_SetRenderDrawColor(renderer, color_borderMenuDropbox);
    SDL_RenderDrawRect(renderer, &soundList);

    SDL_Texture *listTitle =
        renderText(renderer, font, "Sounds", color_black);

    if (listTitle)
    {
        int w, h;
        SDL_QueryTexture(listTitle, nullptr, nullptr, &w, &h);
        SDL_Rect tr = {
            soundList.x + 10,
            soundList.y + 10,
            w,
            h};
        SDL_RenderCopy(renderer, listTitle, nullptr, &tr);
        SDL_DestroyTexture(listTitle);
    }

    /* ---------------------------
       Sound items
    --------------------------- */
    hoveredSound = -1;
    int itemY = soundList.y + 45;

    for (int i = 0; i < SOUND_COUNT; ++i)
    {
        SDL_Rect itemRect = {
            soundList.x + 5,
            itemY,
            soundList.w - 10,
            ITEM_HEIGHT};

        bool hovered = isPointInRect(mouseX, mouseY, itemRect);
        if (hovered)
            hoveredSound = i;

        if (i == selectedSound)
            SDL_SetRenderDrawColor(renderer, 200, 220, 255, 255);
        else if (hovered)
            SDL_SetRenderDrawColor(renderer, color_hoveredMenuOption);
        else
            SDL_SetRenderDrawColor(renderer, 245, 245, 245, 255);

        SDL_RenderFillRect(renderer, &itemRect);

        SDL_Texture *txt =
            renderText(renderer, font, soundNames[i], color_black);

        if (txt)
        {
            int w, h;
            SDL_QueryTexture(txt, nullptr, nullptr, &w, &h);
            SDL_Rect tr = {
                itemRect.x + 10,
                itemRect.y + (itemRect.h - h) / 2,
                w,
                h};
            SDL_RenderCopy(renderer, txt, nullptr, &tr);
            SDL_DestroyTexture(txt);
        }

        if (hovered &&
            (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)))
        {
            selectedSound = i;
        }

        itemY += ITEM_HEIGHT + 4;
    }

    /* ---------------------------
       Upload button
    --------------------------- */
    SDL_Rect uploadBtn = {
        soundList.x + 10,
        soundList.y + soundList.h - BUTTON_HEIGHT - 10,
        soundList.w - 20,
        BUTTON_HEIGHT};

    drawButton(
        renderer,
        font,
        uploadBtn,
        "+ Upload Sound",
        isPointInRect(mouseX, mouseY, uploadBtn));

    /* ---------------------------
       Sound editor (RIGHT)
    --------------------------- */
    SDL_Rect soundEditor = {
        soundList.x + soundList.w,
        editorArea.y,
        editorArea.w - soundList.w,
        editorArea.h};

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &soundEditor);

    SDL_SetRenderDrawColor(renderer, color_borderMenuDropbox);
    SDL_RenderDrawRect(renderer, &soundEditor);
    /* ---------------------------
       Centered mixer controls
    --------------------------- */
    int centerX = soundEditor.x + soundEditor.w / 2;
    int mixerStartY = soundEditor.y + (soundEditor.h - 180) / 2;

    /* BIGGER Play / Stop buttons */
    SDL_Rect playBtn = {
        centerX - 120,
        mixerStartY,
        100, // was 80
        44}; // was BUTTON_HEIGHT (32)

    SDL_Rect stopBtn = {
        centerX + 20,
        mixerStartY,
        100, // was 80
        44};

    drawButton(renderer, font, playBtn, "Play",
               isPointInRect(mouseX, mouseY, playBtn));
    drawButton(renderer, font, stopBtn, "Stop",
               isPointInRect(mouseX, mouseY, stopBtn));

    /* Volume label */
    int volY = mixerStartY + 44 + 35;

    SDL_Texture *volText =
        renderText(renderer, font, "Volume", color_black);

    if (volText)
    {
        int w, h;
        SDL_QueryTexture(volText, nullptr, nullptr, &w, &h);
        SDL_Rect tr = {
            centerX - w / 2,
            volY,
            w,
            h};
        SDL_RenderCopy(renderer, volText, nullptr, &tr);
        SDL_DestroyTexture(volText);
    }

    /* BIGGER slider bar */
    SDL_Rect sliderBar = {
        centerX - 160,
        volY + 30,
        320, // was 220
        10}; // was 6

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(renderer, &sliderBar);

    SDL_SetRenderDrawColor(renderer, color_borderMenuDropbox);
    SDL_RenderDrawRect(renderer, &sliderBar);

    /* Slider interaction unchanged */
    if (isPointInRect(mouseX, mouseY, sliderBar) &&
        (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)))
    {
        int rel = mouseX - sliderBar.x;
        if (rel < 0)
            rel = 0;
        if (rel > sliderBar.w)
            rel = sliderBar.w;
        volumeValue = (rel * 100) / sliderBar.w;
    }

    /* BIGGER knob */
    SDL_Rect sliderKnob = {
        sliderBar.x + (sliderBar.w * volumeValue) / 100 - 8,
        sliderBar.y - 6,
        16,  // was 12
        22}; // was 14

    SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255);
    SDL_RenderFillRect(renderer, &sliderKnob);
    SDL_RenderDrawRect(renderer, &sliderKnob);
}
