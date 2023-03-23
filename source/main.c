#include <tonc.h>
#include <string.h>
#include "constants.h"
#include "gameMap.h"
#include "player_frame1.h"
#include "player_frame2.h"


struct Tile gameMap[MAP_HEIGHT][MAP_WIDTH];

enum facing
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

void initGameMap();
void loadGameMap();
uint8_t getDynamicTileId(struct Tile tile);
int* getTilesetIndex(struct Tile tile, uint8_t screenEntryCorner);

void doPlayerInput();
void updatePlayerDraw(int playerX, int playerY);
void loadSprite(int playerScreenX, int playerScreenY);

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;


// Global Variables ---------------------------------
unsigned int frame = 0;
int playerX = 7, playerY = 5;
enum facing playerFacing;
int playerScreenX = 7, playerScreenY = 5;
int screenOffsetX = 0, screenOffsetY = 0;

int main(void)
{
    irq_init(NULL);
    irq_enable(II_VBLANK);

    initGameMap();
    loadGameMap();

    // set up BG0 for a 4bpp 64x32t map,
    //   using charblock 0 and screenblock 31
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
        loadSprite(playerScreenX, playerScreenY);
        REG_BG0HOFS = screenOffsetX;
        REG_BG0VOFS = screenOffsetY;
        
        // Low-power for rest of frame
        VBlankIntrWait();
        frame++;
    }
}

void doPlayerInput()
{
    if (KEY_EQ(key_hit, KI_RIGHT) && (playerX < MAP_WIDTH - 1))
        {
            playerFacing = RIGHT;
            playerX++;

            if (7 < playerX && playerX < MAP_WIDTH - 7)
                screenOffsetX += TILE_SIZE;
            else
                playerScreenX++;
        }
        if (KEY_EQ(key_hit, KI_LEFT) && (playerX > 0))
        {
            playerFacing = LEFT;
            playerX--;

            if (6 < playerX && playerX < MAP_WIDTH - 8)
                screenOffsetX -= TILE_SIZE;
            else
                playerScreenX--;
        }if (KEY_EQ(key_hit, KI_DOWN) && (playerY < MAP_HEIGHT - 1))
        {
            playerFacing = DOWN;
            playerY++;

            if (5 < playerY && playerY < MAP_HEIGHT - 4)
                screenOffsetY += TILE_SIZE;
            else
                playerScreenY++;
        }
        if (KEY_EQ(key_hit, KI_UP) && (playerY > 0))
        {
            playerFacing = UP;
            playerY--;

            if (4 < playerY && playerY < MAP_HEIGHT - 5)
                screenOffsetY -= TILE_SIZE;
            else
                playerScreenY--;
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

void loadSprite(int playerScreenX, int playerScreenY)
{
    // Load tiles and palette of sprite into video and palete RAM
    if (frame % 20 > 9)
    {
        memcpy32(&tile_mem[4][0], player_frame1Tiles, player_frame1TilesLen / 4);
        memcpy32(pal_obj_mem, player_frame1Pal, player_frame1PalLen / 4);
    }
    else
    {
        memcpy32(&tile_mem[4][0], player_frame2Tiles, player_frame2TilesLen / 4);
        memcpy32(pal_obj_mem, player_frame2Pal, player_frame2PalLen / 4);
    }

    oam_init(obj_buffer, 128);

    OBJ_ATTR *player = &obj_buffer[0];
    obj_set_attr(player,
        ATTR0_SQUARE,  // Square, regular sprite
        ATTR1_SIZE_16, // 16x16 pixels,
        ATTR2_PALBANK(0) | 0); // palette index 0, tile index 0

    // Set position
    obj_set_pos(player, playerScreenX * TILE_SIZE, playerScreenY * TILE_SIZE);

    oam_copy(oam_mem, obj_buffer, 1); // Update first OAM object
}
