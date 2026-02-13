/*
-------------------------------------------
This header file is for managing custome and backgorund setting area named sprite box
-------------------------------------------
*/

// most of functions are called in the function : dawSpriteBoxScreen,
// which is called in main loop to render sprite box every frame.

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "spritebox.h"
#include "color.h"
#include "ui.h"
#include "generaldef.h"
#include <iostream>
#include "canvas.h"

// selected sprite id in sprite box
int SelectedSpriteID = -1;

// these are for adding png icons for add sprite menu
SDL_Texture* catIcon = nullptr;
SDL_Texture* paintIcon = nullptr;
SDL_Texture* uploadIcon = nullptr;
SDL_Texture* randomIcon = nullptr;
SDL_Texture* searchIcon = nullptr;

// these global variables are for handling add sprite button and menu
AddSpriteMenuState addSpriteMenuState = Menu_Closed;
SDL_Rect AddSpriteButtonRect ;
SDL_Rect MenuToolBarRect;
MenuOption menuOptions[4];
bool isHoveringAddSpriteButton = false;

/*
-------------------------------------------------
This function is for adding and initializeing top bar of sprite box
this function will run only one time.
-------------------------------------------------
*/

void DrawTopBarOfSpriteBox(SDL_Renderer *renderer, TTF_Font *font, SDL_Rect topBar) {
    SDL_Color textColor = color_black;

    // 1. drawing "Sprite Label" text on top bar
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Sprite: ", textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    int labelX = topBar.x + 20; 
    int labelY = topBar.y + 20;

    SDL_Rect labelRect = {labelX, labelY, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, nullptr, &labelRect);

    // drawing a white rectangle for showing selected sprite name
    SDL_Rect nameBox = { labelRect.x + labelRect.w , labelRect.y - 5, 130, 30 };
    SDL_SetRenderDrawColor(renderer, color_white);
    SDL_RenderFillRect(renderer, &nameBox);
    SDL_SetRenderDrawColor(renderer, color_black);
    SDL_RenderDrawRect(renderer, &nameBox);

    // 2. drawing X position 
    int xLabelX = labelX + textSurface->w + 10 + 150 + 20;
    int xLabelY = labelY;

    SDL_Surface* xLabelSurface = TTF_RenderText_Solid(font, "x ", textColor);
    SDL_Texture* xLabelTexture = SDL_CreateTextureFromSurface(renderer, xLabelSurface);
    SDL_Rect xLabelRect = {xLabelX, xLabelY, xLabelSurface->w, xLabelSurface->h};
    SDL_RenderCopy(renderer, xLabelTexture, nullptr, &xLabelRect);

    // X box
    SDL_Rect xBox = { xLabelRect.x + xLabelRect.w + 10 , xLabelRect.y - 6, 50, 30 };
    SDL_SetRenderDrawColor ( renderer, color_white);
    SDL_RenderFillRect(renderer, &xBox);
    SDL_SetRenderDrawColor(renderer, color_black);
    SDL_RenderDrawRect(renderer, &xBox);

    // 3. drawing Y position
    int yLabelX = xBox.x + xBox.w + 20;
    int yLabelY = labelY;
    SDL_Surface* yLabelSurface = TTF_RenderText_Solid(font, "y ", textColor);
    SDL_Texture* yLabelTexture = SDL_CreateTextureFromSurface(renderer, yLabelSurface);
    SDL_Rect yLabelRect = {yLabelX, yLabelY, yLabelSurface->w, yLabelSurface->h};
    SDL_RenderCopy(renderer, yLabelTexture, nullptr, &yLabelRect);
    
    // Y box 
    SDL_Rect yBox = { yLabelRect.x + yLabelRect.w + 10 , yLabelRect.y - 6, 50, 30 };
    SDL_SetRenderDrawColor(renderer, color_white);
    SDL_RenderFillRect(renderer, &yBox);
    SDL_SetRenderDrawColor(renderer, color_black);
    SDL_RenderDrawRect(renderer, &yBox);

    // 4. visibility box 
    int showLabelX = topBar.x + 20;
    int showLabelY = labelY + 50;

    SDL_Surface* showLabelSurface = TTF_RenderText_Solid(font, "Visibility", textColor);
    SDL_Texture* showLabelTexture = SDL_CreateTextureFromSurface(renderer, showLabelSurface);
    SDL_Rect showLabelRect = {showLabelX, showLabelY, showLabelSurface->w, showLabelSurface->h};
    SDL_RenderCopy(renderer, showLabelTexture, nullptr, &showLabelRect);

    // visibility box
    SDL_Rect showBox = { showLabelX + showLabelRect.w + 10 , showLabelRect.y - 6, 50, 30 };
    SDL_SetRenderDrawColor(renderer, color_white);
    SDL_RenderFillRect(renderer, &showBox);
    SDL_SetRenderDrawColor(renderer, color_black);
    SDL_RenderDrawRect(renderer, &showBox);

    // 5. size box ( scale)

    int sizeLabelX = showLabelX + showBox.w + 30 + 50 + 30;
    int sizeLabelY = showLabelY;
    SDL_Surface* sizeLabelSurface = TTF_RenderText_Solid(font, "Size", textColor);
    SDL_Texture* sizeLabelTexture = SDL_CreateTextureFromSurface(renderer, sizeLabelSurface);
    SDL_Rect sizeLabelRect = {sizeLabelX, sizeLabelY, sizeLabelSurface->w, sizeLabelSurface->h};
    SDL_RenderCopy(renderer, sizeLabelTexture, nullptr, &sizeLabelRect);

    // size box
    SDL_Rect sizeBox = { sizeLabelRect.x + sizeLabelRect.w + 10 , sizeLabelRect.y - 6, 60, 30 };
    SDL_SetRenderDrawColor(renderer, color_white);
    SDL_RenderFillRect(renderer, &sizeBox);
    SDL_SetRenderDrawColor(renderer, color_black);
    SDL_RenderDrawRect(renderer, &sizeBox);

    // 6. rotation box
    int rotationLabelX = sizeLabelX + sizeBox.w + 30 + 50 + 10;
    int rotationLabelY = sizeLabelY;

    SDL_Surface* rotationLabelSurface = TTF_RenderText_Solid(font, "Rotation", textColor);
    SDL_Texture* rotationLabelTexture = SDL_CreateTextureFromSurface(renderer, rotationLabelSurface);
    SDL_Rect rotationLabelRect = {rotationLabelX, rotationLabelY, rotationLabelSurface->w, rotationLabelSurface->h};
    SDL_RenderCopy(renderer, rotationLabelTexture, nullptr, &rotationLabelRect);

    // rotation box 
    SDL_Rect rotationBox = { rotationLabelRect.x + rotationLabelRect.w + 10 , rotationLabelRect.y - 6, 60, 30 };
    SDL_SetRenderDrawColor(renderer, color_white);
    SDL_RenderFillRect(renderer, &rotationBox);
    SDL_SetRenderDrawColor(renderer, color_black);
    SDL_RenderDrawRect(renderer, &rotationBox);

    // end
    SDL_FreeSurface(textSurface);
    SDL_FreeSurface(xLabelSurface);
    SDL_FreeSurface(yLabelSurface);
    SDL_DestroyTexture(textTexture);
    SDL_DestroyTexture(xLabelTexture);
    SDL_DestroyTexture(yLabelTexture);
}

