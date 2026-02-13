/*
This header file is for handling UI component
*/

#include <SDL2/SDL.h>
#include "menubar.h"
#include "generaldef.h"
#include "block.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "category.h"
#include <string>

#ifndef UI_HEADER
#define UI_HEADER

#define CATEGORY_COLUMN_WIDTH 80
#define BLOCKS_COLUMN_WIDTH 200
#define CANVAS_SCREEN_WIDTH 500
#define CANVAS_SCREEN_HEIGTH 500

#define SCROLL_Y_SENSIVITY 5

extern const SDL_Rect SCREEN;
extern const SDL_Rect MENUBAR_ROW;
extern const SDL_Rect CATEGORY_COLUMN;
extern const SDL_Rect BLOCKS_COLUMN;
extern const SDL_Rect WORKSPACE_COLUMN;
extern const SDL_Rect CANVAS_BOX;
extern const SDL_Rect SPRITE_BOX;
extern const SDL_Rect SAVELOAD_BOX;

SDL_Texture *renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, const SDL_Color &color);
SDL_Texture *renderImage(SDL_Renderer *renderer, const char *imgPath);
bool isPointInRect(int x, int y, const SDL_Rect &rect);
char keysym2char(const SDL_Keysym &key);

void render_categories(SDL_Renderer *renderer, Category categories[], int count, int selected);
void render_blocks(SDL_Renderer *renderer, Block blocks[], int count, float scroll_offset, SDL_Rect viewport);
void render_extension_button(SDL_Renderer *renderer, SDL_Rect bounds);

// Define menu reactangles
extern const SDL_Rect fileMenuRect;
extern const SDL_Rect editMenuRect;

// default font (DO NOT USE IT)
extern TTF_Font *DEFAULT_FONT;

/*
----------------------------------------
    Ui elements
----------------------------------------
*/
void controlUiElementClickDown(int mouseX, int mouseY);
void controlUiElementKeyboardHit(const SDL_Keysym &key);

// textbox
#define TEXTBOX_RL_MARGIN 4
#define TEXTBOX_TB_MARGIN 4

struct TextBox
{
    // Identifier
    int id;

    // Position
    int posX;
    int posY;

    // Storage memory - DO NOT CHANGE IT DIRECTLY
    std::string _str;

    // Controll variables
    bool isFucused;

    // Visibility
    bool isVisible;

    // Cached dimension
    int cachedWidth;
    int cachedHeight;

    // Font and color
    TTF_Font *font;
    SDL_Color fgColor;
    SDL_Color bgColor;

    // Constructor
    TextBox(TTF_Font *font_i);
    TextBox(TTF_Font *font_i, std::string &defaultStr);

    // Functions
    void setStr(const std::string &istr);
    std::string getStr() const;

    // Drawing
    void draw(SDL_Renderer *renderer);
};

void controlTextboxesClickDown(int mouseX, int mouseY);
void controlTextboxesKeyboardHit(const SDL_Keysym &key);

#endif