/*
engine is the core of this project
this will handle events and will run code blocks
*/

#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include "execution_context.h"
#include "block_instance.h"

enum EventType
{
    MOUSE_BTN_LEFT_CLICK,
    MOUSE_BTN_RIGHT_CLICK,
    MOUSE_MOTION,
    KEYBOARD_KEYHIT,
};

struct Proccess
{
    // identifier
    int id;

    // execution ctx for this proccess
    ExecutionContext *ctx;

    // block instance that procces should run that on this step
    BlockInstance *nowInst;

    // properties
    bool isRunning;
};

extern std::vector<Proccess> programCounters;

// proccess routins
void engineEndProcess(int pid);
int enginePushProcess(ExecutionContext *ctx, BlockInstance *nowInst);
void enginePauseProcess(int pid);
void engineRemuseProcess(int pid);
Proccess *getProccessById(int pid);

// engine routins
void engineRunStep(int pid);
void engineRunStep(Proccess *p);
void engineRun();

// engine event handler
void engineEventHandler(SDL_Event &event);