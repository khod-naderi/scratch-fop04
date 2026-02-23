/*
This file is for defineing default properties.
- error handling defenitions
- initialize page defenitions
*/

#ifndef GENERALDEF
#define GENERALDEF

/*
---------------------------------------
Error Codes
---------------------------------------
*/
#define SDL_INITIALIZE_ERROR 1
#define SDL_IMAGE_LOAD_ERROR 2
#define SDL_TTF_INIT_ERROR 3
#define SDL_FONT_NOT_FOUND 4

/*
---------------------------------------
Page properties
---------------------------------------
*/

#define MAIN_WINDOW_WIDTH 1800
#define MAIN_WINDOW_HEIGHT 1000

/*
---------------------------------------
Menubar Logo properties
---------------------------------------
*/
#define LOGO_MARGIN_LEFT 10
#define LOGO_HEIGHT (MENUBAR_HEIGHT - 5)
#define LOGO_WIDTH (int(LOGO_HEIGHT * 2.675))

// for DebugMode
#define DEBUG_MODE
// for adding the three tabs
enum EditorTab
{
    TAB_CODE,
    TAB_COSTUME,
    TAB_SOUND
};

inline EditorTab currentTab = TAB_CODE;

#endif