/*
-------------------------------------------------
This function is for drawing Sprite box.
this function will run on every frame.
-------------------------------------------------
*/

void DrawAddSpriteMenu ( SDL_Renderer *renderer, TTF_Font *font, SDL_Rect SpriteArea ,
     const int mouseX, const int mouseY ) { 
        const int buttonSize = 80;
        AddSpriteButtonRect = { SpriteArea.x + SpriteArea.w - buttonSize - 17,
             SpriteArea.y + 17, buttonSize, buttonSize };

        // hovering effect for add sprite button
        isHoveringAddSpriteButton = ( mouseX >= AddSpriteButtonRect.x && mouseX <= AddSpriteButtonRect.x + AddSpriteButtonRect.w &&
                                     mouseY >= AddSpriteButtonRect.y && mouseY <= AddSpriteButtonRect.y + AddSpriteButtonRect.h);
        

        // updating menu state based on hovering status
        if ( isHoveringAddSpriteButton ) {
            addSpriteMenuState = Menu_Hover_Main;
        }
        else if ( addSpriteMenuState == Menu_Hover_Main ) {
            addSpriteMenuState = Menu_Closed;
        }

        // drawing add sprite button
        if ( isHoveringAddSpriteButton) {
            SDL_SetRenderDrawColor(renderer, 100, 200 , 100 , 255 ); // green color for hovered state
        }
        else {
            SDL_SetRenderDrawColor(renderer, 150, 100, 200, 255 ); // purple color for normal state
        }

        SDL_RenderFillRect(renderer, &AddSpriteButtonRect);

        //Draw border for add sprite button
        SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255);
        SDL_RenderDrawRect(renderer, &AddSpriteButtonRect);

        // now using cat icon for add sprite button
        if (catIcon) {
            SDL_RenderCopy(renderer, catIcon, nullptr, &AddSpriteButtonRect);
        }

        // adding one layer color ( to fix the hovering problem )
        if ( isHoveringAddSpriteButton) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 100, 200, 100, 80); // green - with transparency
            SDL_RenderFillRect(renderer, &AddSpriteButtonRect);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }
        else {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 150, 100, 200, 80); // purple - with transparency
            SDL_RenderFillRect(renderer, &AddSpriteButtonRect);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }

        // if Hovering show add sprite text next to the button. 
        if (isHoveringAddSpriteButton) {
            SDL_Color textColor = color_black;
            SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Add Sprite", textColor);
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

            int textX = AddSpriteButtonRect.x - textSurface->w - 10; // 10 pixels to the left of the button
            int textY = AddSpriteButtonRect.y + (AddSpriteButtonRect.h - textSurface->h) / 2; // vertically centered

            SDL_Rect textRect = {textX, textY, textSurface->w, textSurface->h};
            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);
         }

     }
     
