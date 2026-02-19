#include "engine.h"
#include "workspace.h"
#include "blocks_library.h"
#include "canvas.h"

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
        if (it->id == pid)
            programCounters.erase(it);
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

void engineRunStep(Proccess *p)
{

    // prepare for next step
    p->nowInst = findBlockInstanceById(p->nowInst->nextId);
    if (!p->nowInst)
        engineEndProcess(p->id);
}

void engineRunStep(int pid)
{
    Proccess *p = getProccessById(pid);
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

        ExecutionContext ctx;

        SprintBody *spt = &aliveSprints[0]; // TODO: get it dynamicly

        ctx.sprite = spt;

        executionContextSetVariable(ctx, "_mouseX", fromNumber(event.motion.x));
        executionContextSetVariable(ctx, "_mouseY", fromNumber(event.motion.y));
        executionContextSetVariable(ctx, "_keysym", fromNumber((int)event.key.keysym.sym));

        def->execute(ctx, nullptr, 0);
    }
}