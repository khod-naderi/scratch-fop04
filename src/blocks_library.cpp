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
// #include <iostream>

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
    Value *mouseXVar = executionContextGetVariable(ctx, "_mouseX");
    Value *mouseYVar = executionContextGetVariable(ctx, "_mouseY");
    Value *eventTypeVar = executionContextGetVariable(ctx, "_eventType");

    if (!mouseXVar || !mouseYVar || !eventTypeVar)
        return fromBool(false);

    int mouseX = mouseXVar->asNumber();
    int mouseY = mouseYVar->asNumber();
    EventType eventType = (EventType)eventTypeVar->asNumber();

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

// Motion blocks implementation
Value execGotoMousePointer(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    Value *mouseXVar = executionContextGetVariable(ctx, "_mouseX");
    Value *mouseYVar = executionContextGetVariable(ctx, "_mouseY");

    if (mouseXVar && mouseYVar)
    {
        ctx.sprite->posX = mouseXVar->asNumber();
        ctx.sprite->posY = mouseYVar->asNumber();
    }
    return Value();
}

Value execGotoRandomPoint(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    ctx.sprite->posX = rand() % 800 - 400; // Random x between -400 and 400
    ctx.sprite->posY = rand() % 600 - 300; // Random y between -300 and 300
    return Value();
}

Value execGotoXY(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    ctx.sprite->posX = inputs[0].asNumber();
    ctx.sprite->posY = inputs[1].asNumber();
    return Value();
}

Value execPointToAngle(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    ctx.sprite->angleRotation = inputs[0].asNumber();
    return Value();
}

Value execPointToMousePointer(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    Value *mouseXVar = executionContextGetVariable(ctx, "_mouseX");
    Value *mouseYVar = executionContextGetVariable(ctx, "_mouseY");

    if (mouseXVar && mouseYVar)
    {
        int mouseX = mouseXVar->asNumber();
        int mouseY = mouseYVar->asNumber();
        double dx = mouseX - ctx.sprite->posX;
        double dy = mouseY - ctx.sprite->posY;
        ctx.sprite->angleRotation = SDL_atan2(dy, dx) * (180.0 / M_PI);
    }
    return Value();
}

Value execChangeXby(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    ctx.sprite->posX += inputs[0].asNumber();
    return Value();
}

Value execSetXto(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    ctx.sprite->posX = inputs[0].asNumber();
    return Value();
}

Value execChangeYby(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    ctx.sprite->posY += inputs[0].asNumber();
    return Value();
}

Value execSetYto(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    ctx.sprite->posY = inputs[0].asNumber();
    return Value();
}

Value execLog(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    logger_log(inputs[0].asString().c_str());
    return Value();
}

// Looks blocks implementation
Value execShow(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    // TODO: Add isVisible field to SprintBody
    // ctx.sprite->isVisible = true;
    logger_log("Show sprite");
    return Value();
}

Value execHide(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    // TODO: Add isVisible field to SprintBody
    // ctx.sprite->isVisible = false;
    logger_log("Hide sprite");
    return Value();
}

Value execSay(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    // TODO: Implement speech bubble display
    logger_log(("Say: " + inputs[0].asString()).c_str());
    return Value();
}

Value execThink(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    // TODO: Implement thought bubble display
    logger_log(("Think: " + inputs[0].asString()).c_str());
    return Value();
}

Value execChangeSizeBy(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    // TODO: Add scale field to SprintBody
    ctx.sprite->size += inputs[0].asNumber() / 100.0;
    logger_log("Change size");
    return Value();
}

Value execSetSizeTo(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    // TODO: Add scale field to SprintBody
    ctx.sprite->size = inputs[0].asNumber() / 100.0;
    logger_log("Set size");
    return Value();
}

// Event blocks implementation
Value execKeyPressed(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    Value *eventTypeVar = executionContextGetVariable(ctx, "_eventType");
    if (!eventTypeVar)
        return fromBool(false);

    EventType eventType = (EventType)eventTypeVar->asNumber();

    if (eventType == KEYBOARD_KEYHIT)
    {
        Value *keysymVar = executionContextGetVariable(ctx, "_keysym");
        if (!keysymVar)
            return fromBool(false);

        int keysym = keysymVar->asNumber();
        std::string keyName = inputs[0].asString();

        // Map common key names to SDL keysyms
        // TODO: Add more key mappings as needed
        if (keyName == "space" && keysym == SDLK_SPACE)
            return fromBool(true);
        if (keyName == "up" && keysym == SDLK_UP)
            return fromBool(true);
        if (keyName == "down" && keysym == SDLK_DOWN)
            return fromBool(true);
        if (keyName == "left" && keysym == SDLK_LEFT)
            return fromBool(true);
        if (keyName == "right" && keysym == SDLK_RIGHT)
            return fromBool(true);
    }

    return fromBool(false);
}

