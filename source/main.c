#include <tonc.h>
#include <string.h>
#include "constants.h"
#include "gameMap.h"
#include "globals.h"
#include "mgba.h"
#include "pauseMenu.h"
#include "playerSprite.h"

/******************************************************************/
/* Data Structures                                                */
/******************************************************************/
struct Tile gameMap[MAP_HEIGHT_TILES][MAP_WIDTH_TILES];

/******************************************************************/
/* Global Variables                                               */
/******************************************************************/
OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;
unsigned int frame = 0;
enum state gameState = STATE_GAMEPLAY;
int playerX = 1, playerY = 1;
int screenOffsetX = 0, screenOffsetY = 240;
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
void approach(int8_t *varToIncrement, int8_t incrementTarget);
void movePlayer(int8_t direction);
void drawHUD();
uint8_t isNearScreenEdge(int position, uint8_t dimension);
uint8_t getPlayerScreenCoord(uint8_t playerPos, int8_t offset, int8_t dimension);
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
/* Function: drawHUD                                              */
/*                                                                */
/* Draw placeholder HUD for player in STATE_GAMEPLAY              */
/******************************************************************/
void drawHUD()
{
    int screenEntryTL = 0;                     // screenEntryTopLeft
    int screenEntryTR = 0;                    // screenEntryTopRight
    int screenEntryBL = 0;                  // screenEntryBottomLeft
    int screenEntryBR = 0;                 // screenEntryBottomRight
    int *tileToDraw = NULL;

    for (int x = 0; x <= 15; x++)
    {
        screenEntryTL = x * 2;
        screenEntryTR = screenEntryTL + 1;
        screenEntryBL = screenEntryTL + SCREEN_BLOCK_WIDTH;
        screenEntryBR = screenEntryBL + 1;
        
            // Get and copy the 8x8 tile into map memory
            tileToDraw = (int*)HEART_TL;
            memcpy(&se_mem[31][screenEntryTL], &tileToDraw, 2);

            tileToDraw = (int*)HEART_TR;
            memcpy(&se_mem[31][screenEntryTR], &tileToDraw, 2);

            tileToDraw = (int*)HEART_BL;
            memcpy(&se_mem[31][screenEntryBL], &tileToDraw, 2);

            tileToDraw = (int*)HEART_BR;
            memcpy(&se_mem[31][screenEntryBR], &tileToDraw, 2);
    }
}

/******************************************************************/
/* Function: isNearScreenEdge                                     */
/*                                                                */
/* Check if position is within half screen length of horizontal   */
/* or vertical map edge                                           */
/******************************************************************/
uint8_t isNearScreenEdge(int position, uint8_t dimension)
{
    if (dimension == DIM_WIDTH)
    {
        if (7 >= position)
            return DIR_LEFT;
        else if (position >= MAP_WIDTH_TILES - 8)
            return DIR_RIGHT;
    }
    else if (dimension == DIM_HEIGHT)
    {
        if (4 >= position)
            return DIR_UP;
        else if (position >= MAP_HEIGHT_TILES - 5)
            return DIR_DOWN;
    }
    return DIR_NULL;
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
    lowerBound = (dimension == DIM_WIDTH) ? 7 : 3;
    upperBound = (dimension == DIM_WIDTH) ? MAP_WIDTH_TILES - 8 : MAP_HEIGHT_TILES - 4;

    if (dimension == DIM_WIDTH)
    {
        if (lowerBound >= playerPos)
            screenCoord = playerPos * TILE_SIZE - offset;
        else if (upperBound <= playerPos)
            screenCoord = SCREEN_WIDTH - (MAP_WIDTH_TILES - playerPos) * TILE_SIZE - offset;
        else
            screenCoord = SCREEN_WIDTH / 2 - TILE_SIZE / 2;
    }
    else if (dimension == DIM_HEIGHT)
    {
        if (lowerBound >= playerPos)
            screenCoord = (playerPos + 1) * TILE_SIZE - offset;
        else if (upperBound <= playerPos)
            screenCoord = SCREEN_HEIGHT - (MAP_HEIGHT_TILES - playerPos) * TILE_SIZE - offset;
        else
            screenCoord = SCREEN_HEIGHT / 2;
    }

    return screenCoord;
}

