/*
    This header file is for handling menubar View and Control
*/

#ifndef MENU_HEADER
#define MENU_HEADER

/*
---------------------------------------
Menubar properties
---------------------------------------
*/
#define MENUBAR_HEIGHT 40
#define MENU_MARGIN_LEFT 10
#define MENU_DROPDOX_WIDTH 150
#define MENU_DROPDOX_ITEM_HEIGHT 30

// handle state of menu buttons
struct MenuState
{
    bool isfileMenuOpen;
    bool iseditMenuOpen;
    int hoveredFileItem;
    int hoveredEditItem;
};

// File menu Items
const char *fileMenuItems[] = {"New Project", "Load Project", "Save Project"};
const unsigned int fileMenuItemsCount = 3;
// Edit menu items
const char *editMenuItems[] = {"Help"};
const unsigned int editMenuItemsCount = 1;

#endif