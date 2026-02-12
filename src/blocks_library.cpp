/*
این فایل کتابخانه بلاک‌ها خواهد بود
*/

#include "block.h"
#include "value.h"
#include "execution_context.h"

Value execMoveSteps(ExecutionContext &ctx, Value inputs[], int inputCount)
{
    double steps = inputs[0].asNumber();
    ctx.spriteX += steps * SDL_cos(ctx.spriteAngle);
    ctx.spriteY += steps * SDL_sin(ctx.spriteAngle);
    return Value();
}
