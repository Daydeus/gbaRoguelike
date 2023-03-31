#include <tonc.h>
#include <string.h>
#include "constants.h"
#include "debug.h"
#include "gameMap.h"
#include "globals.h"
#include "mgba.h"
#include "playerSprite.h"

/******************************************************************/
/* Data Structures                                                */
/******************************************************************/
struct Tile gameMap[MAP_HEIGHT][MAP_WIDTH];

/******************************************************************/
/* Global Variables                                               */
/******************************************************************/
OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;
unsigned int frame = 0;
enum state gameState = STATE_GAMEPLAY;
int playerX = 10, playerY = 8;
int screenOffsetX = 48, screenOffsetY = 48;
enum direction playerFacing = DIR_LEFT;
enum direction playerMovedDir = DIR_NULL;
bool debugCollisionIsOff = false;
u32 eva = 0x80, evb = 0;
int8_t dirX[5] = {0, -1, 1, 0, 0};
int8_t dirY[5] = {0, 0, 0, -1, 1};
int8_t offsetX = 0, offsetY = 0;

/******************************************************************/
/* Function Prototypes                                            */
/******************************************************************/
void doPlayerInput();
uint8_t getPlayerScreenCoord(uint8_t playerPos, int8_t offset, int8_t dimension);
void approach(int8_t *varToIncrement, int8_t incrementTarget);
void movePlayer(int8_t direction);
void updateGraphics();
void loadPlayerSprite(int playerScreenX, int playerScreenY);

/******************************************************************/
/* Function: doPlayerInput                                        */
/*                                                                */
/* Updates the player character based on info read from key_poll()*/
/* TODO: Create seperate function for out-of-bounds check and test*/
/* at the same time as isSolid()                                  */
/******************************************************************/
void doPlayerInput()
{
    if (KEY_EQ(key_hit, KI_LEFT))                        // Left Key
    {
        playerFacing = DIR_LEFT;
        if(isSolid(playerX - 1, playerY) ==  false
        && isOutOfBounds(playerX - 1, playerY) == false)
        {
            movePlayer(playerFacing);
            playerMovedDir = DIR_LEFT;

        }
        #ifdef DEBUG
            mgba_printf(MGBA_LOG_INFO, "moved LEFT, position: %d, %d", playerX, playerY);
        #endif
    }
    if (KEY_EQ(key_hit, KI_RIGHT))                      // Right Key
    {
        playerFacing = DIR_RIGHT;
        if(isSolid(playerX + 1, playerY) ==  false
        && isOutOfBounds(playerX + 1, playerY) == false)
        {
            movePlayer(playerFacing);
            playerMovedDir = DIR_RIGHT;

        }
        #ifdef DEBUG
            mgba_printf(MGBA_LOG_INFO, "pressed RIGHT, position: %d, %d", playerX, playerY);
        #endif
    }
    if (KEY_EQ(key_hit, KI_UP))                            // Up Key
    {
        playerFacing = DIR_UP;
        if(isSolid(playerX, playerY - 1) ==  false
        && isOutOfBounds(playerX, playerY - 1) == false)
        {
            movePlayer(playerFacing);
            playerMovedDir = DIR_UP;

        }
        #ifdef DEBUG
            mgba_printf(MGBA_LOG_INFO, "pressed UP, position: %d, %d", playerX, playerY);
        #endif
    }
    if (KEY_EQ(key_hit, KI_DOWN))                        // Down Key
    {
        playerFacing = DIR_DOWN;
        if(isSolid(playerX, playerY + 1) ==  false
        && isOutOfBounds(playerX, playerY + 1) == false)
        {
            movePlayer(playerFacing);
            playerMovedDir = DIR_DOWN;

        }
        #ifdef DEBUG
            mgba_printf(MGBA_LOG_INFO, "pressed DOWN, position: %d, %d", playerX, playerY);
        #endif
    }
    if (KEY_EQ(key_hit, KI_SELECT))
    {
        #ifdef DEBUG
            mgba_printf(MGBA_LOG_INFO, "pressed SELECT");
        #endif
    }
    if (KEY_EQ(key_hit, KI_START))
    {
        #ifdef DEBUG
            mgba_printf(MGBA_LOG_INFO, "pressed START");
        #endif
        doStateTransition(STATE_MENU);
    }
}

