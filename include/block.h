
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

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
    BLOCK_STATEMENT,
    BLOCK_OPERATOR,
};

struct Block
{
    const char *label;
    const unsigned int categoryId;
    const block_t type;
    int (*command)();
};

extern const Block blocksLibrary[];

int blockColumnInit(SDL_Renderer *renderer, TTF_Font *font);
void drawBlockColumn(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY);
void controlBlockColumnClickDown(const int mouseX, const int mouseY);
#endif