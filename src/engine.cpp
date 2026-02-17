#include "engine.h"
#include "workspace.h"

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