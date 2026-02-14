#include "sound_ui.h"
#include "ui.h"
#include "generaldef.h"
#include "color.h"

#include <SDL2/SDL_audio.h>
#include <filesystem>
#include <vector>
#include <string>

/* -------------------------------------------------
   Sound asset
------------------------------------------------- */
struct SoundAsset
{
    std::string name;
    std::string path;

    SDL_AudioSpec spec{};
    Uint8 *buffer = nullptr;
    Uint32 length = 0;

    int volume = 100;
};

static std::vector<SoundAsset> sounds;
static int selectedSound = 0;
static int hoveredSound = -1;

static bool scanned = false;
static bool loaded = false;

/* -------------------------------------------------
   SDL audio device
------------------------------------------------- */
static SDL_AudioDeviceID audioDevice = 0;
static SDL_AudioSpec deviceSpec{};

/* -------------------------------------------------
   Scan assets/sounds once
------------------------------------------------- */
static void scanSoundsOnce()
{
    if (scanned)
        return;

    char *base = SDL_GetBasePath();
    if (!base)
        return;

    std::filesystem::path dir =
        std::filesystem::path(base) / "../assets/sounds";

    SDL_free(base);

    if (!std::filesystem::exists(dir))
    {
        scanned = true;
        return;
    }

    for (const auto &e : std::filesystem::directory_iterator(dir))
    {
        if (e.is_regular_file() && e.path().extension() == ".wav")
        {
            SoundAsset s;
            s.name = e.path().filename().string();
            s.path = e.path().string();
            sounds.push_back(s);
        }
    }

    scanned = true;
}

/* -------------------------------------------------
   Load WAVs once
------------------------------------------------- */
static void loadWavsOnce()
{
    if (loaded)
        return;

    for (auto &s : sounds)
    {
        SDL_LoadWAV(
            s.path.c_str(),
            &s.spec,
            &s.buffer,
            &s.length);
    }

    loaded = true;
}

/* -------------------------------------------------
   Play sound
------------------------------------------------- */
static void playSound(const SoundAsset &s)
{
    if (!s.buffer || s.length == 0)
        return;

    if (!audioDevice)
    {
        audioDevice = SDL_OpenAudioDevice(
            nullptr,
            0,
            &s.spec,
            &deviceSpec,
            0);
    }

    if (!audioDevice)
        return;

    SDL_ClearQueuedAudio(audioDevice);

    std::vector<Uint8> mixed(s.length);
    SDL_MixAudioFormat(
        mixed.data(),
        s.buffer,
        deviceSpec.format,
        s.length,
        (SDL_MIX_MAXVOLUME * s.volume) / 100);

    SDL_QueueAudio(audioDevice, mixed.data(), mixed.size());
    SDL_PauseAudioDevice(audioDevice, 0);
}

/* -------------------------------------------------
   Stop sound
------------------------------------------------- */
static void stopSound()
{
    if (audioDevice)
        SDL_ClearQueuedAudio(audioDevice);
}

