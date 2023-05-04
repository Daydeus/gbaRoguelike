#include <stdlib.h>
#include <string.h>
#include "../libtonc/include/tonc.h"
#include "constants.h"
#include "debug.h"
#include "entity.h"
#include "fieldOfVision.h"
#include "globals.h"
#include "mapGeneration.h"
#include "mgba.h"
#include "pauseMenu.h"
#include "playerSprite.h"
#include "tile.h"

//------------------------------------------------------------------
// Data Structures
//------------------------------------------------------------------
struct Tile gameMap[MAP_HEIGHT_TILES][MAP_WIDTH_TILES];
struct Entity entity[NUM_MAX_ENTITIES];

//------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------
OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;
unsigned int frameCount = 1;
unsigned int randomSeed = 0;
enum state gameState = STATE_TITLE_SCREEN;
int turnOfEntityIndex = 0;
uint8_t playerSightId = TILE_IN_SIGHT;
boolean debugCollisionIsOff = FALSE, debugMapIsVisible = FALSE;
u32 blendingValue = 0x20;
int8_t playerMoveOffsetX = 0, playerMoveOffsetY = 0;
int16_t screenOffsetX = 0, screenOffsetY = 0;

//------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------

static void drawHUD();
static void updateGraphics();
static void loadPlayerSprite(uint16_t const playerScreenX, uint16_t const playerScreenY);

//------------------------------------------------------------------
// Function: drawHUD
// 
// Draw placeholder HUD for player in STATE_GAMEPLAY
//------------------------------------------------------------------
void drawHUD()
{
    int screenEntryTL = 0;                     // screenEntryTopLeft
    int screenEntryTR = 0;                    // screenEntryTopRight
    int screenEntryBL = 0;                  // screenEntryBottomLeft
    int screenEntryBR = 0;                 // screenEntryBottomRight
    int *tileToDraw = NULL;

    for (int y = 0; y < SCREEN_HEIGHT_TILES; y++)
    {
        for (int x = 0; x <= 15; x++)
        {
            screenEntryTL = x * 2;
            screenEntryTR = screenEntryTL + 1;
            screenEntryBL = screenEntryTL + SCREEN_BLOCK_SIZE;
            screenEntryBR = screenEntryBL + 1;
            
            // Get and copy the 8x8 tile into map memory
            tileToDraw = (y == 0) ? (int*)TRANSPARENT : (int*)HEART_TL;
            memcpy(&se_mem[GAME_HUD_SB][screenEntryTL], &tileToDraw, 2);

            tileToDraw = (y == 0) ? (int*)TRANSPARENT : (int*)HEART_TR;
            memcpy(&se_mem[GAME_HUD_SB][screenEntryTR], &tileToDraw, 2);

            tileToDraw = (y == 0) ? (int*)TRANSPARENT : (int*)HEART_BL;
            memcpy(&se_mem[GAME_HUD_SB][screenEntryBL], &tileToDraw, 2);

            tileToDraw = (y == 0) ? (int*)TRANSPARENT : (int*)HEART_BR;
            memcpy(&se_mem[GAME_HUD_SB][screenEntryBR], &tileToDraw, 2);
        }
    }
}

//------------------------------------------------------------------
// Function: updateGraphics
// 
// Updates player sprite position and screen map offset based on
// player input
//------------------------------------------------------------------
void updateGraphics()
{
    struct Entity *player = getEntity(PLAYER_INDEX);
    uint16_t playerScreenX = 112, playerScreenY = 80;

    if (playerMoveOffsetX != 0)
        playerMoveOffsetX = approachValue(playerMoveOffsetX, 0, 1);
    else if (playerMoveOffsetY != 0)
        playerMoveOffsetY = approachValue(playerMoveOffsetY, 0, 1);

    // screenOffsets can loop around multiple times and can throw off calcs
    if (screenOffsetX >= SCREEN_WIDTH + TILE_SIZE)
        screenOffsetX -= (SCREEN_WIDTH + TILE_SIZE);
    else if (screenOffsetX < 0)
        screenOffsetX += (SCREEN_WIDTH + TILE_SIZE);

    if (screenOffsetY >= SCREEN_WIDTH + TILE_SIZE)
        screenOffsetY -= (SCREEN_WIDTH + TILE_SIZE);
    else if (screenOffsetY < 0)
        screenOffsetY += (SCREEN_WIDTH + TILE_SIZE);

    #ifdef DEBUG_ENTITY
        if (turnOfEntityIndex != PLAYER_INDEX)
        {
            mgba_printf(MGBA_LOG_DEBUG, "previous entity turn: %d", turnOfEntityIndex - 1);
            mgba_printf(MGBA_LOG_DEBUG, "current entity turn: %d", turnOfEntityIndex);
        }
    #endif

    if (turnOfEntityIndex != PLAYER_INDEX)
    {
        switch (player->lastAction)
        {
        case WALKED_LEFT:
        case WALKED_RIGHT:
        case WALKED_UP:
        case WALKED_DOWN:
            redrawGameMapEdge(player->lastAction);
        default:
            updateGameMapSight();
        }
        #ifdef PRINT_SIGHT_DRAW
            printTileSightInLog();
        #endif

    if (turnOfEntityIndex >= NUM_MAX_ENTITIES - 1)
        turnOfEntityIndex = 0;
    else
        turnOfEntityIndex++;
    }

    // Update background scrolling offsets
    REG_BG1HOFS = playerMoveOffsetX;
    REG_BG1VOFS = playerMoveOffsetY;
    REG_BG2HOFS = screenOffsetX + playerMoveOffsetX;
    REG_BG2VOFS = screenOffsetY + playerMoveOffsetY;

    //playerAction = PLAYER_NO_ACTION;

    loadPlayerSprite(playerScreenX, playerScreenY);
    REG_BLDCNT= BLD_BUILD(
                    BLD_BG1,        // Top layers
                    BLD_BG2,        // Bottom layers
                    1);             // Mode
}

