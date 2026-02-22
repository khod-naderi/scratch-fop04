/*
engine is the core of this project
this will handle events and will run code blocks
*/

#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <stack>
#include "execution_context.h"
#include "block_instance.h"

enum EventType
{
    MOUSE_BTN_LEFT_CLICK,
    MOUSE_BTN_RIGHT_CLICK,
    MOUSE_MOTION,
    KEYBOARD_KEYHIT,
};

// Stack frame for handling loops and nested blocks
struct StackFrame
{
    BlockInstance *blockInst; // The block that created this frame (e.g., repeat, if-else)
    int loopCounter;          // For repeat blocks: current iteration
    int loopMax;              // For repeat blocks: total iterations
    int bodyIndex;            // Which body we're executing (for if-else: 0=if, 1=else)
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

    // call stack for handling loops and nested blocks
    std::stack<StackFrame> callStack;
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