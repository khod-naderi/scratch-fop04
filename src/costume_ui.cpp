#include "costume_ui.h"
#include "ui.h"
#include "generaldef.h"
#include "color.h"

#include <filesystem>
#include <vector>
#include <string>

extern "C"
{
#include "tinyfiledialogs.h"
}

/* =========================================================
   External tab state
========================================================= */
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

/* =========================================================
   Scan costumes once
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
        selectedCostume = 0;

    scanned = true;
}

/* =========================================================
   File dialog
========================================================= */
static std::string openImageFileDialog()
{
    const char *filters[] = {"*.png", "*.jpg"};
    const char *path =
        tinyfd_openFileDialog("Select Image",
                              "",
                              2,
                              filters,
                              "Image files",
                              0);
    return path ? path : "";
}

/* =========================================================
   Import image
========================================================= */
static void importCostume(SDL_Renderer *renderer,
                          const std::string &srcPath)
{
    if (srcPath.empty())
        return;

    std::filesystem::path src(srcPath);

    char *base = SDL_GetBasePath();
    if (!base)
        return;

    std::filesystem::path dstDir =
        std::filesystem::path(base) / "../assets/costumes";
    SDL_free(base);

    std::filesystem::create_directories(dstDir);
    std::filesystem::path dst = dstDir / src.filename();

    std::filesystem::copy_file(src, dst,
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
    }
}
/* =========================================================
   Remove costume safely
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
        // ignore filesystem errors
    }

    costumes.erase(costumes.begin() + index);

    if (costumes.empty())
        selectedCostume = -1;
    else if (selectedCostume >= (int)costumes.size())
        selectedCostume = (int)costumes.size() - 1;
}

/* =========================================================
   MAIN UI
========================================================= */
void drawCostumeEditor(SDL_Renderer *renderer,
                       TTF_Font *font,
                       int mouseX,
                       int mouseY)
{
    scanCostumesOnce(renderer);

    /* ---------- TAB BAR (same as Sound UI) ---------- */
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
            SDL_Rect tr{
                t.x + (t.w - w) / 2,
                t.y + (t.h - h) / 2,
                w, h};
            SDL_RenderCopy(renderer, tx, nullptr, &tr);
            SDL_DestroyTexture(tx);
        }

        if (hover &&
            (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)))
        {
            currentTab = tabs[i].id;
            return;
        }
    }

    /* ---------- MAIN AREA ---------- */
    SDL_Rect editorArea{
        0,
        MENUBAR_HEIGHT + 42,
        MAIN_WINDOW_WIDTH - CANVAS_SCREEN_WIDTH,
        MAIN_WINDOW_HEIGHT - MENUBAR_HEIGHT - 42};

    SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
    SDL_RenderFillRect(renderer, &editorArea);

    /* ---------- COSTUME LIST ---------- */
    const int LIST_WIDTH = 180;
    const int BUTTON_HEIGHT = 32;

    SDL_Rect list{
        editorArea.x,
        editorArea.y,
        LIST_WIDTH,
        editorArea.h};

    SDL_SetRenderDrawColor(renderer, 245, 245, 245, 255);
    SDL_RenderFillRect(renderer, &list);
    SDL_RenderDrawRect(renderer, &list);

    int y = list.y + 20;
    for (size_t i = 0; i < costumes.size(); ++i)
    {
        SDL_Rect r{list.x + 10, y, list.w - 20, 28};
        SDL_Rect removeBtn{r.x + r.w - 22, r.y + 6, 16, 16};

        bool hover = isPointInRect(mouseX, mouseY, r);
        bool removeHover = isPointInRect(mouseX, mouseY, removeBtn);

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

        if (hover &&
            (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)))
        {
            selectedCostume = (int)i;
        }

        y += 34;
    }

    /* ---------- UPLOAD BUTTON ---------- */
    SDL_Rect uploadBtn{
        list.x + 10,
        list.y + list.h - BUTTON_HEIGHT - 10,
        list.w - 20,
        BUTTON_HEIGHT};

    bool uploadHover = isPointInRect(mouseX, mouseY, uploadBtn);

    SDL_SetRenderDrawColor(renderer,
                           uploadHover ? 200 : 220,
                           uploadHover ? 200 : 220,
                           uploadHover ? 255 : 220,
                           255);
    SDL_RenderFillRect(renderer, &uploadBtn);
    SDL_RenderDrawRect(renderer, &uploadBtn);

    SDL_Texture *btnTxt =
        renderText(renderer, font, "Upload Costume", color_black);
    if (btnTxt)
    {
        int w, h;
        SDL_QueryTexture(btnTxt, nullptr, nullptr, &w, &h);
        SDL_Rect tr{
            uploadBtn.x + (uploadBtn.w - w) / 2,
            uploadBtn.y + (uploadBtn.h - h) / 2,
            w, h};
        SDL_RenderCopy(renderer, btnTxt, nullptr, &tr);
        SDL_DestroyTexture(btnTxt);
    }

    if (uploadHover &&
        (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)))
    {
        std::string file = openImageFileDialog();
        if (!file.empty())
            importCostume(renderer, file);
    }

    /* ---------- PREVIEW ---------- */
    if (selectedCostume < 0)
        return;

    SDL_Rect preview{
        list.x + list.w,
        editorArea.y,
        editorArea.w - list.w,
        editorArea.h};

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &preview);
    SDL_RenderDrawRect(renderer, &preview);

    CostumeAsset &c = costumes[selectedCostume];

    float scale =
        std::min((float)(preview.w - 40) / c.w,
                 (float)(preview.h - 40) / c.h);

    SDL_Rect dst{
        preview.x + (preview.w - (int)(c.w * scale)) / 2,
        preview.y + (preview.h - (int)(c.h * scale)) / 2,
        (int)(c.w * scale),
        (int)(c.h * scale)};

    SDL_RenderCopy(renderer, c.texture, nullptr, &dst);
}