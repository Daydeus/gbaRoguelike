#include <tonc.h>
#include <string.h>
#include "constants.h"
#include "debug.h"
#include "gameMap.h"
#include "globals.h"
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
int playerScreenX = 7, playerScreenY = 5;
int screenOffsetX = 48, screenOffsetY = 48;
enum facing playerFacing = FACING_LEFT;
bool debugCollisionIsOff = false;
u32 eva = 0x80, evb = 0;

/******************************************************************/
/* Function Prototypes                                            */
/******************************************************************/
void doPlayerInput();
void updatePlayerDraw(int playerX, int playerY);
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
        playerFacing = FACING_LEFT;
        if(isSolid(playerX - 1, playerY) ==  false
            && isOutOfBounds(playerX - 1, playerY) == false)
        {
            playerX--;

            if (6 < playerX && playerX < MAP_WIDTH - 8)
                screenOffsetX -= TILE_SIZE;
            else
                playerScreenX--;
        }
    }
    if (KEY_EQ(key_hit, KI_RIGHT))                      // Right Key
    {
        playerFacing = FACING_RIGHT;
        if(isSolid(playerX + 1, playerY) ==  false
            && isOutOfBounds(playerX + 1, playerY) == false)
        {
            playerX++;

            if (7 < playerX && playerX < MAP_WIDTH - 7)
                screenOffsetX += TILE_SIZE;
            else
                playerScreenX++;
        }
    }
    if (KEY_EQ(key_hit, KI_UP))                            // Up Key
    {
        playerFacing = FACING_UP;
        if(isSolid(playerX, playerY - 1) ==  false
            && isOutOfBounds(playerX, playerY - 1) == false)
        {
            playerY--;

            if (4 < playerY && playerY < MAP_HEIGHT - 5)
                screenOffsetY -= TILE_SIZE;
            else
                playerScreenY--;
        }
    }
    if (KEY_EQ(key_hit, KI_DOWN))                        // Down Key
    {
        playerFacing = FACING_DOWN;
        if(isSolid(playerX, playerY + 1) ==  false
            && isOutOfBounds(playerX, playerY + 1) == false)
        {
            playerY++;

            if (5 < playerY && playerY < MAP_HEIGHT - 4)
                screenOffsetY += TILE_SIZE;
            else
                playerScreenY++;
        }
    }
    if (KEY_EQ(key_hit, KI_SELECT))
    {
    }
    if (KEY_EQ(key_hit, KI_START))
    {
        doStateTransition(STATE_MENU);
    }
}

/******************************************************************/
/* Function: updatePlayerDraw()                                   */
/*                                                                */
/*                                                                */
/******************************************************************/
void updatePlayerDraw(int playerX, int playerY)
{
    
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
    switch(playerFacing)
    {
    case FACING_LEFT:
        if (frame % 20 > 9)
            startingIndex = PLAYER_FACING_LEFT_FR1;
        else
            startingIndex = PLAYER_FACING_LEFT_FR2;
        break;
    case FACING_RIGHT:
        player->attr1 ^= ATTR1_HFLIP;
        if (frame % 20 > 9)
            startingIndex = PLAYER_FACING_LEFT_FR1;
        else
            startingIndex = PLAYER_FACING_LEFT_FR2;
        break;
    case FACING_UP:
        if (frame % 20 > 9)
            startingIndex = PLAYER_FACING_UP_FR1;
        else
            startingIndex = PLAYER_FACING_UP_FR2;
        break;
    case FACING_DOWN:
        if (frame % 20 > 9)
            startingIndex = PLAYER_FACING_DOWN_FR1;
        else
            startingIndex = PLAYER_FACING_DOWN_FR2;
        break;
    }
    // Finalize sprite changes
    player->attr2 = ATTR2_BUILD(startingIndex, paletteBank, 0);
    obj_set_pos(player, playerScreenX * TILE_SIZE, playerScreenY * TILE_SIZE);

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
            // Update position, AI, etc.
            doPlayerInput();

            // Render updates
            //updatePlayerDraw();
            loadPlayerSprite(playerScreenX, playerScreenY);
            REG_BLDALPHA= BLDA_BUILD(eva/8, evb/8);
            REG_BG0HOFS = screenOffsetX;
            REG_BG0VOFS = screenOffsetY;
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