//------------------------------------------------------------------
// Function: loadPlayerSprite
// 
// Loads the correct index of the player sprite based on frame
// count and on facing direction. TODO: break into smaller funcs.
//------------------------------------------------------------------
void loadPlayerSprite(uint16_t const playerScreenX, uint16_t const playerScreenY)
{
    struct Entity *player = getEntity(PLAYER_INDEX);
    unsigned int startingIndex = 0, paletteBank = 0;
    OBJ_ATTR *entity = &obj_buffer[0];

    obj_set_attr(entity,
        ATTR0_SQUARE,                     // Square, regular sprite,
        ATTR1_SIZE_16,                              // 16x16 pixels,
        ATTR2_PALBANK(paletteBank) | startingIndex); // palette index 0, tile index 0

    // Update sprite based on status
    switch (player->facing)
    {
    case DIR_LEFT:
        if (frameCount % 20 > 9)
            startingIndex = PLAYER_FACING_LEFT_FR1;
        else
            startingIndex = PLAYER_FACING_LEFT_FR2;
        break;
    case DIR_RIGHT:
        entity->attr1 ^= ATTR1_HFLIP;
        if (frameCount % 20 > 9)
            startingIndex = PLAYER_FACING_LEFT_FR1;
        else
            startingIndex = PLAYER_FACING_LEFT_FR2;
        break;
    case DIR_UP:
        if (frameCount % 20 > 9)
            startingIndex = PLAYER_FACING_UP_FR1;
        else
            startingIndex = PLAYER_FACING_UP_FR2;
        break;
    case DIR_DOWN:
        if (frameCount % 20 > 9)
            startingIndex = PLAYER_FACING_DOWN_FR1;
        else
            startingIndex = PLAYER_FACING_DOWN_FR2;
        break;
    default:
        if (frameCount % 20 > 9)
            startingIndex = PLAYER_FACING_LEFT_FR1;
        else
            startingIndex = PLAYER_FACING_LEFT_FR2;
        break;
    }
    // Finalize sprite changes
    entity->attr2 = ATTR2_BUILD(startingIndex, paletteBank, 0);
    obj_set_pos(entity, playerScreenX, playerScreenY);

    // Update first OAM object
    oam_copy(oam_mem, obj_buffer, 1);
}

//------------------------------------------------------------------
// Function: main
// 
// Entry point for the program
//------------------------------------------------------------------
int main(void)
{
    #ifdef DEBUG
        mgba_console_open();
    #endif

    irq_init(NULL);
    irq_enable(II_VBLANK);

    // Load tiles and palette of sprite into video and palete RAM
    memcpy32(&tile_mem[4][0], playerSpriteTiles, playerSpriteTilesLen / 4);
    memcpy32(pal_obj_mem, playerSpritePal, playerSpritePalLen / 4);

    oam_init(obj_buffer, 128);

    struct Entity *player = getEntity(PLAYER_INDEX);

    while (1)
    {
        // Get player input
        key_poll();

        // gameState program control
        switch(gameState)
        {
        case STATE_TITLE_SCREEN:
            if (__key_curr != 0)
            {
                randomSeed = frameCount;//2915;
                srand(randomSeed);

                #ifdef DEBUG
                    mgba_printf(MGBA_LOG_INFO, "RNG Seed: %d", randomSeed);
                #endif

                generateGameMap();
                initFOV();
                drawHUD();

                initEntities();

                #ifdef DEBUG
                    mgba_printf(MGBA_LOG_INFO, "player startingPosition: (%d, %d)", player->posX, player->posY);
                #endif
                #ifdef PRINT_MAP_DRAW
                    printMapInLog();
                #endif

                doStateTransition(STATE_GAMEPLAY);
            }
            break;
        case STATE_GAMEPLAY:
            if (playerMoveOffsetX == 0 && playerMoveOffsetY == 0)
                doPlayerInput();
            if (turnOfEntityIndex != PLAYER_INDEX)
            {
                doFOV(player->posX, player->posY, getEntitySightRange(player));
                REG_BLDALPHA= BLDA_BUILD(BG_0_BLEND_UP/8, blendingValue/8);
            }
            updateGraphics();
            
            // If player found stairs
            if (getTileTerrain(player->posX, player->posY) == ID_STAIRS)
                doStateTransition(STATE_TITLE_SCREEN);
            break;
        case STATE_MENU:
            REG_BG1HOFS = 0;
            REG_BG1VOFS = 0;
            if(doPauseMenuInput())
                drawPauseMenu();
            break;
        }

        // Low-power for rest of frame
        VBlankIntrWait();
        frameCount++;
    }
}