/*
-------------------------------------------------
This function is for handling clicking on cat icon
true if clicked/ false otherwise
-------------------------------------------------
*/

bool HandleAddSpriteClick( const int mouseX, const int mouseY ) {
    // first check if mouse is on add sprite button
    if ( mouseX >= AddSpriteButtonRect.x && mouseX <= AddSpriteButtonRect.x + AddSpriteButtonRect.w &&
         mouseY >= AddSpriteButtonRect.y && mouseY <= AddSpriteButtonRect.y + AddSpriteButtonRect.h) {
            // clicked on add sprite button
            if ( addSpriteMenuState == Menu_Open ) {
                addSpriteMenuState = Menu_Closed;
            }
            else {
                addSpriteMenuState = Menu_Open;
            }
            return true; 
         }

    // if menu is open, check if mouse is on any of menu options
    if ( addSpriteMenuState == Menu_Open ) {
        for ( int i = 0; i < 4; i++ ) {
            if ( menuOptions[i].isHovered ) {
                // controling each option separately
                switch ( i ) {
                    case 0: // choose a sprite 
                    // calling the function for adding a new sprite .
                    break;
                    case 1: // paint 
                    // calling the function for opening paint page.
                    break;
                    case 2: // upload
                    // calling the function for uploading a new sprite from local files.
                    break;
                    case 3: // random
                    // calling the function for adding a random sprite.
                    break;
                }
                addSpriteMenuState = Menu_Closed; // close menu after clicking on an option / choosing one sprite option
                return true;
            }
        }
    }
    return false;
}

/*
-------------------------------------------------
this function is for drawing the expanded menu with
4 options when clicking on add sprite button
-------------------------------------------------
*/

void DrawAddSpriteMenuOptions(SDL_Renderer* renderer, TTF_Font* font, const int mouseX, const int mouseY ) {
    if ( addSpriteMenuState != Menu_Open ) {
        return; // if menu is not open, we dont draw menu options
    }

    const int optionSize = 60;
    const int spacing = 10;
    const int MenuX = AddSpriteButtonRect.x + ( AddSpriteButtonRect.w - optionSize ) / 2; // center align with add sprite button
    const int MenuStartY = AddSpriteButtonRect.y + AddSpriteButtonRect.h + spacing + 10;

    // defining menu options
    SDL_Texture* optionIcons[4] = { searchIcon, paintIcon, uploadIcon, randomIcon };

    // drawing each menu option
    for ( int i = 0; i < 4; i++ ) {

        // calculating position for each option 
        int optionY = MenuStartY + i * (optionSize + spacing);
        menuOptions[i].rect = { MenuX, optionY, optionSize, optionSize };

        // now cheching if hovering on this option
        menuOptions[i].isHovered = ( mouseX >= menuOptions[i].rect.x && mouseX <= menuOptions[i].rect.x + menuOptions[i].rect.w &&
                                    mouseY >= menuOptions[i].rect.y && mouseY <= menuOptions[i].rect.y + menuOptions[i].rect.h);
        
        // drawing option background
        if ( menuOptions[i].isHovered ) {
            SDL_SetRenderDrawColor(renderer, 100, 200, 100, 80); // green for hovered option
        }
        else {
            SDL_SetRenderDrawColor(renderer, 150, 100, 200, 80); // purple for normal option
        }
        SDL_RenderFillRect(renderer, &menuOptions[i].rect);  
        
        // drawing border for option
        SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255);
        SDL_RenderDrawRect(renderer, &menuOptions[i].rect);

        // drawing option icon
        if ( optionIcons[i] ) {
            SDL_Rect iconRect = { menuOptions[i].rect.x + 6,
                                  menuOptions[i].rect.y + 6,
                                  menuOptions[i].rect.w - 11,
                                  menuOptions[i].rect.h - 11 };
        SDL_RenderCopy(renderer, optionIcons[i], nullptr, &iconRect);
        }

        // if hovering show option name next to the option
    
  }
}


