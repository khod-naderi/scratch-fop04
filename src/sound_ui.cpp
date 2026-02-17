#include "sound_ui.h"
#include "ui.h"
#include "generaldef.h"
#include "color.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_ttf.h>
extern "C"
{
#include "tinyfiledialogs.h"
}

#include <filesystem>
#include <vector>
#include <string>

/* =========================================================
   Sound Asset
========================================================= */
struct SoundAsset
{
    std::string name;
    std::string path;

    SDL_AudioSpec spec{};
    Uint8 *buffer = nullptr;
    Uint32 length = 0;

    int volume = 100;
};

/* =========================================================
   State
========================================================= */
static std::vector<SoundAsset> sounds;
static int selectedSound = -1;

static bool scanned = false;
static bool loaded = false;

static SDL_AudioDeviceID audioDevice = 0;
static SDL_AudioSpec deviceSpec{};

/* =========================================================
   Scan assets/sounds once
========================================================= */
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

    if (std::filesystem::exists(dir))
    {
        for (auto &e : std::filesystem::directory_iterator(dir))
        {
            if (e.is_regular_file() && e.path().extension() == ".wav")
            {
                SoundAsset s;
                s.name = e.path().filename().string();
                s.path = e.path().string();
                sounds.push_back(s);
            }
        }
    }

    if (!sounds.empty())
        selectedSound = 0;

    scanned = true;
}

/* =========================================================
   Load WAVs once
========================================================= */
static void loadWavsOnce()
{
    if (loaded)
        return;

    for (auto &s : sounds)
        SDL_LoadWAV(s.path.c_str(), &s.spec, &s.buffer, &s.length);

    loaded = true;
}