/******************************************************************/
/* Function: approach                                             */
/*                                                                */
/* Increment the passed variable to the target goal               */
/******************************************************************/
void approach(int8_t *varToIncrement, int8_t incrementTarget)
{
    if ((int8_t)varToIncrement < incrementTarget)
        varToIncrement += 1;
    else if ((int8_t)varToIncrement > incrementTarget)
        varToIncrement -= 1;
    
}

/******************************************************************/
/* Function: movePlayer                                           */
/*                                                                */
/* Move the player in the given direction                         */
/******************************************************************/
void movePlayer(int8_t direction)
{
    playerX += dirX[direction];
    playerY += dirY[direction];
}

/******************************************************************/
/* Function: getPlayerScreenCoord                                 */
/*                                                                */
/* Get the coords for drawing player sprite based on map position */
/******************************************************************/
uint8_t getPlayerScreenCoord(uint8_t playerPos, int8_t offset, int8_t dimension)
{
    uint8_t lowerBound = 0, upperBound = 0;
    uint8_t screenCoord = 0;
    lowerBound = (dimension == DIM_WIDTH) ? 6 : 5;
    upperBound = (dimension == DIM_WIDTH) ? MAP_WIDTH - 8 : MAP_HEIGHT - 4;

    if (dimension == DIM_WIDTH)
    {
        if (lowerBound >= playerPos)
            screenCoord = playerPos * TILE_SIZE - offset;
        else if (upperBound <= playerPos)
            screenCoord = SCREEN_WIDTH - (MAP_WIDTH - playerPos) * TILE_SIZE - offset;
        else
            screenCoord = SCREEN_WIDTH / 2 - TILE_SIZE / 2;
    }
    else
    {
        if (lowerBound >= playerPos)
            screenCoord = playerPos * TILE_SIZE - offset;
        else if (upperBound <= playerPos)
            screenCoord = SCREEN_HEIGHT - (MAP_HEIGHT - playerPos) * TILE_SIZE - offset;
        else
            screenCoord = SCREEN_HEIGHT / 2;
    }

    return screenCoord;
}

/******************************************************************/
/* Function: updateGraphics                                       */
/*                                                                */
/* Updates player sprite position and screen map offset based on  */
/* player input                                                   */
/******************************************************************/
void updateGraphics()
{
    int playerScreenX = 0, playerScreenY = 0;

    if (offsetX != 0 || offsetY != 0)
    {
        if (offsetX > 0)
            offsetX -= 2;
        else if (offsetX < 0)
            offsetX += 2;
        if (offsetY > 0)
            offsetY -= 2;
        else if (offsetY < 0)
            offsetY += 2;
    }
    else
    {
        switch (playerMovedDir)
        {
        case DIR_LEFT:
                offsetX += dirX[playerMovedDir] * TILE_SIZE;
            if (7 <= playerX && playerX < MAP_WIDTH - 8
            && offsetX != 0)
                screenOffsetX -= TILE_SIZE;
            break;
        case DIR_RIGHT:
                offsetX += dirX[playerMovedDir] * TILE_SIZE;
            if (7 <= playerX && playerX < MAP_WIDTH - 8)
                screenOffsetX += TILE_SIZE;
            break;
        case DIR_UP:
                offsetY += dirY[playerMovedDir] * TILE_SIZE;
            if (4 <= playerY && playerY < MAP_HEIGHT - 5)
                screenOffsetY -= TILE_SIZE;
            break;
        case DIR_DOWN:
                offsetY += dirY[playerMovedDir] * TILE_SIZE;
            if (5 < playerY && playerY < MAP_HEIGHT - 4)
                screenOffsetY += TILE_SIZE;
            break;
        default:
            break;
        }
    }
    playerScreenX = getPlayerScreenCoord(playerX, offsetX, DIM_WIDTH);
    playerScreenY = getPlayerScreenCoord(playerY, offsetY, DIM_HEIGHT);
    REG_BG0HOFS = screenOffsetX;
    REG_BG0VOFS = screenOffsetY;
    playerMovedDir = DIR_NULL;
    loadPlayerSprite(playerScreenX, playerScreenY);
    REG_BLDALPHA= BLDA_BUILD(eva/8, evb/8);
}

