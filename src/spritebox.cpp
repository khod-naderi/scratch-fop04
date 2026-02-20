/*
-------------------------------------------
This header file is for managing custome and backgorund setting area named sprite box
-------------------------------------------
*/

/*
----------------------------------------------
most of functions are called in the function : dawSpriteBoxScreen,
which is called in main loop to render sprite box every frame.
----------------------------------------------
*/
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "spritebox.h"
#include "color.h"
#include "ui.h"
#include "generaldef.h"
#include <iostream>
#include "canvas.h"
#include <cstdlib> 
#include <ctime> 

// selected sprite id in sprite box
int SelectedSpriteID = -1;

// state of the screen
ScreenState currentState = State_MainEditor;

// managing random option
static bool randomSeeded = false;
static bool requestRandomSprite = false;

// these are for adding png icons for add sprite menu
SDL_Texture* catIcon = nullptr;
SDL_Texture* paintIcon = nullptr;
SDL_Texture* uploadIcon = nullptr;
SDL_Texture* randomIcon = nullptr;
SDL_Texture* searchIcon = nullptr;
SDL_Texture* TrashIcon = nullptr;

// variables for handling text box of top bar 
TextBox* xTextBox = nullptr;
TextBox* yTextBox = nullptr;

// these global variables are for handling add sprite button and menu
AddSpriteMenuState addSpriteMenuState = Menu_Closed;
SDL_Rect AddSpriteButtonRect ;
SDL_Rect MenuToolBarRect;
MenuOption menuOptions[4] = {
    {nullptr, "Choose a Sprite", {0, 0, 0, 0}, false},
    {nullptr, "Paint", {0, 0, 0, 0}, false},
    {nullptr, "Upload", {0, 0, 0, 0}, false},
    {nullptr, "Random", {0, 0, 0, 0}, false}
};
bool isHoveringAddSpriteButton = false;

/*
-------------------------------------------------
these variables are for picker screen
-------------------------------------------------
*/
availableSprite availableSprites[AVAILABLE_SPRITES_COUNT] = {
    {"Block", "assets/icons/Sprites/SpriteBlock.jpg", nullptr},
    {"Dog", "assets/icons/Sprites/SpriteDog.jpg", nullptr},
    {"Enemy", "assets/icons/Sprites/SpriteEnemy.jpg", nullptr},
    {"Flag1", "assets/icons/Sprites/SpriteFlag1.jpg", nullptr},
    {"Flag2", "assets/icons/Sprites/SpriteFlag2.jpg", nullptr},
    {"Pad", "assets/icons/Sprites/SpritePad.jpg", nullptr},
    {"Robot", "assets/icons/Sprites/SpriteRobot.png", nullptr},
    {"Setting", "assets/icons/Sprites/SpriteSetting.jpg", nullptr},
    {"Star", "assets/icons/Sprites/SpriteStar.jpg", nullptr}
};
/*
-------------------------------------------------
This function is for adding and initializeing top bar of sprite box
this function will run on every frame.
-------------------------------------------------
*/