/* =========================================================
   Audio
========================================================= */
static void playSound(const SoundAsset &s)
{
    if (!s.buffer || s.length == 0)
        return;

    if (!audioDevice)
        audioDevice = SDL_OpenAudioDevice(nullptr, 0, &s.spec, &deviceSpec, 0);

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

static void stopSound()
{
    if (audioDevice)
        SDL_ClearQueuedAudio(audioDevice);
}

/* =========================================================
   Open File Explorer (tinyfiledialogs)
========================================================= */
static std::string openWavFileDialog()
{
    const char *filters[] = {"*.wav"};

    const char *path = tinyfd_openFileDialog(
        "Select WAV file",
        "",
        1,
        filters,
        "WAV files",
        0);

    if (!path)
        return "";

    return std::string(path);
}

/* =========================================================
   Import WAV into assets/sounds
========================================================= */
static void importWav(const std::string &srcPath)
{
    std::filesystem::path src(srcPath);
    if (src.extension() != ".wav")
        return;

    char *base = SDL_GetBasePath();
    if (!base)
        return;

    std::filesystem::path dstDir =
        std::filesystem::path(base) / "../assets/sounds";

    SDL_free(base);

    std::filesystem::create_directories(dstDir);
    std::filesystem::path dst = dstDir / src.filename();

    try
    {
        std::filesystem::copy_file(
            src, dst,
            std::filesystem::copy_options::overwrite_existing);
    }
    catch (...)
    {
        return;
    }

    SoundAsset s;
    s.name = dst.filename().string();
    s.path = dst.string();

    if (SDL_LoadWAV(s.path.c_str(), &s.spec, &s.buffer, &s.length))
    {
        sounds.push_back(s);
        selectedSound = (int)sounds.size() - 1;
    }
}

/* =========================================================
   Button helper
========================================================= */
static void drawButton(
    SDL_Renderer *r,
    TTF_Font *font,
    const SDL_Rect &rect,
    const char *text,
    bool hovered)
{
    SDL_SetRenderDrawColor(
        r,
        hovered ? 200 : 220,
        hovered ? 200 : 220,
        hovered ? 255 : 220,
        255);
    SDL_RenderFillRect(r, &rect);
    SDL_RenderDrawRect(r, &rect);

    SDL_Texture *t = renderText(r, font, text, color_black);
    if (t)
    {
        int w, h;
        SDL_QueryTexture(t, nullptr, nullptr, &w, &h);
        SDL_Rect tr{
            rect.x + (rect.w - w) / 2,
            rect.y + (rect.h - h) / 2,
            w, h};
        SDL_RenderCopy(r, t, nullptr, &tr);
        SDL_DestroyTexture(t);
    }
}

/* =========================================================
   MAIN UI
========================================================= */
void drawSoundEditor(
    SDL_Renderer *renderer,
    TTF_Font *font,
    int mouseX,
    int mouseY)
{
    scanSoundsOnce();
    loadWavsOnce();

    const int SOUND_LIST_WIDTH = 180;
    const int BUTTON_HEIGHT = 32;

    SDL_Rect editorArea{
        0,
        MENUBAR_HEIGHT,
        MAIN_WINDOW_WIDTH - CANVAS_SCREEN_WIDTH,
        MAIN_WINDOW_HEIGHT - MENUBAR_HEIGHT};

    SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
    SDL_RenderFillRect(renderer, &editorArea);

    /* ---------- Sound List ---------- */
    SDL_Rect list{
        editorArea.x,
        editorArea.y,
        SOUND_LIST_WIDTH,
        editorArea.h};

    SDL_SetRenderDrawColor(renderer, 245, 245, 245, 255);
    SDL_RenderFillRect(renderer, &list);
    SDL_RenderDrawRect(renderer, &list);

    int y = list.y + 20;
    for (size_t i = 0; i < sounds.size(); ++i)
    {
        SDL_Rect r{list.x + 10, y, list.w - 20, 28};
        bool hover = isPointInRect(mouseX, mouseY, r);

        SDL_SetRenderDrawColor(
            renderer,
            (int)i == selectedSound ? 190 : hover ? 210
                                                  : 235,
            210,
            255,
            255);
        SDL_RenderFillRect(renderer, &r);

        SDL_Texture *t =
            renderText(renderer, font, sounds[i].name.c_str(), color_black);

        if (t)
        {
            int w, h;
            SDL_QueryTexture(t, nullptr, nullptr, &w, &h);
            SDL_Rect tr{r.x + 6, r.y + (r.h - h) / 2, w, h};
            SDL_RenderCopy(renderer, t, nullptr, &tr);
            SDL_DestroyTexture(t);
        }

        if (hover &&
            (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)))
            selectedSound = (int)i;

        y += 34;
    }

    /* ---------- Upload Button ---------- */
    SDL_Rect uploadBtn{
        list.x + 10,
        list.y + list.h - BUTTON_HEIGHT - 10,
        list.w - 20,
        BUTTON_HEIGHT};

    bool uploadHover = isPointInRect(mouseX, mouseY, uploadBtn);
    drawButton(renderer, font, uploadBtn, "Upload Sound", uploadHover);

    if (uploadHover &&
        (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)))
    {
        std::string file = openWavFileDialog();
        if (!file.empty())
            importWav(file);
    }

    /* ---------- Editor ---------- */
    if (selectedSound < 0)
        return;

    SDL_Rect editor{
        list.x + list.w,
        editorArea.y,
        editorArea.w - list.w,
        editorArea.h};

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &editor);
    SDL_RenderDrawRect(renderer, &editor);

    int cx = editor.x + editor.w / 2;
    int cy = editor.y + editor.h / 2;

    SDL_Texture *title =
        renderText(renderer, font, sounds[selectedSound].name.c_str(), color_black);

    if (title)
    {
        int w, h;
        SDL_QueryTexture(title, nullptr, nullptr, &w, &h);
        SDL_Rect tr{cx - w / 2, cy - 120, w, h};
        SDL_RenderCopy(renderer, title, nullptr, &tr);
        SDL_DestroyTexture(title);
    }

    /* Play / Stop */
    SDL_Rect playBtn{cx - 120, cy - 60, 100, 40};
    SDL_Rect stopBtn{cx + 20, cy - 60, 100, 40};

    drawButton(renderer, font, playBtn, "Play",
               isPointInRect(mouseX, mouseY, playBtn));
    drawButton(renderer, font, stopBtn, "Stop",
               isPointInRect(mouseX, mouseY, stopBtn));

    if (isPointInRect(mouseX, mouseY, playBtn) &&
        (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)))
        playSound(sounds[selectedSound]);

    if (isPointInRect(mouseX, mouseY, stopBtn) &&
        (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)))
        stopSound();

    /* Volume Slider */
    SDL_Rect slider{cx - 150, cy + 10, 300, 6};
    SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
    SDL_RenderFillRect(renderer, &slider);

    int knobX =
        slider.x + (sounds[selectedSound].volume * slider.w) / 100;

    SDL_Rect knob{knobX - 6, slider.y - 6, 12, 18};
    SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
    SDL_RenderFillRect(renderer, &knob);

    if (isPointInRect(mouseX, mouseY, slider) &&
        (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)))
    {
        int v = (mouseX - slider.x) * 100 / slider.w;
        if (v < 0)
            v = 0;
        if (v > 100)
            v = 100;
        sounds[selectedSound].volume = v;
    }
}