/******************************************************************/
/* Function: loadPlayerSprite                                     */
/*                                                                */
/* Loads the correct index of the player sprite based on frame    */
/* count and on facing direction. TODO: break into smaller funcs. */
/******************************************************************/
void loadPlayerSprite(int playerScreenX, int playerScreenY)
{
    unsigned int startingIndex = 0, paletteBank = 0;
    OBJ_ATTR *player = &obj_buffer[0];

    obj_set_attr(player,
        ATTR0_SQUARE | ATTR0_BLEND,                  // Square, regular sprite
        ATTR1_SIZE_16,                               // 16x16 pixels,
        ATTR2_PALBANK(paletteBank) | startingIndex); // palette index 0, tile index 0

    // Update sprite based on status
    switch (playerFacing)
    {
    case DIR_LEFT:
        if (frame % 20 > 9)
            startingIndex = PLAYER_FACING_LEFT_FR1;
        else
            startingIndex = PLAYER_FACING_LEFT_FR2;
        break;
    case DIR_RIGHT:
        player->attr1 ^= ATTR1_HFLIP;
        if (frame % 20 > 9)
            startingIndex = PLAYER_FACING_LEFT_FR1;
        else
            startingIndex = PLAYER_FACING_LEFT_FR2;
        break;
    case DIR_UP:
        if (frame % 20 > 9)
            startingIndex = PLAYER_FACING_UP_FR1;
        else
            startingIndex = PLAYER_FACING_UP_FR2;
        break;
    case DIR_DOWN:
        if (frame % 20 > 9)
            startingIndex = PLAYER_FACING_DOWN_FR1;
        else
            startingIndex = PLAYER_FACING_DOWN_FR2;
        break;
    default:
        if (frame % 20 > 9)
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

/******************************************************************/
/* Function: main                                                 */
/*                                                                */
/* Entry point for the program                                    */
/******************************************************************/
int main(void)
{
    #ifdef DEBUG
        mgba_console_open();
    #endif

    irq_init(NULL);
    irq_enable(II_VBLANK);

    initGameMap();
    loadGameMap();


    // Load tiles and palette of sprite into video and palete RAM
    memcpy32(&tile_mem[4][0], playerSpriteTiles, playerSpriteTilesLen / 4);
    memcpy32(pal_obj_mem, playerSpritePal, playerSpritePalLen / 4);

    // set up BG0 for a 4bpp 64x32t map,
    //   using charblock 0 and screenblock 31
    oam_init(obj_buffer, 128);
    REG_BG0CNT= BG_CBB(0) | BG_SBB(GAME_MAP_SB1) | BG_4BPP | BG_REG_64x32;
    REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;

    REG_BLDCNT= BLD_BUILD(
        BLD_OBJ,     // Top layers
        BLD_BG0,     // Bottom layers
        DCNT_MODE0); // Mode

    while (1)
    {
        // Get player input
        key_poll();

        // gameState program control
        switch(gameState)
        {
        case STATE_GAMEPLAY:
            doPlayerInput();
            updateGraphics();
            break;
        case STATE_MENU:
            if(doDebugMenuInput(eva, evb) == true)
                drawDebugMenu();
            break;
        }

        // Low-power for rest of frame
        VBlankIntrWait();
        frame++;
    }
}