/* -------------------------------------------------
   Button helper
------------------------------------------------- */
static void drawButton(
    SDL_Renderer *renderer,
    TTF_Font *font,
    const SDL_Rect &rect,
    const char *label,
    bool hovered)
{
    SDL_SetRenderDrawColor(
        renderer,
        hovered ? color_hoveredMenuOption : SDL_Color{220, 220, 220, 255});
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
    scanSoundsOnce();
    loadWavsOnce();

    const int TAB_BAR_HEIGHT = 30;
    const int SOUND_LIST_WIDTH = 180;
    const int BUTTON_HEIGHT = 32;
    const int ITEM_HEIGHT = 28;

    /* ✅ NON‑NEGOTIABLE GEOMETRY */
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

    hoveredSound = -1;
    int itemY = soundList.y + 20;

    for (size_t i = 0; i < sounds.size(); ++i)
    {
        SDL_Rect r = {
            soundList.x + 5,
            itemY,
            soundList.w - 10,
            ITEM_HEIGHT};

        bool hovered = isPointInRect(mouseX, mouseY, r);
        if (hovered)
            hoveredSound = (int)i;

        SDL_SetRenderDrawColor(
            renderer,
            (int)i == selectedSound
                ? SDL_Color{200, 220, 255, 255}
            : hovered ? color_hoveredMenuOption
                      : SDL_Color{245, 245, 245, 255});

        SDL_RenderFillRect(renderer, &r);

        SDL_Texture *t =
            renderText(renderer, font, sounds[i].name.c_str(), color_black);
        if (t)
        {
            int w, h;
            SDL_QueryTexture(t, nullptr, nullptr, &w, &h);
            SDL_Rect tr = {r.x + 10, r.y + (r.h - h) / 2, w, h};
            SDL_RenderCopy(renderer, t, nullptr, &tr);
            SDL_DestroyTexture(t);
        }

        if (hovered &&
            (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)))
        {
            selectedSound = (int)i;
        }

        itemY += ITEM_HEIGHT + 4;
    }

    if (sounds.empty())
        return;

    /* ---------------------------
       Editor (RIGHT)
    --------------------------- */
    SDL_Rect editor = {
        soundList.x + soundList.w,
        editorArea.y,
        editorArea.w - soundList.w,
        editorArea.h};

    SDL_SetRenderDrawColor(renderer, color_white);
    SDL_RenderFillRect(renderer, &editor);
    SDL_SetRenderDrawColor(renderer, color_borderMenuDropbox);
    SDL_RenderDrawRect(renderer, &editor);

    int cx = editor.x + editor.w / 2;
    int my = editor.y + editor.h / 2 - 60;

    /* File name */
    SDL_Texture *nameText =
        renderText(renderer, font, sounds[selectedSound].name.c_str(), color_black);
    if (nameText)
    {
        int w, h;
        SDL_QueryTexture(nameText, nullptr, nullptr, &w, &h);
        SDL_Rect tr = {cx - w / 2, my - h - 20, w, h};
        SDL_RenderCopy(renderer, nameText, nullptr, &tr);
        SDL_DestroyTexture(nameText);
    }

    /* Play / Stop */
    SDL_Rect playBtn = {cx - 120, my, 100, 44};
    SDL_Rect stopBtn = {cx + 20, my, 100, 44};

    bool playHover = isPointInRect(mouseX, mouseY, playBtn);
    bool stopHover = isPointInRect(mouseX, mouseY, stopBtn);

    drawButton(renderer, font, playBtn, "Play", playHover);
    drawButton(renderer, font, stopBtn, "Stop", stopHover);

    if (playHover &&
        (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)))
    {
        playSound(sounds[selectedSound]);
    }

    if (stopHover &&
        (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)))
    {
        stopSound();
    }

    /* ---------------------------
       Volume slider
    --------------------------- */
    int volY = my + 44 + 35;

    SDL_Texture *volText =
        renderText(renderer, font, "Volume", color_black);
    if (volText)
    {
        int w, h;
        SDL_QueryTexture(volText, nullptr, nullptr, &w, &h);
        SDL_Rect tr = {cx - w / 2, volY, w, h};
        SDL_RenderCopy(renderer, volText, nullptr, &tr);
        SDL_DestroyTexture(volText);
    }

    SDL_Rect sliderBar = {cx - 160, volY + 30, 320, 10};

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(renderer, &sliderBar);
    SDL_SetRenderDrawColor(renderer, color_borderMenuDropbox);
    SDL_RenderDrawRect(renderer, &sliderBar);

    if (isPointInRect(mouseX, mouseY, sliderBar) &&
        (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)))
    {
        int rel = mouseX - sliderBar.x;
        if (rel < 0)
            rel = 0;
        if (rel > sliderBar.w)
            rel = sliderBar.w;

        sounds[selectedSound].volume =
            (rel * 100) / sliderBar.w;
    }

    SDL_Rect knob = {
        sliderBar.x +
            (sliderBar.w * sounds[selectedSound].volume) / 100 - 8,
        sliderBar.y - 6,
        16,
        22};

    SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255);
    SDL_RenderFillRect(renderer, &knob);
    SDL_RenderDrawRect(renderer, &knob);
}
