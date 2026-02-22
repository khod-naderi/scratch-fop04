#include "costume_ui.h"
#include "ui.h"
#include "generaldef.h"
#include "color.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>

#include "tinyfiledialogs.h"

extern EditorTab currentTab;

/* =========================================================
   Costume asset
========================================================= */
struct CostumeAsset
{
    std::string name;
    std::string path;
    SDL_Texture *texture = nullptr;
    int w = 0;
    int h = 0;
};

/* =========================================================
   State
========================================================= */
static std::vector<CostumeAsset> costumes;
static int selectedCostume = -1;
static bool scanned = false;

/* transform (preview only) */
static SDL_FPoint costumePos = {0.f, 0.f};
static float costumeScale = 1.f;

static bool dragging = false;
static SDL_Point dragStartMouse;
static SDL_FPoint dragStartPos;

/* =========================================================
   Scan assets
========================================================= */
static void scanCostumesOnce(SDL_Renderer *renderer)
{
    if (scanned)
        return;

    char *base = SDL_GetBasePath();
    if (!base)
        return;

    std::filesystem::path dir =
        std::filesystem::path(base) / "../assets/costumes";
    SDL_free(base);

    if (std::filesystem::exists(dir))
    {
        for (auto &e : std::filesystem::directory_iterator(dir))
        {
            if (!e.is_regular_file())
                continue;

            auto ext = e.path().extension().string();
            if (ext == ".png" || ext == ".jpg")
            {
                CostumeAsset c;
                c.name = e.path().filename().string();
                c.path = e.path().string();
                c.texture = renderImage(renderer, c.path.c_str());
                if (c.texture)
                {
                    SDL_QueryTexture(c.texture, nullptr, nullptr, &c.w, &c.h);
                    costumes.push_back(c);
                }
            }
        }
    }

    if (!costumes.empty())
    {
        selectedCostume = 0;
        costumePos = {0.f, 0.f};
        costumeScale = 1.f;
    }

    scanned = true;
}

/* =========================================================
   File dialog
========================================================= */
static std::string openImageFileDialog()
{
    const char *filters[] = {"*.png", "*.jpg"};
    const char *path =
        tinyfd_openFileDialog("Select Costume", "", 2, filters, "Images", 0);
    return path ? path : "";
}

/* =========================================================
   Import
========================================================= */
static void importCostume(SDL_Renderer *renderer, const std::string &srcPath)
{
    if (srcPath.empty())
        return;

    char *base = SDL_GetBasePath();
    if (!base)
        return;

    std::filesystem::path dstDir =
        std::filesystem::path(base) / "../assets/costumes";
    SDL_free(base);

    std::filesystem::create_directories(dstDir);

    std::filesystem::path src(srcPath);
    std::filesystem::path dst = dstDir / src.filename();
    std::filesystem::copy_file(
        src, dst,
        std::filesystem::copy_options::overwrite_existing);

    CostumeAsset c;
    c.name = dst.filename().string();
    c.path = dst.string();
    c.texture = renderImage(renderer, c.path.c_str());

    if (c.texture)
    {
        SDL_QueryTexture(c.texture, nullptr, nullptr, &c.w, &c.h);
        costumes.push_back(c);
        selectedCostume = (int)costumes.size() - 1;
        costumePos = {0.f, 0.f};
        costumeScale = 1.f;
    }
}

/* =========================================================
   Remove
========================================================= */
static void removeCostume(size_t index)
{
    if (index >= costumes.size())
        return;

    if (costumes[index].texture)
        SDL_DestroyTexture(costumes[index].texture);

    try
    {
        std::filesystem::remove(costumes[index].path);
    }
    catch (...)
    {
    }

    costumes.erase(costumes.begin() + index);

    if (costumes.empty())
        selectedCostume = -1;
    else if (selectedCostume >= (int)costumes.size())
        selectedCostume = (int)costumes.size() - 1;
}

