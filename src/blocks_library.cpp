/*
این فایل کتابخانه بلاک‌ها خواهد بود
*/

#include "block.h"
#include "value.h"
#include "blocks_library.h"
#include "execution_context.h"
#include "logger.h"
#include "ui.h"
#include "engine.h"

Value execMoveSteps(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    double steps = inputs[0].asNumber();
    ctx.sprite->posX += steps * SDL_cos(ctx.sprite->angleRotation * (M_PI / 180));
    ctx.sprite->posY += steps * SDL_sin(ctx.sprite->angleRotation * (M_PI / 180));
    return Value();
}

Value execTurnAngleDegree(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    double angle = inputs[0].asNumber();
    ctx.sprite->angleRotation += angle;
    if (ctx.sprite->angleRotation > 360)
        ctx.sprite->angleRotation -= 360;
    return Value();
}

Value execGreenflag(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    // event triggerer
    int mouseX = executionContextGetVariable(ctx, "_mouseX")->asNumber();
    int mouseY = executionContextGetVariable(ctx, "_mouseY")->asNumber();
    EventType eventType = (EventType)executionContextGetVariable(ctx, "_eventType")->asNumber();

    if (eventType == MOUSE_BTN_LEFT_CLICK && isPointInRect(mouseX, mouseY, GREENFLAG_BTN))
    {
        logger_log("Green flag clicked");
        return fromBool(true);
    }

    return fromBool(false);
}

Value execRepeat(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    // I do not know how to handle this !!
    return inputs[0];
}

Value execIfElse(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    return inputs[0];
}

Value execAdd(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    return fromNumber(inputs[0].asNumber() + inputs[1].asNumber());
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
    // [1] when green flag clicked
    {
        1,
        "when green flag clicked",
        CATEGORY_EVENT,
        BLOCK_EVENT,
        {},
        0,
        VALUE_NUMBER,
        false,
        true,
        0,
        execGreenflag,
        200,
        40,
    },
    // [2] move %s steps
    {
        2,                // id
        "turn %s degree", // label
        CATEGORY_MOTION,  // category id
        BLOCK_COMMAND,    // type
        {
            // slots
            {"angle", VALUE_NUMBER, fromNumber(15)},
        },
        1,                   // slotCount
        VALUE_NUMBER,        // output tyep
        true,                // canHaveTopConnection
        true,                // canHaveBottomConnection
        0,                   // body count
        execTurnAngleDegree, // execution
        150,                 // base width
        40,                  // base height
    },
    // [3] Repeat %s
    {
        3,
        "repeat %s",
        CATEGORY_CONTROL,
        BLOCK_STATEMENT,
        {
            {"count", VALUE_NUMBER, fromNumber(10)},
        },
        1,
        VALUE_NUMBER,
        true,
        true,
        1,
        execRepeat,
        150,
        40,
    },
    // [4] %s + %s
    {
        4,
        "%s + %s",
        CATEGORY_OPERATOR,
        BLOCK_OPERATOR,
        {
            {"a", VALUE_NUMBER, fromNumber(0)},
            {"b", VALUE_NUMBER, fromNumber(0)},
        },
        2,
        VALUE_NUMBER,
        false,
        false,
        0,
        execAdd,
        100,
        30,
    },
    // [5] if-else statement
    {
        3,
        "if-else %s",
        CATEGORY_CONTROL,
        BLOCK_STATEMENT,
        {
            {"condition", VALUE_BOOLEAN, fromBool(true)},
        },
        1,
        VALUE_NUMBER,
        true,
        true,
        2,
        execIfElse,
        150,
        40,
    },
};

int blocksLibraryCount = sizeof(blocksLibrary) / sizeof(blocksLibrary[0]);