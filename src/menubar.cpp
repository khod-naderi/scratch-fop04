#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "menubar.h"
#include "ui.h"
#include "color.h"
#include "generaldef.h"
#include <iostream>
#include "saveload.h"

// Define menu items
enum fileMenuBtn
{
    MN_NEW_PROJECT,
    MN_LOAD_PROJECT,
    MN_SAVE_PROJECT,
};
enum editMenuBtn
{
    MN_HELP,
};
const char *fileMenuItems[] = {"New Project", "Load Project", "Save Project"};
const int fileMenuItemsCount = 3;
const char *editMenuItems[] = {"Help"};
const int editMenuItemsCount = 1;

// Define menu state
MenuState menuState = {false, false, -1, -1};

// Define texture and surface pointers
SDL_Surface *logoSurface = nullptr;
SDL_Texture *logoTexture = nullptr;
SDL_Texture *fileMenuText = nullptr;
SDL_Texture *editMenuText = nullptr;

int menu_init(SDL_Renderer *renderer, TTF_Font *font)
{

    logoTexture = renderImage(renderer, "assets/images/logo.png");
    if (!logoTexture)
    {
        std::cerr << "Faild to load logo image: " << IMG_GetError() << std::endl;
        return SDL_IMAGE_LOAD_ERROR;
    }

    fileMenuText = renderText(renderer, font, "File", color_white);
    editMenuText = renderText(renderer, font, "Edit", color_white);

    return 0;
}

// Update hover states of menus
void updateMenuHoverState(int mouseX, int mouseY)
{
    menuState.hoveredFileItem = -1;
    menuState.hoveredEditItem = -1;

    if (menuState.isfileMenuOpen && mouseY >= MENUBAR_HEIGHT)
    {
        int itemIndex = (mouseY - MENUBAR_HEIGHT) / MENU_DROPDOX_ITEM_HEIGHT;
        if (mouseX >= fileMenuRect.x && mouseX < fileMenuRect.x + MENU_DROPDOX_WIDTH &&
            itemIndex >= 0 && itemIndex < fileMenuItemsCount)
        {
            menuState.hoveredFileItem = itemIndex;
        }
    }

    if (menuState.iseditMenuOpen && mouseY >= MENUBAR_HEIGHT)
    {
        int itemIndex = (mouseY - MENUBAR_HEIGHT) / MENU_DROPDOX_ITEM_HEIGHT;
        if (mouseX >= editMenuRect.x && mouseX < editMenuRect.x + MENU_DROPDOX_WIDTH &&
            itemIndex >= 0 && itemIndex < editMenuItemsCount)
        {
            menuState.hoveredEditItem = itemIndex;
        }
    }
}

/*
---------------------------------------------
Draw menubar
---------------------------------------------
*/
void drawMenubar(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY)
{

    // Draw blue rectangle at top as menu bar
    SDL_SetRenderDrawColor(renderer, color_menubarBackground);
    SDL_Rect menubar = {
        0,
        0,
        MAIN_WINDOW_WIDTH,
        MENUBAR_HEIGHT};
    SDL_RenderFillRect(renderer, &menubar);

    // Draw Logo
    SDL_Rect logoRect = {
        LOGO_MARGIN_LEFT, // margin left
        (MENUBAR_HEIGHT - LOGO_HEIGHT) / 2,
        LOGO_WIDTH,
        LOGO_HEIGHT};

    SDL_RenderCopy(renderer, logoTexture, NULL, &logoRect);

    // Draw File menu button
    bool fileMenuHoverd = isPointInRect(mouseX, mouseY, fileMenuRect);
    if (fileMenuHoverd || menuState.isfileMenuOpen)
    {
        SDL_SetRenderDrawColor(renderer, color_hoveredMenuButton);
    }
    else
    {
        SDL_SetRenderDrawColor(renderer, color_menubarBackground);
    }
    SDL_RenderFillRect(renderer, &fileMenuRect);

    // Draw File menu text
    if (fileMenuText)
    {
        int tw, th;
        SDL_QueryTexture(fileMenuText, NULL, NULL, &tw, &th);
        SDL_Rect textRect = {
            fileMenuRect.x + (fileMenuRect.w - tw) / 2,
            fileMenuRect.y + (fileMenuRect.h - th) / 2,
            tw,
            th,
        };
        SDL_RenderCopy(renderer, fileMenuText, NULL, &textRect);
    }

    // Draw Edit menu button
    bool editMenuHoverd = isPointInRect(mouseX, mouseY, editMenuRect);
    if (editMenuHoverd || menuState.iseditMenuOpen)
    {
        SDL_SetRenderDrawColor(renderer, color_hoveredMenuButton);
    }
    else
    {
        SDL_SetRenderDrawColor(renderer, color_menubarBackground);
    }
    SDL_RenderFillRect(renderer, &editMenuRect);

    // Draw Edit menu text
    if (editMenuText)
    {
        int tw, th;
        SDL_QueryTexture(editMenuText, NULL, NULL, &tw, &th);
        SDL_Rect textRect = {
            editMenuRect.x + (editMenuRect.w - tw) / 2,
            editMenuRect.y + (editMenuRect.h - th) / 2,
            tw,
            th,
        };
        SDL_RenderCopy(renderer, editMenuText, NULL, &textRect);
    }

    // Draw File menu Dropbox
    if (menuState.isfileMenuOpen)
    {
        for (int i = 0; i < fileMenuItemsCount; i++)
        {
            SDL_Rect itemRect = {
                fileMenuRect.x,
                MENUBAR_HEIGHT + i * MENU_DROPDOX_ITEM_HEIGHT,
                MENU_DROPDOX_WIDTH,
                MENU_DROPDOX_ITEM_HEIGHT,
            };

            // Background
            if (i == menuState.hoveredFileItem)
            {
                SDL_SetRenderDrawColor(renderer, color_hoveredMenuOption);
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, color_menuDropbox);
            }
            SDL_RenderFillRect(renderer, &itemRect);

            // Border
            SDL_SetRenderDrawColor(renderer, color_borderMenuDropbox);
            SDL_RenderDrawRect(renderer, &itemRect);

            // Text
            SDL_Texture *itemTextText = renderText(renderer, font, fileMenuItems[i], color_black);
            if (itemTextText)
            {
                int tw, th;
                SDL_QueryTexture(itemTextText, NULL, NULL, &tw, &th);
                SDL_Rect textRect = {
                    itemRect.x + 10,
                    itemRect.y + (itemRect.h - th) / 2,
                    tw,
                    th,
                };
                SDL_RenderCopy(renderer, itemTextText, NULL, &textRect);
            }
        }
    }

    // Draw Edit menu Dropbox
    if (menuState.iseditMenuOpen)
    {
        for (int i = 0; i < editMenuItemsCount; i++)
        {
            SDL_Rect itemRect = {
                editMenuRect.x,
                MENUBAR_HEIGHT + i * MENU_DROPDOX_ITEM_HEIGHT,
                MENU_DROPDOX_WIDTH,
                MENU_DROPDOX_ITEM_HEIGHT,
            };

            // Background
            if (i == menuState.hoveredEditItem)
            {
                SDL_SetRenderDrawColor(renderer, color_hoveredMenuOption);
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, color_menuDropbox);
            }
            SDL_RenderFillRect(renderer, &itemRect);

            // Border
            SDL_SetRenderDrawColor(renderer, color_borderMenuDropbox);
            SDL_RenderDrawRect(renderer, &itemRect);

            // Text
            SDL_Texture *itemTextText = renderText(renderer, font, editMenuItems[i], color_black);
            if (itemTextText)
            {
                int tw, th;
                SDL_QueryTexture(itemTextText, NULL, NULL, &tw, &th);
                SDL_Rect textRect = {
                    itemRect.x + 10,
                    itemRect.y + (itemRect.h - th) / 2,
                    tw,
                    th,
                };
                SDL_RenderCopy(renderer, itemTextText, NULL, &textRect);
            }
        }
    }

    // draw green flag
    filledTrigonRGBA(renderer, GREENFLAG_BTN.x, GREENFLAG_BTN.y, GREENFLAG_BTN.x, GREENFLAG_BTN.y + GREENFLAG_BTN.h, GREENFLAG_BTN.x + GREENFLAG_BTN.w, GREENFLAG_BTN.y + GREENFLAG_BTN.h / 2, 0, 255, 0, 255);
}

