/*
engine is the core of this project
this will handle events and will run code blocks
*/

#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include "execution_context.h"
#include "block_instance.h"

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

void engineRunStep(int pid);
int enginePushProcess();
void engineEndProcess(int pid);
void enginePauseProcess(int pid);
void engineRemuseProcess(int pid);
void engineMouseEvent(int mouseX, int mouseY);
void engineKeyboardEvents(SDL_Keysym &key);
void engineRun();
Proccess *getProccessById(int pid);