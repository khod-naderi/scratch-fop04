#include "engine.h"
#include "workspace.h"
#include "blocks_library.h"
#include "canvas.h"
#include "logger.h"
#include <iostream>
#include "spritebox.h"

int lastProccessId = 0;
std::vector<Proccess> programCounters;

/*
This function will return pointer to proccess that have same pid
*/
Proccess *getProccessById(int pid)
{
    for (Proccess &p : programCounters)
        if (p.id == pid)
            return &p;

    return nullptr;
}

/*
this function will pause proccess with same pid (if exists)
*/
void enginePauseProcess(int pid)
{
    Proccess *p = getProccessById(pid);
    if (p)
        p->isRunning = false;
}

/*
this function will remuse proccess with same pid (if exists)
*/
void engineRemuseProcess(int pid)
{
    Proccess *p = getProccessById(pid);
    if (p)
        p->isRunning = true;
}

/*
this will kill a proccess
*/
void engineEndProcess(int pid)
{
    if (programCounters.empty())
        return;

    for (auto it = programCounters.begin(); it != programCounters.end(); ++it)
    {
        if (it->id == pid)
        {
            programCounters.erase(it);
            break;
        }
    }
}

/*
this will create a proccess
*/
int enginePushProcess(ExecutionContext *ctx, BlockInstance *nowInst)
{
    Proccess newP;
    newP.ctx = ctx;
    newP.nowInst = nowInst;
    newP.isRunning = true;
    newP.id = lastProccessId++;

    programCounters.push_back(newP);

    return newP.id;
}

Value *getValueArray(BlockInstance *inst, ExecutionContext &ctx)
{
    updateBlockInstanceInputValues(*inst);
    Value *out = new Value[inst->inputCount];

    for (int i = 0; i < inst->inputCount; i++)
    {
        if (inst->inputs[i].isBlock)
        {
            // Get the block instance that provides the value
            BlockInstance *inputBlockInst = findBlockInstanceById(inst->inputs[i].blockInstanceId);
            if (inputBlockInst)
            {
                // Recursively evaluate the input block to get its value
                const Block *inputBlockDef = &blocksLibrary[inputBlockInst->defenitionId];
                Value *nestedInputs = getValueArray(inputBlockInst, ctx);

                // Execute the input block to get its return value
                out[i] = inputBlockDef->execute(ctx, nestedInputs, inputBlockInst->inputCount);

                delete[] nestedInputs;
            }
            else
            {
                // Fallback to literal if block not found
                out[i] = inst->inputs[i].literal;
            }
        }
        else
        {
            out[i] = inst->inputs[i].literal;
        }
    }

    return out;
}