void DrawTopBarOfSpriteBox(SDL_Renderer *renderer, TTF_Font *font, SDL_Rect topBar) {

    if ( !renderer || !font ){
        return;
    }

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

    // writing name of selected sprite in sprite rectangle on top bar.
    std::string selectedName = "";

    for ( const SprintBody & s : aliveSprints ) {
        if ( s.id == SelectedSpriteID ) {
            selectedName = s.name ;
            break;
        }
    }
        if ( !selectedName.empty() ) {
            SDL_Surface* nameSurface = TTF_RenderText_Blended(font, selectedName.c_str() , textColor);
            if ( nameSurface ) {
                 SDL_Texture* nameTexture = SDL_CreateTextureFromSurface(renderer, nameSurface);
                 
                 if ( nameTexture ) {
                    SDL_Rect nameTextRect = {
                        nameBox.x + 6,
                        nameBox.y + ( nameBox.h - nameSurface->h )/ 2,
                        nameSurface->w,
                        nameSurface->h
                    };

                    SDL_RenderCopy(renderer, nameTexture , nullptr , &nameTextRect );
                    SDL_DestroyTexture ( nameTexture );
                 }
                 SDL_FreeSurface(nameSurface);
        }
    }


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

    // position of text box for X position
    if ( SelectedSpriteID != -1 && !xTextBox->isFucused ){
        for ( const SprintBody& s : aliveSprints ){
            if ( s.id == SelectedSpriteID ) {
                xTextBox->setStr(std::to_string(s.posX));
                break;
            }
        } 
    }
    
    if ( SelectedSpriteID != -1 && xTextBox->isFucused ) {
        std::string xStr = xTextBox->getStr();
        if ( !xStr.empty() ) {
            int xValue = std::stoi(xStr);
            for ( SprintBody& s : aliveSprints ) {
                if ( s.id == SelectedSpriteID ) {
                    s.posX = xValue;
                    break;
                }
            }
        }
    }
    xTextBox->posX = xBox.x + 5;
    xTextBox->posY = xBox.y + 5;
    xTextBox->draw(renderer);

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

    // creating text box for Y position
    if ( SelectedSpriteID != -1 && !yTextBox->isFucused ){
        for ( const SprintBody& s : aliveSprints ){
            if ( s.id == SelectedSpriteID ) {
                yTextBox->setStr(std::to_string(s.posY));
                break;
            }
        } 
    }

    yTextBox->posX = yBox.x + 5;
    yTextBox->posY = yBox.y + 5;
    yTextBox->draw(renderer);

    if ( SelectedSpriteID != -1 && yTextBox->isFucused ) {
        std::string yStr = yTextBox->getStr();
        if ( !yStr.empty() ) {
            int yValue = std::stoi(yStr);
            for ( SprintBody& s : aliveSprints ) {
                if ( s.id == SelectedSpriteID ) {
                    s.posY = yValue;
                    break;
                }
            }
        }
    }

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
    SDL_FreeSurface(showLabelSurface);
    SDL_FreeSurface(sizeLabelSurface);
    SDL_FreeSurface(rotationLabelSurface);

    SDL_DestroyTexture(textTexture);
    SDL_DestroyTexture(xLabelTexture);
    SDL_DestroyTexture(yLabelTexture);
    SDL_DestroyTexture(showLabelTexture);
    SDL_DestroyTexture(sizeLabelTexture);
    SDL_DestroyTexture(rotationLabelTexture);
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
                    currentState = State_Sprite_Picker; 
                    break;
                    case 1: // paint 
                    // calling the function for opening paint page.
                    break;
                    case 2: // upload
                    // calling the function for uploading a new sprite from local files.
                    break;
                    case 3: // random
                    requestRandomSprite = true; // this case is handled in the void function drawSpriteBoxScreen.
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
    const int spacing = 2;
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
        if ( menuOptions[i].isHovered ) {
            SDL_Color textColor = color_black;
            SDL_Surface* textSurface = TTF_RenderText_Solid(font, menuOptions[i].name.c_str(), textColor);
            if ( textSurface ) {
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                int textX = menuOptions[i].rect.x - textSurface->w - 10;
                int textY = menuOptions[i].rect.y + (menuOptions[i].rect.h - textSurface->h) / 2; 
                SDL_Rect textRect = {textX, textY, textSurface->w, textSurface->h};
                SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
                SDL_FreeSurface(textSurface);
                SDL_DestroyTexture(textTexture);
            }
        }
    
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
    // handling random option of cat icon
    if ( requestRandomSprite ) {
        int index = std::rand() % AVAILABLE_SPRITES_COUNT;
        SelectedSpriteID = addSprintToScreen( renderer, availableSprites[index].name, availableSprites[index].imagePath.c_str());
        requestRandomSprite = false;
    }
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
    DrawAddSpriteMenuOptions(renderer, font, mouseX, mouseY);

    // Sprtie thumbnails
    const int thumbnailSize = 70;
    const int spacing = 10;
    const int MAX_SPRITES_PER_ROW = 4;
    int StartX = spriteArea.x + spacing; 
    int StartY = spriteArea.y + spacing;

    for ( int i = 0; i < aliveSprints.size(); i++ )
    {
        SprintBody& sprite = aliveSprints[i];

        // row and column for sprite thumbnails
        int row = i / MAX_SPRITES_PER_ROW;
        int col = i % MAX_SPRITES_PER_ROW;

        // position of thumbnail
        int thumbnailX = StartX + col * (thumbnailSize + spacing);
        int thumbnailY = StartY + row * ( thumbnailSize + spacing + 20 ); // 20 pixels for text ( name ) of the thumbnail

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

        // draw trash icon on top right corner of thumbnail when selected
        if ( isSelected && TrashIcon ) {
            const int TRASH_SIZE = 24;

            int trashX = thumbRect.x + thumbRect.w - TRASH_SIZE - 4; 
            int trashY = thumbRect.y + 4;

            SDL_Rect trashRect = { trashX, trashY, TRASH_SIZE, TRASH_SIZE };
            
            SDL_SetRenderDrawColor(renderer, color_white );
            SDL_Rect trashBackGround = { trashRect.x - 2, trashRect.y - 2, trashRect.w + 4, trashRect.h + 4 };
            SDL_RenderFillRect(renderer, &trashBackGround);
            SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
            SDL_RenderDrawRect(renderer, &trashBackGround);
            SDL_RenderCopy(renderer, TrashIcon, nullptr, &trashRect);

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
    TrashIcon = renderImage(renderer, "assets/icons/SpriteBox/TrashIcon.png");

    if (!catIcon || !paintIcon || !uploadIcon || !randomIcon || !searchIcon || !TrashIcon) {
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

    // Creat text box for X position in top bar
    xTextBox = new TextBox(font);
    xTextBox->setStr("0");

    // Create text box for Y position in top bar 
    yTextBox = new TextBox(font);
    yTextBox->setStr("0");

    return 0;
}

/*
----------------------------------------------------
This function is for handling selection on sprite thumbnails 
----------------------------------------------------
*/
void SpriteBoxClick(const int mouseX , const int mouseY ) 
{
    if (HandleAddSpriteClick(mouseX, mouseY)) {
        return;
    }

    int thumbnailSize = 70;
    int spacing = 10;
    const int MAX_SPRITES_PER_ROW = 4;

    SDL_Rect spriteArea = { SPRITE_BOX.x, SPRITE_BOX.y + 100, SPRITE_BOX.w, SPRITE_BOX.h - 100 };

    int startX = spriteArea.x + spacing;
    int startY = spriteArea.y + spacing;

    // checking each thumbnail 
    for ( int i = 0; i < aliveSprints.size(); i++ ){

        int row = i / MAX_SPRITES_PER_ROW;
        int col = i % MAX_SPRITES_PER_ROW;

        int thumbnailX = startX + col * (thumbnailSize + spacing);
        int thumbnailY = startY + row * ( thumbnailSize + spacing + 20 ); // 20 pixels for text ( name ) of the thumbnail

        SDL_Rect thumbRect = {thumbnailX , thumbnailY, thumbnailSize, thumbnailSize};

        // if clicked on this thumbnail 
        if ( mouseX >= thumbRect.x && mouseX <= thumbRect.x + thumbRect.w &&
             mouseY >= thumbRect.y && mouseY <= thumbRect.y + thumbRect.h) 
        {
            if ( SelectedSpriteID == aliveSprints[i].id ) {

                const int TRASH_SIZE = 24;
                int trashX = thumbRect.x + thumbRect.w - TRASH_SIZE - 4;
                int trashY = thumbRect.y + 4;

                if ( mouseX >= trashX && mouseX <= trashX + TRASH_SIZE &&
                     mouseY >= trashY && mouseY <= trashY + TRASH_SIZE) {
                        // clicked on trash icon, delete this sprite
                        aliveSprints.erase(aliveSprints.begin() + i);
                        SelectedSpriteID = -1; // reset selected sprite id 
                        return;
                }
            }
            SelectedSpriteID = aliveSprints[i].id; // set selected sprite id
            break; 
        }
    } 
}

/*
----------------------------------------------------
This function is for drawing sprite picker screen when clicking on choose a sprite option in add sprite menu
----------------------------------------------------
*/

void DrawSpritePickerScreen(SDL_Renderer *renderer, TTF_Font *font, const int mouseX, const int mouseY ) {

    // 1. drawing the screen text at top 
    const char* title = "Choose a Sprite";
    SDL_Color titleColor = color_black;
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, title, titleColor);
    if ( titleSurface ) {
        SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    int titleX = CATEGORY_COLUMN_WIDTH + MAIN_WINDOW_WIDTH / 2 - titleSurface->w / 2;
    int titleY = 55;
    SDL_Rect titleRect = { titleX, titleY, titleSurface->w, titleSurface->h };
    SDL_RenderCopy(renderer, titleTexture, nullptr, &titleRect);
    SDL_DestroyTexture(titleTexture);
    }
    SDL_FreeSurface(titleSurface);

    // 2. drawing back button at top left corner
    SDL_Rect backButtonRect = { CATEGORY_COLUMN_WIDTH + BLOCKS_COLUMN_WIDTH + 100 , 55, 80, 30 };
    SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
    SDL_RenderFillRect(renderer, &backButtonRect);
    SDL_SetRenderDrawColor(renderer, color_black);
    SDL_RenderDrawRect(renderer, &backButtonRect);

    // 3. hover effect for back button
    bool isHoveringBackButton = ( mouseX >= backButtonRect.x && mouseX <= backButtonRect.x + backButtonRect.w &&
                                 mouseY >= backButtonRect.y && mouseY <= backButtonRect.y + backButtonRect.h);
    if ( isHoveringBackButton ) {
        SDL_SetRenderDrawColor(renderer, 201, 201, 201, 255);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
    }
    SDL_RenderFillRect(renderer, &backButtonRect);

    // 4. writing back button text
    SDL_Color textColor = color_black;
    SDL_Surface* backTextSurface = TTF_RenderText_Solid(font, "< back", textColor);
    if ( backTextSurface ) {
        SDL_Texture* backTextTexture = SDL_CreateTextureFromSurface(renderer, backTextSurface);
        int textX = backButtonRect.x + 15; 
        int textY = backButtonRect.y + (backButtonRect.h - backTextSurface->h) / 2; 
        SDL_Rect textRect = {textX, textY, backTextSurface->w, backTextSurface->h};
        SDL_RenderCopy(renderer, backTextTexture, nullptr, &textRect);
        SDL_DestroyTexture(backTextTexture);
    }
    SDL_FreeSurface(backTextSurface);

    // 5. if back button is clicked, return to main editor screen


    // 6. drawing separate line between sprite picker and other parts of screen
    SDL_SetRenderDrawColor(renderer, color_black);
    int SepratorY = backButtonRect.y + backButtonRect.h + 20;
    SDL_RenderDrawLine(renderer, CATEGORY_COLUMN_WIDTH + BLOCKS_COLUMN_WIDTH + 50, SepratorY, MAIN_WINDOW_WIDTH, SepratorY);

    // 7. drawing sprite options
    const int SPRITE_THUMBNAIL_SIZE = 80;
    const int spacing = 50;
    const int SPRITES_PER_ROW = 9;
    int GridStartX = CATEGORY_COLUMN_WIDTH + BLOCKS_COLUMN_WIDTH + 150;
    int GridStartY = SepratorY + 30;

    // loading textures
    static bool TexturesLoaded = false;
    
    if ( !TexturesLoaded ) {
        // loading textures for sprite options
        for ( int i = 0; i < AVAILABLE_SPRITES_COUNT; i++ ) {
            availableSprites[i].texture = renderImage(renderer, availableSprites[i].imagePath.c_str());
            if ( !availableSprites[i].texture ) {
                std::cerr << "Failed to load texture for sprite: " << availableSprites[i].name << std::endl;
            }
        }

        TexturesLoaded = true;
    }

        for ( int i = 0; i < AVAILABLE_SPRITES_COUNT; i++ ) {
            int row = i / SPRITES_PER_ROW;
            int col = i % SPRITES_PER_ROW;

            int SpriteX = GridStartX + col * (SPRITE_THUMBNAIL_SIZE + spacing);
            int SpriteY = GridStartY + row * (SPRITE_THUMBNAIL_SIZE + spacing);

            SDL_Rect thumbRect = { SpriteX, SpriteY, SPRITE_THUMBNAIL_SIZE, SPRITE_THUMBNAIL_SIZE };

            // check hover 
            bool isSpriteHovered = ( mouseX >= thumbRect.x && mouseX <= thumbRect.x + thumbRect.w &&
                                  mouseY >= thumbRect.y && mouseY <= thumbRect.y + thumbRect.h);                 

            // 1. draw background for hovered sprite
            if ( isSpriteHovered ) {
                SDL_SetRenderDrawColor ( renderer, 100, 150, 255, 80); // blue with some transparency
            }
            else {
                SDL_SetRenderDrawColor(renderer, 230, 230, 230, 255); // normal background color
            }
            SDL_RenderFillRect(renderer, &thumbRect);

            // 2. draw sprite image
            if ( availableSprites[i].texture ) {
                SDL_RenderCopy(renderer, availableSprites[i].texture, nullptr, &thumbRect);
            }   
            
            // 3. hover effect layer
            if ( isSpriteHovered ) {
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer, 100, 150, 255, 80); // blue with some transparency
                SDL_RenderFillRect(renderer, &thumbRect);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            }

            // 4. draw border
            if ( isSpriteHovered ) {
                SDL_SetRenderDrawColor(renderer, 100, 150, 255, 255); // blue for hovered sprite
            }
            else {
                SDL_SetRenderDrawColor(renderer, color_black); 
            }
            SDL_RenderDrawRect(renderer, &thumbRect);

            // Draw Sprite name below
            SDL_Color nameColor = color_black;
            SDL_Surface* nameSurface = TTF_RenderText_Solid(font, availableSprites[i].name.c_str(), nameColor);
            if ( nameSurface ) {
                SDL_Texture* nameTexture = SDL_CreateTextureFromSurface(renderer, nameSurface);
                int nameX = SpriteX + SPRITE_THUMBNAIL_SIZE / 2 - nameSurface->w / 2;
                int nameY = SpriteY + SPRITE_THUMBNAIL_SIZE + 5;
                SDL_Rect nameRect = { nameX, nameY, nameSurface->w, nameSurface->h };
                SDL_RenderCopy(renderer, nameTexture, nullptr, &nameRect); 
                SDL_FreeSurface(nameSurface);
                SDL_DestroyTexture(nameTexture);
            }
        }
    

}

/*
----------------------------------------------------
This function is for handling clicks on sprite picker screen
if clicked on back button, return to main editor screen
if clicked on a sprite option, add that sprite to screen and return to main editor screen
----------------------------------------------------
*/

bool HandleSpritePickerClick(SDL_Renderer* renderer, const int mouseX, const int mouseY ) {
    const int Left_Bar_Width = CATEGORY_COLUMN_WIDTH + BLOCKS_COLUMN_WIDTH + 150;
    SDL_Rect PickerArea = { Left_Bar_Width, 0, MAIN_WINDOW_WIDTH - Left_Bar_Width, MAIN_WINDOW_HEIGHT };

    // 1. check if clicked on back button
    SDL_Rect backButtonRect = { CATEGORY_COLUMN_WIDTH + BLOCKS_COLUMN_WIDTH + 100 , 55, 80, 30 };
    if ( mouseX >= backButtonRect.x && mouseX <= backButtonRect.x + backButtonRect.w &&
         mouseY >= backButtonRect.y && mouseY <= backButtonRect.y + backButtonRect.h) {
            currentState = State_MainEditor; // return to main editor screen
            return true;    
        }
    
    // 2. check if clicked on any sprite option
    const int SPRITE_THUMBNAIL_SIZE = 80;
    const int spacing = 50;
    const int SPRITES_PER_ROW = 9;
    int separatorY = backButtonRect.y + backButtonRect.h + 20;
    int GridStartX = CATEGORY_COLUMN_WIDTH + BLOCKS_COLUMN_WIDTH + 150;
    int GridStartY = separatorY + 30;
    
    for ( int i = 0; i < AVAILABLE_SPRITES_COUNT; i++ ) {
        int row = i / SPRITES_PER_ROW;
        int col = i % SPRITES_PER_ROW;

        int SpriteX = GridStartX + col * (SPRITE_THUMBNAIL_SIZE + spacing);
        int SpriteY = GridStartY + row * (SPRITE_THUMBNAIL_SIZE + spacing);

        SDL_Rect thumbRect = { SpriteX , SpriteY , SPRITE_THUMBNAIL_SIZE, SPRITE_THUMBNAIL_SIZE };

        if ( mouseX >= thumbRect.x && mouseX <= thumbRect.x + thumbRect.w &&
             mouseY >= thumbRect.y && mouseY <= thumbRect.y + thumbRect.h) 
        {
            int newID = addSprintToScreen(renderer, availableSprites[i].name, availableSprites[i].imagePath.c_str());
            if ( newID >= 0 ) {
                SelectedSpriteID = newID; // set the newly added sprite as selected
                currentState = State_MainEditor; // return to main editor screen
                return true;
            }
            else {
                std::cerr << "Failed to add sprite to screen." << std::endl;
                return false;
            }
        }
    }

    return false;
}

