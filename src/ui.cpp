/*
This cpp file is for handling UI component
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "ui.h"
#include "menubar.h"
#include <iostream>
#include "generaldef.h"
#include "color.h"

const SDL_Rect SCREEN = {0, 0, MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT};
const SDL_Rect MENUBAR_ROW = {0, 0, MAIN_WINDOW_WIDTH, MENUBAR_HEIGHT};
const SDL_Rect CATEGORY_COLUMN = {0, MENUBAR_HEIGHT, CATEGORY_COLUMN_WIDTH, MAIN_WINDOW_HEIGHT};
const SDL_Rect BLOCKS_COLUMN = {CATEGORY_COLUMN_WIDTH, MENUBAR_HEIGHT, CATEGORY_COLUMN_WIDTH + BLOCKS_COLUMN_WIDTH, MAIN_WINDOW_HEIGHT};
const SDL_Rect WORKSPACE_COLUMN = {CATEGORY_COLUMN_WIDTH + CATEGORY_COLUMN_WIDTH + BLOCKS_COLUMN_WIDTH, MENUBAR_HEIGHT, MAIN_WINDOW_WIDTH - (CATEGORY_COLUMN_WIDTH + CATEGORY_COLUMN_WIDTH + BLOCKS_COLUMN_WIDTH + CANVAS_SCREEN_WIDTH), MAIN_WINDOW_HEIGHT};
const SDL_Rect CANVAS_BOX = {MAIN_WINDOW_WIDTH - CANVAS_SCREEN_WIDTH, MENUBAR_HEIGHT, CANVAS_SCREEN_WIDTH, CANVAS_SCREEN_HEIGTH};
const SDL_Rect SPRITE_BOX = {MAIN_WINDOW_WIDTH - CANVAS_SCREEN_WIDTH, CANVAS_SCREEN_HEIGTH + MENUBAR_HEIGHT, CANVAS_SCREEN_WIDTH, MAIN_WINDOW_HEIGHT - (CANVAS_SCREEN_HEIGTH + MENUBAR_HEIGHT )};

const SDL_Rect fileMenuRect = {LOGO_MARGIN_LEFT + LOGO_WIDTH + 2 * MENU_MARGIN_LEFT, 5, 60, MENUBAR_HEIGHT - 10};
const SDL_Rect editMenuRect = {fileMenuRect.x + fileMenuRect.w + MENU_MARGIN_LEFT, 5, 60, MENUBAR_HEIGHT - 10};

const SDL_Rect SAVELOAD_BOX = {MAIN_WINDOW_WIDTH / 4, MAIN_WINDOW_HEIGHT / 3, MAIN_WINDOW_WIDTH / 2, MAIN_WINDOW_HEIGHT / 3};
const SDL_Rect LOGGER_Box = {
    CATEGORY_COLUMN_WIDTH + CATEGORY_COLUMN_WIDTH + BLOCKS_COLUMN_WIDTH,
    MAIN_WINDOW_HEIGHT - LOGGER_HEIGHT,
    MAIN_WINDOW_WIDTH - (CATEGORY_COLUMN_WIDTH + CATEGORY_COLUMN_WIDTH + BLOCKS_COLUMN_WIDTH + CANVAS_SCREEN_WIDTH),
    LOGGER_HEIGHT};

TTF_Font *DEFAULT_FONT;

/*
    This function render text
*/
SDL_Texture *
renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, const SDL_Color &color)
{
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);
    if (!surface)
        return nullptr;
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

/*
    This function will check that a point is inside a rectangle or not
*/
bool isPointInRect(int x, int y, const SDL_Rect &rect)
{
    return (x >= rect.x) && (x <= (rect.x + rect.w)) && (y >= rect.y) && (y <= (rect.y + rect.h));
}

/*
-----------------------------------
Load an image from a file path
-----------------------------------
this will return texture of that photo.
*/
SDL_Texture *renderImage(SDL_Renderer *renderer, const char *imgPath)
{
    SDL_Surface *custome = IMG_Load(imgPath);
    if (!custome)
        return nullptr;

    SDL_Texture *imgText = SDL_CreateTextureFromSurface(renderer, custome);
    SDL_FreeSurface(custome);

    return imgText;
}

/*
    This function will return proper character of keySym (if non-printable, return '\0')
    also handling meta 'SHIFT' key holding
*/
char keysym2char(const SDL_Keysym &keysym)
{
    SDL_Keycode key = keysym.sym;
    Uint16 mod = keysym.mod;
    bool shift_pressed = (mod & KMOD_SHIFT) != 0;

    // Handle letters (a-z)
    if (key >= SDLK_a && key <= SDLK_z)
    {
        char base_char = 'a' + (key - SDLK_a);
        return shift_pressed ? toupper(base_char) : base_char;
    }

    // Handle numbers and their shift symbols
    if (key >= SDLK_0 && key <= SDLK_9)
    {
        if (shift_pressed)
        {
            // Shift + number keys produce symbols
            switch (key)
            {
            case SDLK_0:
                return ')';
            case SDLK_1:
                return '!';
            case SDLK_2:
                return '@';
            case SDLK_3:
                return '#';
            case SDLK_4:
                return '$';
            case SDLK_5:
                return '%';
            case SDLK_6:
                return '^';
            case SDLK_7:
                return '&';
            case SDLK_8:
                return '*';
            case SDLK_9:
                return '(';
            default:
                return '\0';
            }
        }
        else
        {
            return '0' + (key - SDLK_0);
        }
    }

    // Handle special symbols
    switch (key)
    {
    case SDLK_SPACE:
        return ' ';
    case SDLK_MINUS:
        return shift_pressed ? '_' : '-';
    case SDLK_EQUALS:
        return shift_pressed ? '+' : '=';
    case SDLK_LEFTBRACKET:
        return shift_pressed ? '{' : '[';
    case SDLK_RIGHTBRACKET:
        return shift_pressed ? '}' : ']';
    case SDLK_BACKSLASH:
        return shift_pressed ? '|' : '\\';
    case SDLK_SEMICOLON:
        return shift_pressed ? ':' : ';';
    case SDLK_QUOTE:
        return shift_pressed ? '"' : '\'';
    case SDLK_COMMA:
        return shift_pressed ? '<' : ',';
    case SDLK_PERIOD:
        return shift_pressed ? '>' : '.';
    case SDLK_SLASH:
        return shift_pressed ? '?' : '/';
    case SDLK_BACKQUOTE:
        return shift_pressed ? '~' : '`';

    // Function keys, arrows, etc. don't have character representations
    default:
        return '\0';
    }
}

