#include "engine.h"
#include "workspace.h"
#include "blocks_library.h"
#include "canvas.h"
#include "logger.h"
#include <iostream>

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

Value *getValueArray(BlockInstance *inst)
{
    Value *out = new Value[inst->inputCount];

    for (int i = 0; i < inst->inputCount; i++)
    {
        if (inst->inputs[i].isBlock)
        {
            const BlockInstance *inst = findBlockInstanceById(inst->inputs[i].blockInstanceId);
            out[i] = inst->inputs[i].literal;
            // TODO: make it recursively work
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
    Value *inputs = getValueArray(p->nowInst);
    def->execute(*p->ctx, inputs, p->nowInst->inputCount);

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

        SprintBody *spt = &aliveSprints[0]; // TODO: get it dynamicly

        ctx->sprite = spt;

        executionContextSetVariable(*ctx, "_mouseX", fromNumber(event.motion.x));
        executionContextSetVariable(*ctx, "_mouseY", fromNumber(event.motion.y));
        executionContextSetVariable(*ctx, "_keysym", fromNumber((int)event.key.keysym.sym));

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
        executionContextSetVariable(*ctx, "_eventType", fromNumber((int)eventType));

        bool isTrigered = def->execute(*ctx, nullptr, 0).asBool();
        if (isTrigered)
        {
            enginePushProcess(ctx, findBlockInstanceById(cs->instances[i].nextId));
            std::cout << "New proccess pushed." << std::endl;
        }
    }
}