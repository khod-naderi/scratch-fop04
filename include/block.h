
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "category.h"
#include <vector>
#include "value.h"

/*
This header file is for handling command Blocks
And Ui part of Blocks list
*/

#ifndef BLOCK_HEADER
#define BLOCK_HEADER

#define BLOCK_ITEM_HEIGTH 50
#define BLOCK_MAX_SLOTS 8
#define BLOCK_MAX_BODIES 2

/*
Blocks type
*/
enum BlockType
{
    BLOCK_COMMAND,
    BLOCK_EVENT,
    BLOCK_STATEMENT,
    BLOCK_OPERATOR,
    BLOCK_REPORTER,
};

/*
For handling slots of blocks
*/
struct SlotDefinition
{
    const char *name;
    ValueType dateType;
    Value defaultValue;
};

/*
For handling execution context for each block
witch sprint, witch position, etc.
*/
struct ExecutionContext;

/*
this is type for execution function of each block,
what that block should do .
it is a function pointer
*/
typedef Value (*BlockExecuteFunc)(ExecutionContext &ctx, Value inputs[], int inputCount);

struct Block
{
    int id;            // identifier of block defenition
    const char *label; // label of block defenition
    CategoryId categoryId;
    BlockType type;

    // input slots
    SlotDefinition slots[BLOCK_MAX_SLOTS];
    int slotCount;

    // Output type for operators and reporters
    ValueType outputType;

    // Old method
    // // void (*command)();
    int width, height;
    SDL_Texture *textrue;

    // shape and connecting
    const bool canHaveTopConnection;
    const bool canHaveBottomConnection;
    int bodyCount;

    // execution
    BlockExecuteFunc execute;

    // base rendering dimension
    int baseWidth, baseHeight;

    Block(const char *f_label, const CategoryId f_categoryId, const BlockType f_type, const bool f_canHaveTopConnection, const bool f_canHaveBottomConnection, BlockExecuteFunc command)
        : label(f_label), categoryId(f_categoryId), type(f_type), canHaveTopConnection(f_canHaveTopConnection), canHaveBottomConnection(f_canHaveBottomConnection), execute(command)
    {
    }
    Block(const char *f_label, const CategoryId f_categoryId, const BlockType f_type, const bool f_canHaveTopConnection, const bool f_canHaveBottomConnection)
        : label(f_label), categoryId(f_categoryId), type(f_type), canHaveTopConnection(f_canHaveTopConnection), canHaveBottomConnection(f_canHaveBottomConnection)
    {
    }
};

extern std::vector<Block> blocksLibrary;
#define BLOCKS_COUNT (blocksLibrary.size())

extern int scrollBlockColumnIndex;

extern bool isBLockDraged;
extern int dragedBlockIndex;

int blockColumnInit(SDL_Renderer *renderer, TTF_Font *font);
void drawBlockColumn(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY);
void controlBlockColumnClickDown(const int mouseX, const int mouseY);
void controlBlockColumnClickUp(const int mouseX, const int mouseY);
void controlBlockColumnMouseScroll(const int mouseX, const int mouseY, const Sint32 scrollY);
#endif