/* =========================================================
   Button helper
========================================================= */
static void drawButton(SDL_Renderer *r, TTF_Font *font,
                       const SDL_Rect &rect,
                       const char *text, bool hover)
{
    SDL_SetRenderDrawColor(r,
                           hover ? 200 : 220,
                           hover ? 200 : 220,
                           hover ? 255 : 220, 255);
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
   Main UI
========================================================= */
void drawCostumeEditor(SDL_Renderer *renderer,
                       TTF_Font *font,
                       int mouseX, int mouseY)
{
    scanCostumesOnce(renderer);

    SDL_Rect editorArea{
        0, MENUBAR_HEIGHT + 42,
        MAIN_WINDOW_WIDTH - CANVAS_SCREEN_WIDTH,
        MAIN_WINDOW_HEIGHT - MENUBAR_HEIGHT - 42};

    SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
    SDL_RenderFillRect(renderer, &editorArea);

    const int LIST_WIDTH = 180;

    SDL_Rect list{
        editorArea.x, editorArea.y,
        LIST_WIDTH, editorArea.h};

    SDL_SetRenderDrawColor(renderer, 245, 245, 245, 255);
    SDL_RenderFillRect(renderer, &list);
    SDL_RenderDrawRect(renderer, &list);

    int y = list.y + 20;

    /* Costume list */
    for (size_t i = 0; i < costumes.size(); ++i)
    {
        SDL_Rect r{list.x + 10, y, list.w - 20, 28};
        SDL_Rect xBtn{r.x + r.w - 22, r.y + 6, 16, 16};

        bool hover = isPointInRect(mouseX, mouseY, r);
        bool xHover = isPointInRect(mouseX, mouseY, xBtn);

        SDL_SetRenderDrawColor(renderer,
                               (int)i == selectedCostume ? 190 : hover ? 210
                                                                       : 235,
                               210, 255, 255);
        SDL_RenderFillRect(renderer, &r);

        SDL_Texture *t =
            renderText(renderer, font, costumes[i].name.c_str(), color_black);
        if (t)
        {
            int w, h;
            SDL_QueryTexture(t, nullptr, nullptr, &w, &h);
            SDL_Rect tr{r.x + 6, r.y + (r.h - h) / 2, w, h};
            SDL_RenderCopy(renderer, t, nullptr, &tr);
            SDL_DestroyTexture(t);
        }

        SDL_SetRenderDrawColor(renderer, xHover ? 220 : 200, 80, 80, 255);
        SDL_RenderFillRect(renderer, &xBtn);
        SDL_RenderDrawRect(renderer, &xBtn);

        SDL_Texture *xt = renderText(renderer, font, "X", color_black);
        if (xt)
        {
            int w, h;
            SDL_QueryTexture(xt, nullptr, nullptr, &w, &h);
            SDL_Rect tr{xBtn.x + (xBtn.w - w) / 2,
                        xBtn.y + (xBtn.h - h) / 2, w, h};
            SDL_RenderCopy(renderer, xt, nullptr, &tr);
            SDL_DestroyTexture(xt);
        }

        Uint32 mouse = SDL_GetMouseState(nullptr, nullptr);
        if (xHover && (mouse & SDL_BUTTON(SDL_BUTTON_LEFT)))
        {
            removeCostume(i);
            break;
        }
        if (hover && (mouse & SDL_BUTTON(SDL_BUTTON_LEFT)))
        {
            selectedCostume = (int)i;
            costumePos = {0.f, 0.f};
            costumeScale = 1.f;
        }

        y += 34;
    }

    /* Upload button */
    SDL_Rect uploadBtn{
        list.x + 10,
        list.y + list.h - 42,
        list.w - 20, 32};

    bool uploadHover = isPointInRect(mouseX, mouseY, uploadBtn);
    drawButton(renderer, font, uploadBtn, "Upload Costume", uploadHover);

    if (uploadHover &&
        (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)))
    {
        importCostume(renderer, openImageFileDialog());
    }

    /* Preview */
    if (selectedCostume < 0)
        return;

    CostumeAsset &c = costumes[selectedCostume];

    SDL_Rect preview{
        list.x + list.w, editorArea.y,
        editorArea.w - list.w, editorArea.h};

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &preview);
    SDL_RenderDrawRect(renderer, &preview);

    float baseScale = std::min(
        (float)(preview.w - 80) / c.w,
        (float)(preview.h - 80) / c.h);

    float scale = baseScale * costumeScale;

    SDL_Rect dst{
        (int)(preview.x + preview.w / 2 - c.w * scale / 2 + costumePos.x),
        (int)(preview.y + preview.h / 2 - c.h * scale / 2 + costumePos.y),
        (int)(c.w * scale),
        (int)(c.h * scale)};

    SDL_RenderCopy(renderer, c.texture, nullptr, &dst);

    /* Drag logic */
    Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
    bool leftDown = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);

    if (!dragging && isPointInRect(mouseX, mouseY, dst) && leftDown)
    {
        dragging = true;
        dragStartMouse = {mouseX, mouseY};
        dragStartPos = costumePos;
    }
    if (dragging && leftDown)
    {
        costumePos.x = dragStartPos.x + (mouseX - dragStartMouse.x);
        costumePos.y = dragStartPos.y + (mouseY - dragStartMouse.y);
    }
    if (!leftDown)
    {
        dragging = false;
    }

    /* Scroll wheel zoom */
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_MOUSEWHEEL)
        {
            if (e.wheel.y > 0)
            { // scroll up
                costumeScale += 0.1f;
            }
            else if (e.wheel.y < 0)
            { // scroll down
                costumeScale -= 0.1f;
                if (costumeScale < 0.1f)
                    costumeScale = 0.1f;
            }
        }
    }
}