/*
--------------------------------------------
    UI elements
--------------------------------------------
*/

void controlUiElementClickDown(int mouseX, int mouseY)
{
    controlTextboxesClickDown(mouseX, mouseY);
}

void controlUiElementKeyboardHit(const SDL_Keysym &key)
{
    controlTextboxesKeyboardHit(key);
}

/*
-----------------------
    TextBox
-----------------------
*/
int textboxLastId = 0;
std::vector<TextBox *> textboxList;

TextBox::TextBox(TTF_Font *font_i)
{
    id = textboxLastId++;
    isFucused = false;
    posX = 0;
    posY = 0;
    isVisible = true;
    font = font_i;
    fgColor = color_black;
    bgColor = color_white;
    setStr(" ");

    textboxList.push_back(this);
}

TextBox::TextBox(TTF_Font *font_i, std::string &defaultStr)
{
    id = textboxLastId++;
    isFucused = false;
    posX = 0;
    posY = 0;
    isVisible = true;
    font = font_i;
    fgColor = color_black;
    bgColor = color_white;
    setStr(defaultStr);

    textboxList.push_back(this);
}

std::string TextBox::getStr() const
{
    return _str;
}

void TextBox::setStr(const std::string &istr)
{
    _str = istr;

    // update dimentions
    TTF_SizeText(font, _str.c_str(), &cachedWidth, &cachedHeight);
}

void TextBox::draw(SDL_Renderer *renderer)
{
    if (!isVisible)
        return;

    SDL_Rect bgRect = {
        posX,
        posY,
        cachedWidth + TEXTBOX_RL_MARGIN,  // +[TEXTBOX_RL_MARGIN]px for left-right margin
        cachedHeight + TEXTBOX_TB_MARGIN, // +[TEXTBOX_RL_MARGIN]px for top-bottom margin
    };

    // Draw background
    if (isFucused)
        SDL_SetRenderDrawColor(renderer, colorDim(bgColor));
    else
        SDL_SetRenderDrawColor(renderer, bgColor);

    SDL_RenderFillRect(renderer, &bgRect);

    // Draw text string
    SDL_Rect fgRect = {
        posX + TEXTBOX_RL_MARGIN / 2,
        posY + TEXTBOX_TB_MARGIN / 2,
        cachedWidth,
        cachedHeight,
    };

    SDL_Texture *textureTxt = renderText(renderer, font, getStr().c_str(), fgColor);
    if (textureTxt == nullptr)
        return;
    SDL_RenderCopy(renderer, textureTxt, NULL, &fgRect);
}

void controlTextboxesClickDown(int mouseX, int mouseY)
{
    for (TextBox *txb : textboxList)
    {
        SDL_Rect rect = {
            txb->posX,
            txb->posY,
            txb->cachedWidth + TEXTBOX_RL_MARGIN,  // +[TEXTBOX_RL_MARGIN]px for left-right margin
            txb->cachedHeight + TEXTBOX_TB_MARGIN, // +[TEXTBOX_RL_MARGIN]px for top-bottom margin
        };

        if (txb->isVisible && isPointInRect(mouseX, mouseY, rect))
            txb->isFucused = true;
        else
            txb->isFucused = false;
    }
}

void controlTextboxesKeyboardHit(const SDL_Keysym &key)
{
    for (TextBox *txb : textboxList)
    {
        if (txb->isFucused)
        {
            if (key.sym == SDLK_ESCAPE)
            {
                txb->isFucused = false;
                return;
            }

            std::string newStr = txb->getStr();

            if (key.sym == SDLK_BACKSPACE)
            {
                if (!newStr.empty())
                    newStr.pop_back();
            }
            else
            {
                char keychar = keysym2char(key);
                newStr.push_back(keychar);
            }
            txb->setStr(newStr);
        }
    }
}
/*
this function draws a thick rectangle instead of the default sdl2 rectangle.
*/
void draw_thick_rect(SDL_Renderer *renderer, const SDL_Rect &rect, int thickness)
{
    SDL_Rect temp = rect;
    for (int i = 0; i < thickness; ++i)
    {
        SDL_RenderDrawRect(renderer, &temp);
        temp.x++;
        temp.y++;
        temp.w -= 2;
        temp.h -= 2;
    }
}
