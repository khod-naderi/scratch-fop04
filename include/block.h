
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "category.h"

/*
This header file is for handling command Blocks
And Ui part of Blocks list
*/

#ifndef BLOCK_HEADER
#define BLOCK_HEADER

#define BLOCK_ITEM_HEIGTH 50

enum block_t
{
    BLOCK_COMMAND,
    BLOCK_EVENT,
    BLOCK_STATEMENT,
    BLOCK_OPERATOR,
};

struct Block
{
    const char *label;
    const CATEGORY_ID categoryId;
    const block_t type;
    void (*command)();
    int width, height;
    SDL_Texture *textrue;

    Block(const char *f_label, const CATEGORY_ID f_categoryId, const block_t f_type, void (*command)())
        : label(f_label), categoryId(f_categoryId), type(f_type), command(command)
    {
    }
    Block(const char *f_label, const CATEGORY_ID f_categoryId, const block_t f_type)
        : label(f_label), categoryId(f_categoryId), type(f_type)
    {
    }
};

extern int blocks_count;
extern Block blocksLibrary[];

extern int scrollIndex;

int blockColumnInit(SDL_Renderer *renderer, TTF_Font *font);
void drawBlockColumn(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY);
void controlBlockColumnClickDown(const int mouseX, const int mouseY);
void controlBlockColumnMouseScroll(const int mouseX, const int mouseY, const Sint32 scrollY);
#endif