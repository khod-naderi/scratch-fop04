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

Value execGotoMousePointer(ExecutionContext &ctx, Value inputs[], int inputCount) { return Value(); }
Value execGotoRandomPoint(ExecutionContext &ctx, Value inputs[], int inputCount) { return Value(); }
Value execGotoXY(ExecutionContext &ctx, Value inputs[], int inputCount) { return Value(); }
Value execPointToAngle(ExecutionContext &ctx, Value inputs[], int inputCount) { return Value(); }
Value execPointToMousePointer(ExecutionContext &ctx, Value inputs[], int inputCount) { return Value(); }
Value execChangeXby(ExecutionContext &ctx, Value inputs[], int inputCount) { return Value(); }
Value execSetXto(ExecutionContext &ctx, Value inputs[], int inputCount) { return Value(); }
Value execChangeYby(ExecutionContext &ctx, Value inputs[], int inputCount) { return Value(); }
Value execSetYto(ExecutionContext &ctx, Value inputs[], int inputCount) { return Value(); }
Value execLog(ExecutionContext &ctx, Value inputs[], int inputCount) { return Value(); }

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
        5,
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
    // [6] goto mouse pointer
    {
        6,                    // id
        "goto mouse pointer", // label
        CATEGORY_MOTION,      // category id
        BLOCK_COMMAND,        // type
        {

        },
        0,                    // slotCount
        VALUE_NUMBER,         // output tyep
        true,                 // canHaveTopConnection
        true,                 // canHaveBottomConnection
        0,                    // body count
        execGotoMousePointer, // execution
        150,                  // base width
        40,                   // base height
    },
    // [7] goto random point
    {
        7,                   // id
        "goto random point", // label
        CATEGORY_MOTION,     // category id
        BLOCK_COMMAND,       // type
        {

        },
        0,                   // slotCount
        VALUE_NUMBER,        // output tyep
        true,                // canHaveTopConnection
        true,                // canHaveBottomConnection
        0,                   // body count
        execGotoRandomPoint, // execution
        150,                 // base width
        40,                  // base height
    },
    // [8] goto x: %s y: %s
    {
        8,                  // id
        "goto x: %s y: %s", // label
        CATEGORY_MOTION,    // category id
        BLOCK_COMMAND,      // type
        {
            {"x", VALUE_NUMBER, fromNumber(0)},
            {"y", VALUE_NUMBER, fromNumber(0)},
        },
        2,            // slotCount
        VALUE_NUMBER, // output tyep
        true,         // canHaveTopConnection
        true,         // canHaveBottomConnection
        0,            // body count
        execGotoXY,   // execution
        150,          // base width
        40,           // base height
    },
    // [9] point in direction %s
    {
        9,                       // id
        "point in direction %s", // label
        CATEGORY_MOTION,         // category id
        BLOCK_COMMAND,           // type
        {
            {"theta", VALUE_NUMBER, fromNumber(0)},
        },
        1,                // slotCount
        VALUE_NUMBER,     // output tyep
        true,             // canHaveTopConnection
        true,             // canHaveBottomConnection
        0,                // body count
        execPointToAngle, // execution
        150,              // base width
        40,               // base height
    },
    // [10] point toward mouse pointer
    {
        10,                           // id
        "point toward mouse pointer", // label
        CATEGORY_MOTION,              // category id
        BLOCK_COMMAND,                // type
        {

        },
        0,                       // slotCount
        VALUE_NUMBER,            // output tyep
        true,                    // canHaveTopConnection
        true,                    // canHaveBottomConnection
        0,                       // body count
        execPointToMousePointer, // execution
        150,                     // base width
        40,                      // base height
    },
    // [11] change x by %s
    {
        11,               // id
        "change x by %s", // label
        CATEGORY_MOTION,  // category id
        BLOCK_COMMAND,    // type
        {
            {"deltaX", VALUE_NUMBER, fromNumber(0)},
        },
        1,             // slotCount
        VALUE_NUMBER,  // output tyep
        true,          // canHaveTopConnection
        true,          // canHaveBottomConnection
        0,             // body count
        execChangeXby, // execution
        150,           // base width
        40,            // base height
    },
    // [12] change y by %s
    {
        12,               // id
        "change y by %s", // label
        CATEGORY_MOTION,  // category id
        BLOCK_COMMAND,    // type
        {
            {"deltaY", VALUE_NUMBER, fromNumber(0)},
        },
        1,             // slotCount
        VALUE_NUMBER,  // output tyep
        true,          // canHaveTopConnection
        true,          // canHaveBottomConnection
        0,             // body count
        execChangeYby, // execution
        150,           // base width
        40,            // base height
    },
    // [13] set x to %s
    {
        13,              // id
        "set x to %s",   // label
        CATEGORY_MOTION, // category id
        BLOCK_COMMAND,   // type
        {
            {"setX", VALUE_NUMBER, fromNumber(0)},
        },
        1,            // slotCount
        VALUE_NUMBER, // output tyep
        true,         // canHaveTopConnection
        true,         // canHaveBottomConnection
        0,            // body count
        execSetXto,   // execution
        150,          // base width
        40,           // base height
    },
    // [14] set x to %s
    {
        14,              // id
        "set y to %s",   // label
        CATEGORY_MOTION, // category id
        BLOCK_COMMAND,   // type
        {
            {"setY", VALUE_NUMBER, fromNumber(0)},
        },
        1,            // slotCount
        VALUE_NUMBER, // output tyep
        true,         // canHaveTopConnection
        true,         // canHaveBottomConnection
        0,            // body count
        execSetYto,   // execution
        150,          // base width
        40,           // base height
    },
    // [15] log %s
    {
        15,              // id
        "log %s",        // label
        CATEGORY_MOTION, // category id
        BLOCK_COMMAND,   // type
        {
            {"str", VALUE_STRING, fromString("-")},
        },
        1,            // slotCount
        VALUE_NUMBER, // output tyep
        true,         // canHaveTopConnection
        true,         // canHaveBottomConnection
        0,            // body count
        execLog,      // execution
        150,          // base width
        40,           // base height
    },

};

int blocksLibraryCount = sizeof(blocksLibrary) / sizeof(blocksLibrary[0]);