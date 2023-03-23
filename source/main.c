#include <tonc.h>
#include <string.h>
#include "constants.h"
#include "gameMap.h"
#include "playerSprite.h"


struct Tile gameMap[MAP_HEIGHT][MAP_WIDTH];

void doPlayerInput();
void updatePlayerDraw(int playerX, int playerY);
void loadPlayerSprite(int playerScreenX, int playerScreenY);

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;


// Global Variables ---------------------------------
unsigned int frame = 0;
int playerX = 10, playerY = 8;               // Player position on gameMap[][]
int playerScreenX = 7, playerScreenY = 5;   // Position of player sprite drawn on screen
int screenOffsetX = 48, screenOffsetY = 48; // Screen offset from TopLeft corner of gameMap[][]
enum facing playerFacing = FACING_LEFT;

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
    REG_BG0CNT= BG_CBB(0) | BG_SBB(30) | BG_4BPP | BG_REG_64x32;
    REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;

    /*
    tte_init_chr4c_default(0, BG_CBB(0) | BG_SBB(31));
    tte_set_pos(92, 68);
    tte_write("Hello World!");
    */

    while (1)
    {
        // Get player input
        key_poll();

        // Update position, AI, etc.
        doPlayerInput();

        // Render updates
        //updatePlayerDraw();
        loadPlayerSprite(playerScreenX, playerScreenY);
        REG_BG0HOFS = screenOffsetX;
        REG_BG0VOFS = screenOffsetY;
        
        // Low-power for rest of frame
        VBlankIntrWait();
        frame++;
    }
}

void doPlayerInput()
{
    if (KEY_EQ(key_hit, KI_LEFT) && (playerX > 0))
    {
        playerFacing = FACING_LEFT;
        if(isSolid(playerX - 1, playerY) ==  false)
        {
            playerX--;

            if (6 < playerX && playerX < MAP_WIDTH - 8)
                screenOffsetX -= TILE_SIZE;
            else
                playerScreenX--;
        }
    }
    if (KEY_EQ(key_hit, KI_RIGHT) && (playerX < MAP_WIDTH - 1))
    {
        playerFacing = FACING_RIGHT;
        if(isSolid(playerX + 1, playerY) ==  false)
        {
            playerX++;

            if (7 < playerX && playerX < MAP_WIDTH - 7)
                screenOffsetX += TILE_SIZE;
            else
                playerScreenX++;
        }
    }
    if (KEY_EQ(key_hit, KI_UP) && (playerY > 0))
    {
        playerFacing = FACING_UP;
        if(isSolid(playerX, playerY - 1) ==  false)
        {
            playerY--;

            if (4 < playerY && playerY < MAP_HEIGHT - 5)
                screenOffsetY -= TILE_SIZE;
            else
                playerScreenY--;
        }
    }
    if (KEY_EQ(key_hit, KI_DOWN) && (playerY < MAP_HEIGHT - 1))
    {
        playerFacing = FACING_DOWN;
        if(isSolid(playerX, playerY + 1) ==  false)
        {
            playerY++;

            if (5 < playerY && playerY < MAP_HEIGHT - 4)
                screenOffsetY += TILE_SIZE;
            else
                playerScreenY++;
        }
    }
    if (KEY_EQ(key_hit, KI_START))
    {
        playerX = 7;
        playerY = 5;
        playerScreenX = 7;
        playerScreenY = 5;
        screenOffsetX = 0;
        screenOffsetY = 0;
    }
}

void updatePlayerDraw(int playerX, int playerY)
{
    
}

void loadPlayerSprite(int playerScreenX, int playerScreenY)
{
    unsigned int startingIndex = 0, paletteBank = 0;
    OBJ_ATTR *player = &obj_buffer[0];

    obj_set_attr(player,
        ATTR0_SQUARE,  // Square, regular sprite
        ATTR1_SIZE_16, // 16x16 pixels,
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
