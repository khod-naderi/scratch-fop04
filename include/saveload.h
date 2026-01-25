/*
---------------------------------------------------
This header file is for save and load logic
---------------------------------------------------
*/

#ifndef SAVELOAD_HEADER
#define SAVELOAD_HEADER

extern bool isOnLoadScreen;
extern bool isOnSaveScreen;

void drawSaveScreen();
void drawLoadScreen();
int saveAllData();
int loadAllData();
int newProject();
#endif