/*
---------------------------------------------
In this part we check if any part of menubar or menu options clicked
---------------------------------------------
*/
void controlMenubarClickDown(const int mouseX, const int mouseY)
{
    // Check if File menu clicked
    if (isPointInRect(mouseX, mouseY, fileMenuRect))
    {
        menuState.isfileMenuOpen = !menuState.isfileMenuOpen;
        menuState.iseditMenuOpen = false;
    }
    // Check if Edit menu clicked
    else if (isPointInRect(mouseX, mouseY, editMenuRect))
    {
        menuState.iseditMenuOpen = !menuState.iseditMenuOpen;
        menuState.isfileMenuOpen = false;
    }
    // Check if File dropdown item clicked
    else if (menuState.isfileMenuOpen)
    {
        SDL_Rect fileDropdown = {fileMenuRect.x, MENUBAR_HEIGHT,
                                 MENU_DROPDOX_WIDTH, MENU_DROPDOX_ITEM_HEIGHT * fileMenuItemsCount};
        if (isPointInRect(mouseX, mouseY, fileDropdown))
        {
            int itemIndex = (mouseY - MENUBAR_HEIGHT) / MENU_DROPDOX_ITEM_HEIGHT;
            if (itemIndex >= 0 && itemIndex < fileMenuItemsCount)
            {
                switch (itemIndex)
                {
                case MN_NEW_PROJECT:
                    newProject();
                    break;
                case MN_SAVE_PROJECT:
                    isOnSaveScreen = true;
                    break;
                case MN_LOAD_PROJECT:
                    isOnLoadScreen = true;
                    break;
                default:
                    break;
                }
                menuState.isfileMenuOpen = false;
            }
        }
        else
        {
            menuState.isfileMenuOpen = false;
        }
    }
    // Check if Edit dropdown item clicked
    else if (menuState.iseditMenuOpen)
    {
        SDL_Rect editDropdown = {editMenuRect.x, MENUBAR_HEIGHT,
                                 MENU_DROPDOX_WIDTH, MENU_DROPDOX_ITEM_HEIGHT * editMenuItemsCount};
        if (isPointInRect(mouseX, mouseY, editDropdown))
        {
            int itemIndex = (mouseY - MENUBAR_HEIGHT) / MENU_DROPDOX_ITEM_HEIGHT;
            if (itemIndex >= 0 && itemIndex < editMenuItemsCount)
            {
                switch (itemIndex)
                {
                case MN_HELP:
                    break;

                default:
                    break;
                }
                menuState.iseditMenuOpen = false;
            }
        }
        else
        {
            menuState.iseditMenuOpen = false;
        }
    }
    else
    {
        menuState.isfileMenuOpen = false;
        menuState.iseditMenuOpen = false;
    }
}