/******************************************************************/
/* Function: getScreenOffset                                      */
/*                                                                */
/* Calc screen offset using player position and given dimension   */
/******************************************************************/
uint16_t getScreenOffset(uint8_t dimension, int8_t offset)
{
    if (dimension == DIM_WIDTH)
    {
        if (isNearScreenEdge(playerX, dimension) == DIR_LEFT)
            return 0;
        else if (isNearScreenEdge(playerX, dimension) == DIR_RIGHT)
            return (MAP_WIDTH_TILES - SCREEN_WIDTH_TILES) * TILE_SIZE;
        else
            return (playerX - SCREEN_WIDTH_TILES / 2) * TILE_SIZE - offset;
    }
    else if (dimension == DIM_HEIGHT)
    {
        if (isNearScreenEdge(playerY, dimension) == DIR_UP)
            return (MAP_HEIGHT_TILES - 1) * TILE_SIZE;        // 240
        else if (isNearScreenEdge(playerY, dimension) == DIR_DOWN)
            return (MAP_HEIGHT_TILES - SCREEN_HEIGHT_TILES) * TILE_SIZE;
        else
            return (playerY - SCREEN_HEIGHT_TILES / 2) * TILE_SIZE - offset;
    }
    return 0;
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
        //if (frame % 10 == 1)
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
    }
    else
    {
        switch (playerMovedDir) // Falls to default if player hasn't moved position
        {
        case DIR_LEFT:
            offsetX += dirX[playerMovedDir] * TILE_SIZE;
            break;
        case DIR_RIGHT:
            offsetX += dirX[playerMovedDir] * TILE_SIZE;
            break;
        case DIR_UP:
            offsetY += dirY[playerMovedDir] * TILE_SIZE;
            break;
        case DIR_DOWN:
            offsetY += dirY[playerMovedDir] * TILE_SIZE;
            break;
        default:
            break;
        }
    }
    playerScreenX = getPlayerScreenCoord(playerX, offsetX, DIM_WIDTH);
    playerScreenY = getPlayerScreenCoord(playerY, offsetY, DIM_HEIGHT);
    REG_BG1HOFS = getScreenOffset(DIM_WIDTH, offsetX);
    REG_BG1VOFS = getScreenOffset(DIM_HEIGHT, offsetY);
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
    drawHUD();


    // Load tiles and palette of sprite into video and palete RAM
    memcpy32(&tile_mem[4][0], playerSpriteTiles, playerSpriteTilesLen / 4);
    memcpy32(pal_obj_mem, playerSpritePal, playerSpritePalLen / 4);

    // set up BG0 for a 4bpp 64x32t map,
    //   using charblock 0 and screenblock 31
    oam_init(obj_buffer, 128);
    REG_BG0CNT= BG_CBB(0) | BG_SBB(31) | BG_4BPP | BG_REG_32x32;
    REG_BG1CNT= BG_CBB(0) | BG_SBB(GAME_MAP_SB1) | BG_4BPP | BG_REG_64x32;
    REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_OBJ | DCNT_OBJ_1D;

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
            if (offsetX == 0 && offsetY == 0)
            doPlayerInput();
            updateGraphics();
            #ifdef DEBUG
                if (offsetX != 0 || offsetY != 0)
                    mgba_printf(MGBA_LOG_DEBUG, "offsetX %d, offseY %d", offsetX, offsetY);
            #endif
            break;
        case STATE_MENU:
            if(doPauseMenuInput(eva, evb) == true)
                drawPauseMenu();
            break;
        }

        // Low-power for rest of frame
        VBlankIntrWait();
        frame++;
    }
}