Value execSpriteClicked(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    Value *mouseXVar = executionContextGetVariable(ctx, "_mouseX");
    Value *mouseYVar = executionContextGetVariable(ctx, "_mouseY");
    Value *eventTypeVar = executionContextGetVariable(ctx, "_eventType");

    if (!mouseXVar || !mouseYVar || !eventTypeVar)
        return fromBool(false);

    int mouseX = mouseXVar->asNumber();
    int mouseY = mouseYVar->asNumber();
    EventType eventType = (EventType)eventTypeVar->asNumber();

    if (eventType == MOUSE_BTN_LEFT_CLICK)
    {
        // Check if mouse is within sprite bounds
        // TODO: Implement proper sprite bounds checking
        return fromBool(true);
    }
    return fromBool(false);
}

// Control blocks implementation
Value execForever(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    // Handled by engine like repeat
    return fromNumber(-1); // -1 means infinite loop
}

Value execWait(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    // TODO: Implement proper wait with timing
    return Value();
}

Value execStop(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    // TODO: Implement stop execution
    return Value();
}

Value execIf(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    return inputs[0];
}

// Operator blocks implementation
Value execSubtract(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    return fromNumber(inputs[0].asNumber() - inputs[1].asNumber());
}

Value execMultiply(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    return fromNumber(inputs[0].asNumber() * inputs[1].asNumber());
}

Value execDivide(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    double divisor = inputs[1].asNumber();
    if (divisor == 0)
        return fromNumber(0);
    return fromNumber(inputs[0].asNumber() / divisor);
}

Value execLessThan(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    return fromBool(inputs[0].asNumber() < inputs[1].asNumber());
}

Value execGreaterThan(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    return fromBool(inputs[0].asNumber() > inputs[1].asNumber());
}

Value execEquals(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    return fromBool(inputs[0].asNumber() == inputs[1].asNumber());
}

Value execAnd(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    return fromBool(inputs[0].asBool() && inputs[1].asBool());
}

Value execOr(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    return fromBool(inputs[0].asBool() || inputs[1].asBool());
}

Value execNot(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    return fromBool(!inputs[0].asBool());
}

Value execRandom(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    int min = inputs[0].asNumber();
    int max = inputs[1].asNumber();
    return fromNumber(min + rand() % (max - min + 1));
}

Value execMod(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    int a = inputs[0].asNumber();
    int b = inputs[1].asNumber();
    if (b == 0)
        return fromNumber(0);
    return fromNumber(a % b);
}

// Reporter blocks implementation
Value execSpriteX(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    return fromNumber(ctx.sprite->posX);
}

Value execSpriteY(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    return fromNumber(ctx.sprite->posY);
}

Value execSpriteDirection(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    return fromNumber(ctx.sprite->angleRotation);
}

Value execTrue(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    return fromBool(true);
}

Value execFalse(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    return fromBool(false);
}

// Variable blocks implementation
Value execSetVariable(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    std::string varName = inputs[0].asString();
    executionContextSetVariable(ctx, varName.c_str(), inputs[1]);
    return Value();
}

Value execChangeVariable(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    std::string varName = inputs[0].asString();
    Value *var = executionContextGetVariable(ctx, varName.c_str());
    if (var)
    {
        double newValue = var->asNumber() + inputs[1].asNumber();
        executionContextSetVariable(ctx, varName.c_str(), fromNumber(newValue));
    }
    else
    {
        executionContextSetVariable(ctx, varName.c_str(), inputs[1]);
    }
    return Value();
}