void engineRunStep(Proccess *p)
{
    if (!p || !p->nowInst)
        return;

    const Block *def = &blocksLibrary[p->nowInst->defenitionId];
    Value *inputs = getValueArray(p->nowInst, *p->ctx);
    Value result = def->execute(*p->ctx, inputs, p->nowInst->inputCount);

    delete[] inputs;

    // Handle blocks with bodies (loops, if-else, etc.)
    if (def->type == BLOCK_STATEMENT && p->nowInst->bodyCount > 0)
    {
        if (def->id == 3) // Repeat block
        {
            // Check if we're already in a loop for this block
            bool inLoop = false;
            if (!p->callStack.empty() && p->callStack.top().blockInst == p->nowInst)
            {
                inLoop = true;
            }

            if (!inLoop)
            {
                // First time entering this repeat block
                int repeatCount = result.asNumber();
                if (repeatCount > 0 && p->nowInst->bodies[0] != -1)
                {
                    // Push a new stack frame for this loop
                    StackFrame frame;
                    frame.blockInst = p->nowInst;
                    frame.loopCounter = 0;
                    frame.loopMax = repeatCount;
                    frame.bodyIndex = 0;
                    p->callStack.push(frame);

                    // Enter the body
                    p->nowInst = findBlockInstanceById(p->nowInst->bodies[0]);
                    return;
                }
            }
            else
            {
                // We're returning from the body, check if we need to loop again
                StackFrame &frame = p->callStack.top();
                frame.loopCounter++;

                if (frame.loopCounter < frame.loopMax)
                {
                    // Loop again
                    p->nowInst = findBlockInstanceById(frame.blockInst->bodies[0]);
                    return;
                }
                else
                {
                    // Loop finished, pop the stack frame
                    BlockInstance *loopBlock = frame.blockInst;
                    p->callStack.pop();

                    // Continue after the loop block
                    p->nowInst = findBlockInstanceById(loopBlock->nextId);
                    if (!p->nowInst)
                        engineEndProcess(p->id);
                    return;
                }
            }
        }
        else if (def->id == 24) // Forever block
        {
            // Check if we're already in a loop for this block
            bool inLoop = false;
            if (!p->callStack.empty() && p->callStack.top().blockInst == p->nowInst)
            {
                inLoop = true;
            }

            if (!inLoop)
            {
                // First time entering this forever block
                if (p->nowInst->bodies[0] != -1)
                {
                    // Push a new stack frame for this infinite loop
                    StackFrame frame;
                    frame.blockInst = p->nowInst;
                    frame.loopCounter = 0;
                    frame.loopMax = -1; // -1 means infinite
                    frame.bodyIndex = 0;
                    p->callStack.push(frame);

                    // Enter the body
                    p->nowInst = findBlockInstanceById(p->nowInst->bodies[0]);
                    return;
                }
            }
            else
            {
                // We're returning from the body, loop forever
                StackFrame &frame = p->callStack.top();
                frame.loopCounter++;

                // Always loop again (infinite loop)
                p->nowInst = findBlockInstanceById(frame.blockInst->bodies[0]);
                return;
            }
        }
        else if (def->id == 5) // If-else block
        {
            // The condition is in the result
            bool condition = result.asBool();

            if (condition && p->nowInst->bodies[0] != -1)
            {
                // Enter the "if" body (body 0)
                StackFrame frame;
                frame.blockInst = p->nowInst;
                frame.loopCounter = 0;
                frame.loopMax = 1;
                frame.bodyIndex = 0;
                p->callStack.push(frame);

                p->nowInst = findBlockInstanceById(p->nowInst->bodies[0]);
                return;
            }
            else if (!condition && p->nowInst->bodyCount > 1 && p->nowInst->bodies[1] != -1)
            {
                // Enter the "else" body (body 1)
                StackFrame frame;
                frame.blockInst = p->nowInst;
                frame.loopCounter = 0;
                frame.loopMax = 1;
                frame.bodyIndex = 1;
                p->callStack.push(frame);

                p->nowInst = findBlockInstanceById(p->nowInst->bodies[1]);
                return;
            }
        }
        else if (def->id == 25) // If block (without else)
        {
            // The condition is in the result
            bool condition = result.asBool();

            if (condition && p->nowInst->bodies[0] != -1)
            {
                // Enter the "if" body
                StackFrame frame;
                frame.blockInst = p->nowInst;
                frame.loopCounter = 0;
                frame.loopMax = 1;
                frame.bodyIndex = 0;
                p->callStack.push(frame);

                p->nowInst = findBlockInstanceById(p->nowInst->bodies[0]);
                return;
            }
        }
    }

    // Check if we're at the end of a body and need to return to parent
    if (!p->callStack.empty())
    {
        // We've reached the end of a body (no nextId)
        if (p->nowInst->nextId == -1)
        {
            StackFrame &frame = p->callStack.top();

            // For repeat blocks, we need to check if we should loop
            const Block *parentDef = &blocksLibrary[frame.blockInst->defenitionId];
            if (parentDef->id == 3) // Repeat block
            {
                frame.loopCounter++;
                if (frame.loopCounter < frame.loopMax)
                {
                    // Loop again
                    p->nowInst = findBlockInstanceById(frame.blockInst->bodies[0]);
                    return;
                }
            }
            else if (parentDef->id == 24) // Forever block
            {
                // Always loop again (infinite loop)
                frame.loopCounter++;
                p->nowInst = findBlockInstanceById(frame.blockInst->bodies[0]);
                return;
            }

            // Pop the frame and continue after the parent block
            BlockInstance *parentBlock = frame.blockInst;
            p->callStack.pop();
            p->nowInst = findBlockInstanceById(parentBlock->nextId);

            if (!p->nowInst)
                engineEndProcess(p->id);
            return;
        }
    }

    // prepare for next step
    p->nowInst = findBlockInstanceById(p->nowInst->nextId);
    if (!p->nowInst)
        engineEndProcess(p->id);
}

void engineRunStep(int pid)
{
    Proccess *p = getProccessById(pid);
    if (p)
        engineRunStep(p);
}

/*
this fucntion will run every frame and run each running procces one step
*/
void engineRun()
{
    for (Proccess &p : programCounters)
    {
        if (p.isRunning)
            engineRunStep(&p);
    }
}

/*
this function will handle events to run related proccess
*/
void engineEventHandler(SDL_Event &event)
{
    CodeSpace *cs = &workspaceCodeSpace; // TODO: get it dynamicly
    for (int i = 0; i < cs->nextInstanceId; i++)
    {
        if (!cs->instanceUsed[i])
            continue;

        const Block *def = &blocksLibrary[cs->instances[i].defenitionId];

        if (def->type != BLOCK_EVENT)
            continue;

        ExecutionContext *ctx = new ExecutionContext();

        SprintBody *spt = getSprintById(SelectedSpriteID);
        if (spt)
            ctx->sprite = spt;
        else
            ctx->sprite = &aliveSprints[0];

        executionContextSetVariable(*ctx, "_mouseX", fromNumber(event.motion.x));
        executionContextSetVariable(*ctx, "_mouseY", fromNumber(event.motion.y));

        EventType eventType;
        if (event.type == SDL_MOUSEMOTION)
        {
            eventType = MOUSE_MOTION;
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                eventType = MOUSE_BTN_LEFT_CLICK;
            }
        }
        else if (event.type == SDL_KEYDOWN)
        {
            eventType = KEYBOARD_KEYHIT;
        }

        if (eventType == KEYBOARD_KEYHIT)
        {
            executionContextSetVariable(*ctx, "_keysym", fromNumber((int)event.key.keysym.sym));
        }
        executionContextSetVariable(*ctx, "_eventType", fromNumber((int)eventType));

        // Get the input values for the event block
        Value *inputs = getValueArray(&cs->instances[i], *ctx);
        bool isTrigered = def->execute(*ctx, inputs, cs->instances[i].inputCount).asBool();
        delete[] inputs;

        if (isTrigered)
        {
            enginePushProcess(ctx, findBlockInstanceById(cs->instances[i].nextId));
            std::cout << "New proccess pushed." << std::endl;
        }
    }
}