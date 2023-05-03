#include <stdlib.h>
#include <string.h>
#include "../libtonc/include/tonc.h"
#include "constants.h"
#include "debug.h"
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

//------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------
OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;
unsigned int frameCount = 1;
unsigned int randomSeed = 0;
enum state gameState = STATE_TITLE_SCREEN;
int playerX = 1, playerY = 1;
uint8_t playerSightId = TILE_IN_SIGHT;
int sightRange = SIGHT_RANGE_STANDARD;
enum direction playerFacing = DIR_LEFT;
enum playerAction playerAction = PLAYER_NO_ACTION;
boolean debugCollisionIsOff = FALSE, debugMapIsVisible = FALSE;
u32 blendingValue = 0x40;
int8_t playerMoveOffsetX = 0, playerMoveOffsetY = 0;
int16_t screenOffsetX = 0, screenOffsetY = 0;

//------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------
static void doPlayerInput();
static void movePlayer(int8_t const direction);
static void drawHUD();
static void updateGraphics();
static void loadPlayerSprite(uint16_t const playerScreenX, uint16_t const playerScreenY);

//------------------------------------------------------------------
// Function: doPlayerInput
// 
// Updates the player character based on info read from key_poll()
// TODO: Create seperate function for out-of-bounds check and test
// at the same time as isSolid()
//------------------------------------------------------------------
void doPlayerInput()
{
    if ((KEY_EQ(key_hit, KI_LEFT) || KEY_EQ(key_held, KI_LEFT)) && !KEY_EQ(key_held, KI_R)) // Left Key
    {
        #ifdef DEBUG_PLAYER
            mgba_printf(MGBA_LOG_INFO, "pressed LEFT");
        #endif

        playerFacing = DIR_LEFT;
        if(isSolid(playerX + dirX[playerFacing], playerY + dirY[playerFacing]) ==  FALSE
        && isOutOfBounds(playerX + dirX[playerFacing], playerY + dirY[playerFacing]) == FALSE)
        {
            movePlayer(playerFacing);
            playerAction = PLAYER_WALKED_LEFT;
            playerSightId++;

            playerMoveOffsetX = dirX[DIR_RIGHT] * TILE_SIZE;
            screenOffsetX += dirX[DIR_LEFT] * TILE_SIZE;
        }
    }
    else if ((KEY_EQ(key_hit, KI_RIGHT) || KEY_EQ(key_held, KI_RIGHT)) && !KEY_EQ(key_held, KI_R)) // Right Key
    {
        #ifdef DEBUG_PLAYER
            mgba_printf(MGBA_LOG_INFO, "pressed RIGHT");
        #endif

        playerFacing = DIR_RIGHT;
        if(isSolid(playerX + dirX[playerFacing], playerY + dirY[playerFacing]) ==  FALSE
        && isOutOfBounds(playerX + dirX[playerFacing], playerY + dirY[playerFacing]) == FALSE)
        {
            movePlayer(playerFacing);
            playerAction = PLAYER_WALKED_RIGHT;
            playerSightId++;

            playerMoveOffsetX = dirX[DIR_LEFT] * TILE_SIZE;
            screenOffsetX += dirX[DIR_RIGHT] * TILE_SIZE;
        }
    }
    else if ((KEY_EQ(key_hit, KI_UP) || KEY_EQ(key_held, KI_UP)) && !KEY_EQ(key_held, KI_R)) // Up Key
    {
        #ifdef DEBUG_PLAYER
            mgba_printf(MGBA_LOG_INFO, "pressed UP");
        #endif

        playerFacing = DIR_UP;
        if(isSolid(playerX + dirX[playerFacing], playerY + dirY[playerFacing]) ==  FALSE
        && isOutOfBounds(playerX + dirX[playerFacing], playerY + dirY[playerFacing]) == FALSE)
        {
            movePlayer(playerFacing);
            playerAction = PLAYER_WALKED_UP;
            playerSightId++;

            playerMoveOffsetY = dirY[DIR_DOWN] * TILE_SIZE;
            screenOffsetY += dirY[DIR_UP] * TILE_SIZE;
        }
    }
    else if ((KEY_EQ(key_hit, KI_DOWN) || KEY_EQ(key_held, KI_DOWN)) && !KEY_EQ(key_held, KI_R)) // Down Key
    {
        #ifdef DEBUG_PLAYER
            mgba_printf(MGBA_LOG_INFO, "pressed DOWN");
        #endif

        playerFacing = DIR_DOWN;
        if(isSolid(playerX + dirX[playerFacing], playerY + dirY[playerFacing]) ==  FALSE
        && isOutOfBounds(playerX + dirX[playerFacing], playerY + dirY[playerFacing]) == FALSE)
        {
            movePlayer(playerFacing);
            playerAction = PLAYER_WALKED_DOWN;
            playerSightId++;

            playerMoveOffsetY = dirY[DIR_UP] * TILE_SIZE;
            screenOffsetY += dirY[DIR_DOWN] * TILE_SIZE;
        }
    }

    if (KEY_EQ(key_hit, KI_A))
    {
        uint8_t terrainToSet = getTileTerrain(playerX + dirX[playerFacing], playerY + dirY[playerFacing]) != ID_WALL ? ID_WALL : ID_FLOOR_BIG;
        if (getTileTerrain(playerX + dirX[playerFacing], playerY + dirY[playerFacing]) != ID_STAIRS)
            setTileTerrain(playerX + dirX[playerFacing], playerY + dirY[playerFacing], terrainToSet);
        playerAction = PLAYER_EARTH_BEND;
        playerSightId++;

        #ifdef DEBUG_PLAYER
            mgba_printf(MGBA_LOG_INFO, "pressed A");
        #endif
    }

    if (KEY_EQ(key_held, KI_R))
    {
        if (KEY_EQ(key_hit, KI_LEFT))
            playerFacing = DIR_LEFT;
        else if (KEY_EQ(key_hit, KI_RIGHT))
            playerFacing = DIR_RIGHT;
        else if (KEY_EQ(key_hit, KI_UP))
            playerFacing = DIR_UP;
        else if (KEY_EQ(key_hit, KI_DOWN))
            playerFacing = DIR_DOWN;
    }

    if (KEY_EQ(key_hit, KI_SELECT))
    {
        #ifdef DEBUG_PLAYER
            mgba_printf(MGBA_LOG_INFO, "pressed SELECT");
        #endif
        doStateTransition(STATE_TITLE_SCREEN);
    }
    if (KEY_EQ(key_hit, KI_START))
    {
        #ifdef DEBUG_PLAYER
            mgba_printf(MGBA_LOG_INFO, "pressed START");
        #endif
        doStateTransition(STATE_MENU);
    }
}

