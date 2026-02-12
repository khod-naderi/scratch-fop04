/*
این فایل کتابخانه بلاک‌ها خواهد بود
*/

#include "block.h"
#include "value.h"
#include "blocks_library.h"
#include "execution_context.h"

Value execMoveSteps(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    double steps = inputs[0].asNumber();
    ctx.spriteX += steps * SDL_cos(ctx.spriteAngle);
    ctx.spriteY += steps * SDL_sin(ctx.spriteAngle);
    return Value();
}

Value execTurnAngleDegree(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    double angle = inputs[0].asNumber();
    ctx.spriteAngle += angle;
    if (ctx.spriteAngle > 360)
        ctx.spriteAngle -= 360;
    return Value();
}

Block blocksLibrary[] = {
    // [0] move %s steps
    {
        0,               // id
        "move %s steps", // label
        CATEGORY_MOTION, // category id
        BLOCK_COMMAND,   // type
        {
            // slots
            {"steps", VALUE_NUMBER, fromNumber(10)},
        },
        1,             // slotCount
        VALUE_NUMBER,  // output tyep
        true,          // canHaveTopConnection
        true,          // canHaveBottomConnection
        0,             // body count
        execMoveSteps, // execution
        150,           // base width
        40,            // base height
    },
};

int blocksLibraryCount = sizeof(blocksLibrary) / sizeof(blocksLibrary[0]);