Value execGetVariable(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    std::string varName = inputs[0].asString();
    Value *var = executionContextGetVariable(ctx, varName.c_str());
    if (var)
        return *var;
    return fromNumber(0);
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
    // [16] show
    {
        16,
        "show",
        CATEGORY_LOOKS,
        BLOCK_COMMAND,
        {},
        0,
        VALUE_NUMBER,
        true,
        true,
        0,
        execShow,
        100,
        40,
    },
    // [17] hide
    {
        17,
        "hide",
        CATEGORY_LOOKS,
        BLOCK_COMMAND,
        {},
        0,
        VALUE_NUMBER,
        true,
        true,
        0,
        execHide,
        100,
        40,
    },
    // [18] say %s
    {
        18,
        "say %s",
        CATEGORY_LOOKS,
        BLOCK_COMMAND,
        {
            {"message", VALUE_STRING, fromString("Hello!")},
        },
        1,
        VALUE_NUMBER,
        true,
        true,
        0,
        execSay,
        150,
        40,
    },
    // [19] think %s
    {
        19,
        "think %s",
        CATEGORY_LOOKS,
        BLOCK_COMMAND,
        {
            {"message", VALUE_STRING, fromString("Hmm...")},
        },
        1,
        VALUE_NUMBER,
        true,
        true,
        0,
        execThink,
        150,
        40,
    },
    // [20] change size by %s
    {
        20,
        "change size by %s",
        CATEGORY_LOOKS,
        BLOCK_COMMAND,
        {
            {"delta", VALUE_NUMBER, fromNumber(10)},
        },
        1,
        VALUE_NUMBER,
        true,
        true,
        0,
        execChangeSizeBy,
        150,
        40,
    },
    // [21] set size to %s
    {
        21,
        "set size to %s",
        CATEGORY_LOOKS,
        BLOCK_COMMAND,
        {
            {"size", VALUE_NUMBER, fromNumber(100)},
        },
        1,
        VALUE_NUMBER,
        true,
        true,
        0,
        execSetSizeTo,
        150,
        40,
    },
    // [22] when key %s pressed
    {
        22,
        "when key %s pressed",
        CATEGORY_EVENT,
        BLOCK_EVENT,
        {
            {"key", VALUE_STRING, fromString("space")},
        },
        1,
        VALUE_BOOLEAN,
        false,
        true,
        0,
        execKeyPressed,
        200,
        40,
    },
    // [23] when this sprite clicked
    {
        23,
        "when this sprite clicked",
        CATEGORY_EVENT,
        BLOCK_EVENT,
        {},
        0,
        VALUE_BOOLEAN,
        false,
        true,
        0,
        execSpriteClicked,
        200,
        40,
    },
    // [24] forever
    {
        24,
        "forever",
        CATEGORY_CONTROL,
        BLOCK_STATEMENT,
        {},
        0,
        VALUE_NUMBER,
        true,
        true,
        1,
        execForever,
        150,
        40,
    },
    // [25] if %s
    {
        25,
        "if %s",
        CATEGORY_CONTROL,
        BLOCK_STATEMENT,
        {
            {"condition", VALUE_BOOLEAN, fromBool(true)},
        },
        1,
        VALUE_NUMBER,
        true,
        true,
        1,
        execIf,
        150,
        40,
    },
    // [26] wait %s seconds
    {
        26,
        "wait %s seconds",
        CATEGORY_CONTROL,
        BLOCK_COMMAND,
        {
            {"seconds", VALUE_NUMBER, fromNumber(1)},
        },
        1,
        VALUE_NUMBER,
        true,
        true,
        0,
        execWait,
        150,
        40,
    },
    // [27] stop
    {
        27,
        "stop",
        CATEGORY_CONTROL,
        BLOCK_COMMAND,
        {},
        0,
        VALUE_NUMBER,
        true,
        true,
        0,
        execStop,
        100,
        40,
    },
    // [28] %s - %s
    {
        28,
        "%s - %s",
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
        execSubtract,
        100,
        30,
    },
    // [29] %s * %s
    {
        29,
        "%s * %s",
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
        execMultiply,
        100,
        30,
    },
    // [30] %s / %s
    {
        30,
        "%s / %s",
        CATEGORY_OPERATOR,
        BLOCK_OPERATOR,
        {
            {"a", VALUE_NUMBER, fromNumber(0)},
            {"b", VALUE_NUMBER, fromNumber(1)},
        },
        2,
        VALUE_NUMBER,
        false,
        false,
        0,
        execDivide,
        100,
        30,
    },
    // [31] %s < %s
    {
        31,
        "%s < %s",
        CATEGORY_OPERATOR,
        BLOCK_OPERATOR,
        {
            {"a", VALUE_NUMBER, fromNumber(0)},
            {"b", VALUE_NUMBER, fromNumber(0)},
        },
        2,
        VALUE_BOOLEAN,
        false,
        false,
        0,
        execLessThan,
        100,
        30,
    },
    // [32] %s > %s
    {
        32,
        "%s > %s",
        CATEGORY_OPERATOR,
        BLOCK_OPERATOR,
        {
            {"a", VALUE_NUMBER, fromNumber(0)},
            {"b", VALUE_NUMBER, fromNumber(0)},
        },
        2,
        VALUE_BOOLEAN,
        false,
        false,
        0,
        execGreaterThan,
        100,
        30,
    },
    // [33] %s = %s
    {
        33,
        "%s = %s",
        CATEGORY_OPERATOR,
        BLOCK_OPERATOR,
        {
            {"a", VALUE_NUMBER, fromNumber(0)},
            {"b", VALUE_NUMBER, fromNumber(0)},
        },
        2,
        VALUE_BOOLEAN,
        false,
        false,
        0,
        execEquals,
        100,
        30,
    },
    // [34] %s and %s
    {
        34,
        "%s and %s",
        CATEGORY_OPERATOR,
        BLOCK_OPERATOR,
        {
            {"a", VALUE_BOOLEAN, fromBool(false)},
            {"b", VALUE_BOOLEAN, fromBool(false)},
        },
        2,
        VALUE_BOOLEAN,
        false,
        false,
        0,
        execAnd,
        100,
        30,
    },
    // [35] %s or %s
    {
        35,
        "%s or %s",
        CATEGORY_OPERATOR,
        BLOCK_OPERATOR,
        {
            {"a", VALUE_BOOLEAN, fromBool(false)},
            {"b", VALUE_BOOLEAN, fromBool(false)},
        },
        2,
        VALUE_BOOLEAN,
        false,
        false,
        0,
        execOr,
        100,
        30,
    },
    // [36] not %s
    {
        36,
        "not %s",
        CATEGORY_OPERATOR,
        BLOCK_OPERATOR,
        {
            {"a", VALUE_BOOLEAN, fromBool(false)},
        },
        1,
        VALUE_BOOLEAN,
        false,
        false,
        0,
        execNot,
        80,
        30,
    },
    // [37] random %s to %s
    {
        37,
        "random %s to %s",
        CATEGORY_OPERATOR,
        BLOCK_OPERATOR,
        {
            {"from", VALUE_NUMBER, fromNumber(1)},
            {"to", VALUE_NUMBER, fromNumber(10)},
        },
        2,
        VALUE_NUMBER,
        false,
        false,
        0,
        execRandom,
        150,
        30,
    },
    // [38] %s mod %s
    {
        38,
        "%s mod %s",
        CATEGORY_OPERATOR,
        BLOCK_OPERATOR,
        {
            {"a", VALUE_NUMBER, fromNumber(0)},
            {"b", VALUE_NUMBER, fromNumber(1)},
        },
        2,
        VALUE_NUMBER,
        false,
        false,
        0,
        execMod,
        100,
        30,
    },
    // [39] x position
    {
        39,
        "x position",
        CATEGORY_MOTION,
        BLOCK_REPORTER,
        {},
        0,
        VALUE_NUMBER,
        false,
        false,
        0,
        execSpriteX,
        100,
        30,
    },
    // [40] y position
    {
        40,
        "y position",
        CATEGORY_MOTION,
        BLOCK_REPORTER,
        {},
        0,
        VALUE_NUMBER,
        false,
        false,
        0,
        execSpriteY,
        100,
        30,
    },
    // [41] direction
    {
        41,
        "direction",
        CATEGORY_MOTION,
        BLOCK_REPORTER,
        {},
        0,
        VALUE_NUMBER,
        false,
        false,
        0,
        execSpriteDirection,
        100,
        30,
    },
    // [42] true
    {
        42,
        "true",
        CATEGORY_OPERATOR,
        BLOCK_REPORTER,
        {},
        0,
        VALUE_BOOLEAN,
        false,
        false,
        0,
        execTrue,
        60,
        30,
    },
    // [43] false
    {
        43,
        "false",
        CATEGORY_OPERATOR,
        BLOCK_REPORTER,
        {},
        0,
        VALUE_BOOLEAN,
        false,
        false,
        0,
        execFalse,
        60,
        30,
    },
    // [44] set variable %s to %s
    {
        44,
        "set variable %s to %s",
        CATEGORY_VARIABLES,
        BLOCK_COMMAND,
        {
            {"varName", VALUE_STRING, fromString("myVar")},
            {"value", VALUE_NUMBER, fromNumber(0)},
        },
        2,
        VALUE_NUMBER,
        true,
        true,
        0,
        execSetVariable,
        200,
        40,
    },
    // [45] change variable %s by %s
    {
        45,
        "change variable %s by %s",
        CATEGORY_VARIABLES,
        BLOCK_COMMAND,
        {
            {"varName", VALUE_STRING, fromString("myVar")},
            {"delta", VALUE_NUMBER, fromNumber(1)},
        },
        2,
        VALUE_NUMBER,
        true,
        true,
        0,
        execChangeVariable,
        200,
        40,
    },
    // [46] variable %s
    {
        46,
        "variable %s",
        CATEGORY_VARIABLES,
        BLOCK_REPORTER,
        {
            {"varName", VALUE_STRING, fromString("myVar")},
        },
        1,
        VALUE_NUMBER,
        false,
        false,
        0,
        execGetVariable,
        120,
        30,
    },

};

int blocksLibraryCount = sizeof(blocksLibrary) / sizeof(blocksLibrary[0]);