//------------------------------------------------------------------
// Function: movePlayer
// 
// Move the player in the given direction
//------------------------------------------------------------------
void movePlayer(int8_t const direction)
{
    playerX += dirX[direction];
    playerY += dirY[direction];

    #ifdef DEBUG_PLAYER
        mgba_printf(MGBA_LOG_INFO, "Player moved to: (%d, %d)", playerX, playerY);
    #endif

    // If player found stairs
    if (getTileTerrain(playerX, playerY) == ID_STAIRS)
        doStateTransition(STATE_TITLE_SCREEN);
}

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
    

    if (playerAction != PLAYER_NO_ACTION)
    {
        switch (playerAction)
        {
        case PLAYER_WALKED_LEFT:
        case PLAYER_WALKED_RIGHT:
        case PLAYER_WALKED_UP:
        case PLAYER_WALKED_DOWN:
            redrawGameMapEdge(playerAction);
        default:
            updateGameMapSight();
        }
        #ifdef PRINT_SIGHT_DRAW
            printTileSightInLog();
        #endif
        #ifdef PRINT_MAP_DRAW
            printMapInLog();
        #endif
    }

    // Update background scrolling offsets
    REG_BG1HOFS = playerMoveOffsetX;
    REG_BG1VOFS = playerMoveOffsetY;
    REG_BG2HOFS = screenOffsetX + playerMoveOffsetX;
    REG_BG2VOFS = screenOffsetY + playerMoveOffsetY;

    playerAction = PLAYER_NO_ACTION;
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
    unsigned int startingIndex = 0, paletteBank = 0;
    OBJ_ATTR *player = &obj_buffer[0];

    obj_set_attr(player,
        ATTR0_SQUARE,                     // Square, regular sprite,
        ATTR1_SIZE_16,                              // 16x16 pixels,
        ATTR2_PALBANK(paletteBank) | startingIndex); // palette index 0, tile index 0

    // Update sprite based on status
    switch (playerFacing)
    {
    case DIR_LEFT:
        if (frameCount % 20 > 9)
            startingIndex = PLAYER_FACING_LEFT_FR1;
        else
            startingIndex = PLAYER_FACING_LEFT_FR2;
        break;
    case DIR_RIGHT:
        player->attr1 ^= ATTR1_HFLIP;
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
    player->attr2 = ATTR2_BUILD(startingIndex, paletteBank, 0);
    obj_set_pos(player, playerScreenX, playerScreenY);

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

                // Randomize player position
                do
                {
                    playerX = randomInRange(1, MAP_WIDTH_TILES - 1);
                    playerY = randomInRange(1, MAP_HEIGHT_TILES - 1);
                } while (isSolid(playerX, playerY));

                #ifdef PRINT_MAP_DRAW
                    printMapInLog();
                #endif

                drawGameMap();
                doStateTransition(STATE_GAMEPLAY);
            }
            break;
        case STATE_GAMEPLAY:
            if (playerMoveOffsetX == 0 && playerMoveOffsetY == 0)
                doPlayerInput();
            if (playerAction != PLAYER_NO_ACTION)
            {
                doFOV(playerX, playerY, sightRange);
                REG_BLDALPHA= BLDA_BUILD(BG_0_BLEND_UP/8, blendingValue/8);
            }
            updateGraphics();
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