/*
-------------------------------------------------
This function is for adding and initializeing a sprint to screen
this function will run only one time.
-------------------------------------------------

it will return id of new sprint
*/
int addSprintToScreen(SDL_Renderer *renderer, std::string name, const char *imgPath)
{
    SprintBody newSprint;

    newSprint.name = name;

    // spon on the center of screen
    newSprint.posX = CANVAS_SCREEN_WIDTH / 2;
    newSprint.posY = CANVAS_SCREEN_HEIGTH / 2;

    newSprint.angleRotation = 0; // render at it's original oriantation.

    int id = 0;
    for (const SprintBody &s : aliveSprints) // founding new latest ID
    {
        if (id <= s.id)
        {
            id = s.id + 1;
        }
    }
    newSprint.id = id;

    // load texture of it's custome.
    newSprint.nowCustome = renderImage(renderer, imgPath);
    if (!newSprint.nowCustome)
    {
        std::cerr << "Faild to load Custome image of \"" << name << "\": " << IMG_GetError() << std::endl;
        return SDL_IMAGE_LOAD_ERROR;
    }

    aliveSprints.push_back(newSprint);

    return newSprint.id;
}

/*
-------------------------------------------------
This function is for drawing Sprite box.
this function will run on every frame.
-------------------------------------------------
*/

void drawSpriteBoxScreen(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY)
{
    // Draw backgound color
    SDL_SetRenderDrawColor(renderer, color_spriteBoxBackground);
    SDL_RenderFillRect(renderer, &SPRITE_BOX);
    // Draw border of Canvas
    SDL_SetRenderDrawColor(renderer, color_black);
    SDL_RenderDrawRect(renderer, &SPRITE_BOX);

    // Top bar of Sprite box 
    SDL_Rect topBar = { SPRITE_BOX.x, SPRITE_BOX.y, SPRITE_BOX.w, 100 };
    SDL_SetRenderDrawColor(renderer, 211, 211, 211, 255);
    SDL_RenderFillRect(renderer, &topBar);
    SDL_SetRenderDrawColor(renderer, color_black);
    SDL_RenderDrawRect(renderer, &topBar);

    // Sprite Area
    SDL_Rect spriteArea = { SPRITE_BOX.x, topBar.y + topBar.h, SPRITE_BOX.w, SPRITE_BOX.h - topBar.h };
    SDL_SetRenderDrawColor(renderer, 230, 230, 230, 255);
    SDL_RenderFillRect(renderer, &spriteArea);
    SDL_SetRenderDrawColor(renderer, color_black);
    SDL_RenderDrawRect(renderer, &spriteArea);

    // Draw top bar of sprite box
    DrawTopBarOfSpriteBox(renderer, font, topBar);

    // Draw add sprite button and menu
    DrawAddSpriteMenu(renderer, font, spriteArea, mouseX, mouseY);

    // Sprtie thumbnails
    const int thumbnailSize = 70;
    const int spacing = 10;
    int StartX = spriteArea.x + spacing; 
    int StartY = spriteArea.y + spacing;

    for ( int i = 0; i < aliveSprints.size(); i++ )
    {
        SprintBody& sprite = aliveSprints[i];

        // position of thumbnail
        int thumbnailX = StartX + i * (thumbnailSize + spacing);
        int thumbnailY = StartY;

        // Draw thumbnail
        SDL_Rect thumbRect = {thumbnailX , thumbnailY, thumbnailSize, thumbnailSize};

        // if hovering 
        bool ishovered = ( mouseX >= thumbRect.x && mouseX <= thumbRect.x + thumbRect.w &&
                           mouseY >= thumbRect.y && mouseY <= thumbRect.y + thumbRect.h);

        // if selected
        bool isSelected = ( SelectedSpriteID == sprite.id );                   
        
        // background color 
        SDL_SetRenderDrawColor( renderer , 230, 230, 230, 255);
        SDL_RenderFillRect(renderer, &thumbRect);                     

        // image of thumbnail
        SDL_RenderCopy (renderer, sprite.nowCustome, nullptr, &thumbRect);

        // hovering effect
        if ( isSelected ) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0 , 255); // color = red for selected sprite

            // Drawin a thick border for selected sprite
            SDL_Rect border1 = thumbRect;
            SDL_RenderDrawRect(renderer, &border1);
            SDL_Rect border2 = { thumbRect.x - 1, thumbRect.y - 1, thumbRect.w + 2, thumbRect.h + 2 };
            SDL_RenderDrawRect(renderer, &border2);
            SDL_Rect border3 = { thumbRect.x - 2, thumbRect.y - 2, thumbRect.w + 4, thumbRect.h + 4 };
            SDL_RenderDrawRect(renderer, &border3);
            SDL_Rect border4 = { thumbRect.x - 3, thumbRect.y - 3, thumbRect.w + 6, thumbRect.h + 6 };
            SDL_RenderDrawRect(renderer, &border4);
            SDL_Rect border5 = { thumbRect.x - 4, thumbRect.y - 4, thumbRect.w + 8, thumbRect.h + 8 };
            SDL_RenderDrawRect(renderer, &border5); 
        }
        else if (ishovered) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 100, 150, 255, 80); // blue with some transparency
            SDL_RenderFillRect(renderer, &thumbRect);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }

        // Draw border
        SDL_SetRenderDrawColor(renderer, color_black);
        SDL_RenderDrawRect(renderer, &thumbRect);

        // hovering border & selected border
        if (ishovered) {
            SDL_SetRenderDrawColor(renderer, 100, 150, 255, 255); // blue = for hovered sprite
            SDL_RenderDrawRect(renderer, &thumbRect);
        } 
        else {
            SDL_SetRenderDrawColor(renderer, color_black);
            SDL_RenderDrawRect(renderer, &thumbRect);
        }

        // drawing sprite name 
        SDL_Color textColor = color_black;
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, sprite.name.c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        // position of text
        int textX = thumbnailX + thumbnailSize / 2 - textSurface->w / 2;
        int textY = thumbnailY + thumbnailSize + 5; // 5 pixels below the thumbnail
        
        SDL_Rect textRect = {textX, textY, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);

    }
}
/*
----------------------------------------------------
this function is for loading all UI icons for sprite box menu
true if loade, false otherwise
----------------------------------------------------
*/

