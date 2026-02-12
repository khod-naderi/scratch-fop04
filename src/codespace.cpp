#include "codespace.h"

/*
To initialize an empty codespace
*/
void codespaceInit(CodeSpace &cs)
{
    cs.nextInstanceId = 0;
    for (int i = 0; i < MAX_INSTANCES; i++)
    {
        cs.instanceUsed[i] = false;
        blockInstanceConstruct(cs.instances[i]);
    }
}