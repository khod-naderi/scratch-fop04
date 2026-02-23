#include "sound_ui.h"
#include "ui.h"
#include "generaldef.h"
#include "color.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_ttf.h>
#include "tinyfiledialogs.h"
#include <algorithm>

#include <filesystem>
#include <vector>
#include <string>

/* =========================================================
   External tab state (use your enum)
========================================================= */
extern EditorTab currentTab; // from ui.cpp

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
   Scan once
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
   Load WAVs
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
   Remove sound safely
========================================================= */
static void removeSound(size_t index)
{
    if (index >= sounds.size())
        return;

    if (sounds[index].buffer)
        SDL_FreeWAV(sounds[index].buffer);
    sounds.erase(sounds.begin() + index);

    if (sounds.empty())
        selectedSound = -1;
    else if (selectedSound >= (int)sounds.size())
        selectedSound = (int)sounds.size() - 1;
}

/* =========================================================
   Audio playback
========================================================= */
static void playSound(const SoundAsset &s)
{
    if (!s.buffer)
        return;

    if (!audioDevice)
        audioDevice =
            SDL_OpenAudioDevice(nullptr, 0, &s.spec, &deviceSpec, 0);

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
   File dialog
========================================================= */
static std::string openWavFileDialog()
{
    const char *filters[] = {"*.wav"};
    const char *path =
        tinyfd_openFileDialog("Select WAV file", "", 1, filters, "WAV files", 0);
    return path ? path : "";
}

/* =========================================================
   Import WAV
========================================================= */
static void importWav(const std::string &srcPath)
{
    if (srcPath.empty())
        return;

    std::filesystem::path src(srcPath);

    char *base = SDL_GetBasePath();
    if (!base)
        return;

    std::filesystem::path dstDir =
        std::filesystem::path(base) / "../assets/sounds";
    SDL_free(base);

    std::filesystem::create_directories(dstDir);
    std::filesystem::path dst = dstDir / src.filename();

    std::filesystem::copy_file(src, dst,
                               std::filesystem::copy_options::overwrite_existing);

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
static void drawButton(SDL_Renderer *r, TTF_Font *font,
                       const SDL_Rect &rect, const char *text, bool hover)
{
    SDL_SetRenderDrawColor(r,
                           hover ? 200 : 220,
                           hover ? 200 : 220,
                           hover ? 255 : 220,
                           255);
    SDL_RenderFillRect(r, &rect);
    SDL_RenderDrawRect(r, &rect);

    SDL_Texture *t = renderText(r, font, text, color_black);
    if (t)
    {
        int w, h;
        SDL_QueryTexture(t, nullptr, nullptr, &w, &h);
        SDL_Rect tr{rect.x + (rect.w - w) / 2,
                    rect.y + (rect.h - h) / 2, w, h};
        SDL_RenderCopy(r, t, nullptr, &tr);
        SDL_DestroyTexture(t);
    }
}

/* =========================================================
   MAIN UI
========================================================= */
void drawSoundEditor(SDL_Renderer *renderer, TTF_Font *font,
                     int mouseX, int mouseY)
{
    scanSoundsOnce();
    loadWavsOnce();

    /* ---------- TAB BAR ---------- */
    SDL_Rect tabBar{0, MENUBAR_HEIGHT, MAIN_WINDOW_WIDTH, 42};
    SDL_SetRenderDrawColor(renderer, 230, 230, 230, 255);
    SDL_RenderFillRect(renderer, &tabBar);

    struct TabDef
    {
        EditorTab id;
        const char *label;
    } tabs[3] = {
        {TAB_CODE, "Code"},
        {TAB_COSTUME, "Costumes"},
        {TAB_SOUND, "Sounds"}};

    for (int i = 0; i < 3; ++i)
    {
        SDL_Rect t{20 + i * 120, tabBar.y + 6, 110, 28};
        bool hover = isPointInRect(mouseX, mouseY, t);

        SDL_SetRenderDrawColor(renderer,
                               currentTab == tabs[i].id ? 255 : hover ? 210
                                                                      : 200,
                               currentTab == tabs[i].id ? 255 : hover ? 210
                                                                      : 200,
                               currentTab == tabs[i].id ? 255 : hover ? 210
                                                                      : 200,
                               255);
        SDL_RenderFillRect(renderer, &t);
        SDL_RenderDrawRect(renderer, &t);

        SDL_Texture *tx = renderText(renderer, font, tabs[i].label, color_black);
        if (tx)
        {
            int w, h;
            SDL_QueryTexture(tx, nullptr, nullptr, &w, &h);
            SDL_Rect tr{t.x + (t.w - w) / 2, t.y + (t.h - h) / 2, w, h};
            SDL_RenderCopy(renderer, tx, nullptr, &tr);
            SDL_DestroyTexture(tx);
        }

        if (hover && (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)))
        {
            currentTab = tabs[i].id; // enum safe assignment
            return;                  // exit immediately after tab switch
        }
    }

    /* ---------- MAIN AREA ---------- */
    SDL_Rect editorArea{0,
                        MENUBAR_HEIGHT + 42,
                        MAIN_WINDOW_WIDTH - CANVAS_SCREEN_WIDTH,
                        MAIN_WINDOW_HEIGHT - MENUBAR_HEIGHT - 42};
    SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
    SDL_RenderFillRect(renderer, &editorArea);

    const int SOUND_LIST_WIDTH = 180;
    const int BUTTON_HEIGHT = 32;
    SDL_Rect list{editorArea.x, editorArea.y, SOUND_LIST_WIDTH, editorArea.h};

    SDL_SetRenderDrawColor(renderer, 245, 245, 245, 255);
    SDL_RenderFillRect(renderer, &list);
    SDL_RenderDrawRect(renderer, &list);

    int y = list.y + 20;
    for (size_t i = 0; i < sounds.size(); ++i)
    {
        SDL_Rect r{list.x + 10, y, list.w - 20, 28};
        SDL_Rect removeBtn{r.x + r.w - 22, r.y + 6, 16, 16};
        bool hover = isPointInRect(mouseX, mouseY, r);
        bool removeHover = isPointInRect(mouseX, mouseY, removeBtn);

        SDL_SetRenderDrawColor(renderer,
                               (int)i == selectedSound ? 190 : hover ? 210
                                                                     : 235,
                               210, 255, 255);
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

        SDL_SetRenderDrawColor(renderer, removeHover ? 220 : 200, 80, 80, 255);
        SDL_RenderFillRect(renderer, &removeBtn);
        SDL_RenderDrawRect(renderer, &removeBtn);

        SDL_Texture *xTex = renderText(renderer, font, "X", color_black);
        if (xTex)
        {
            int w, h;
            SDL_QueryTexture(xTex, nullptr, nullptr, &w, &h);
            SDL_Rect tr{removeBtn.x + (removeBtn.w - w) / 2,
                        removeBtn.y + (removeBtn.h - h) / 2, w, h};
            SDL_RenderCopy(renderer, xTex, nullptr, &tr);
            SDL_DestroyTexture(xTex);
        }

        Uint32 mouse = SDL_GetMouseState(nullptr, nullptr);
        if (removeHover && (mouse & SDL_BUTTON(SDL_BUTTON_LEFT)))
        {
            removeSound(i);
            break;
        }
        else if (hover && (mouse & SDL_BUTTON(SDL_BUTTON_LEFT)))
        {
            selectedSound = (int)i;
        }
        y += 34;
    }

    SDL_Rect uploadBtn{list.x + 10, list.y + list.h - BUTTON_HEIGHT - 10,
                       list.w - 20, BUTTON_HEIGHT};
    bool uploadHover = isPointInRect(mouseX, mouseY, uploadBtn);
    drawButton(renderer, font, uploadBtn, "Upload Sound", uploadHover);
    if (uploadHover && (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)))
    {
        std::string file = openWavFileDialog();
        if (!file.empty())
            importWav(file);
    }

    if (selectedSound < 0)
        return;

    SDL_Rect editor{list.x + list.w, editorArea.y, editorArea.w - list.w, editorArea.h};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &editor);
    SDL_RenderDrawRect(renderer, &editor);

    int cx = editor.x + editor.w / 2;
    int cy = editor.y + editor.h / 2;

    SDL_Texture *title = renderText(renderer, font, sounds[selectedSound].name.c_str(), color_black);
    if (title)
    {
        int w, h;
        SDL_QueryTexture(title, nullptr, nullptr, &w, &h);
        SDL_Rect tr{cx - w / 2, cy - 120, w, h};
        SDL_RenderCopy(renderer, title, nullptr, &tr);
        SDL_DestroyTexture(title);
    }

    SDL_Rect playBtn{cx - 120, cy - 60, 100, 40};
    SDL_Rect stopBtn{cx + 20, cy - 60, 100, 40};
    drawButton(renderer, font, playBtn, "Play", isPointInRect(mouseX, mouseY, playBtn));
    drawButton(renderer, font, stopBtn, "Stop", isPointInRect(mouseX, mouseY, stopBtn));

    if (isPointInRect(mouseX, mouseY, playBtn) &&
        (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)))
        playSound(sounds[selectedSound]);
    if (isPointInRect(mouseX, mouseY, stopBtn) &&
        (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)))
        stopSound();

    SDL_Rect slider{cx - 150, cy + 10, 300, 6};
    SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
    SDL_RenderFillRect(renderer, &slider);

    int knobX = slider.x + (sounds[selectedSound].volume * slider.w) / 100;
    SDL_Rect knob{knobX - 6, slider.y - 6, 12, 18};
    SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
    SDL_RenderFillRect(renderer, &knob);

    if (isPointInRect(mouseX, mouseY, slider) &&
        (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)))
    {
        int v = (mouseX - slider.x) * 100 / slider.w;
        sounds[selectedSound].volume = std::clamp(v, 0, 100);
    }

    std::string volText = "Volume: " + std::to_string(sounds[selectedSound].volume) + "%";
    SDL_Texture *volTex = renderText(renderer, font, volText.c_str(), color_black);
    if (volTex)
    {
        int tw, th;
        SDL_QueryTexture(volTex, nullptr, nullptr, &tw, &th);
        SDL_Rect tr{cx - tw / 2, slider.y + 20, tw, th};
        SDL_RenderCopy(renderer, volTex, nullptr, &tr);
        SDL_DestroyTexture(volTex);
    }
}