bool loadSpriteBoxMenuIcons(SDL_Renderer* renderer) {
    catIcon = renderImage(renderer, "assets/icons/SpriteBox/addSpriteIcon.png");
    paintIcon = renderImage(renderer, "assets/icons/SpriteBox/addSpritePaintIcon.png");
    uploadIcon = renderImage(renderer, "assets/icons/SpriteBox/addSpriteUploadIcon.png");
    randomIcon = renderImage(renderer, "assets/icons/SpriteBox/addSpriteRandomIcon.png");
    searchIcon = renderImage(renderer, "assets/icons/SpriteBox/addSpriteChooseIcon.png");

    if (!catIcon || !paintIcon || !uploadIcon || !randomIcon || !searchIcon) {
        std::cerr << "Failed to load one or more sprite box menu icons." << std::endl;
        return false;
    }
    return true;
}

/*
----------------------------------------------------
This function is for initializing Sprint Screen
----------------------------------------------------
*/
int sprintBoxInit(SDL_Renderer *renderer, TTF_Font *font)
{
    // Add default Sprint
    int errcode = addSprintToScreen(renderer, "Mad Man", "assets/images/sprints/madman.png");
    if (errcode)
        return errcode;

    // Adding menu bar icons
    if (!loadSpriteBoxMenuIcons(renderer)) {
        std::cerr << "Failed to load sprite box menu icons." << std::endl;
    }    

    return 0;
}

/*
----------------------------------------------------
This function is for handling selection on sprite thumbnails 
----------------------------------------------------
*/
void SpriteBoxClick(const int mouseX , const int mouseY ) 
{
    int thumbnailSize = 70;
    int spacing = 10;

    SDL_Rect spriteArea = { SPRITE_BOX.x, SPRITE_BOX.y + 100, SPRITE_BOX.w, SPRITE_BOX.h - 100 };

    int startX = spriteArea.x + spacing;
    int startY = spriteArea.y + spacing;

    // checking each thumbnail 
    for ( int i = 0; i < aliveSprints.size(); i++ ){

        int thumbnailX = startX + i * (thumbnailSize + spacing);
        int thumbnailY = startY;

        SDL_Rect thumbRect = {thumbnailX , thumbnailY, thumbnailSize, thumbnailSize};

        // if clicked on this thumbnail 
        if ( mouseX >= thumbRect.x && mouseX <= thumbRect.x + thumbRect.w &&
             mouseY >= thumbRect.y && mouseY <= thumbRect.y + thumbRect.h) 
        {
            SelectedSpriteID = aliveSprints[i].id; // set selected sprite id
            break; 
        }
    } 
}
