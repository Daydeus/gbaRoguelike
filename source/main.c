#include <tonc.h>
#include <string.h>
#include "constants.h"
#include "gameMap.h"
#include "playerSprite.h"


struct Tile gameMap[MAP_HEIGHT][MAP_WIDTH];
void doStateTransition(enum state targetState);
void doPlayerInput();
void updatePlayerDraw(int playerX, int playerY);
void loadPlayerSprite(int playerScreenX, int playerScreenY);
void tte_write_var_int(int varToPrint);

// Global Variables ---------------------------------
OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;
unsigned int frame = 0;
enum state gameState = STATE_GAMEPLAY;
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
    REG_BG0CNT= BG_CBB(0) | BG_SBB(GAME_MAP_SB1) | BG_4BPP | BG_REG_64x32;
    REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;

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
            REG_BG0HOFS = screenOffsetX;
            REG_BG0VOFS = screenOffsetY;
            break;
        case STATE_MENU:
            if (KEY_EQ(key_hit, KI_START))
            {
                doStateTransition(STATE_GAMEPLAY);
            }
            break;
        }

        // Low-power for rest of frame
        VBlankIntrWait();
        frame++;
    }
}

void doStateTransition(enum state targetState)
{
    switch(targetState)
    {
    case STATE_GAMEPLAY:
        REG_BG0CNT= BG_CBB(0) | BG_SBB(GAME_MAP_SB1) | BG_4BPP | BG_REG_64x32;
        REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;
        gameState = STATE_GAMEPLAY;
        break;
    case STATE_MENU:
        REG_BG1CNT= BG_CBB(0) | BG_SBB(30) | BG_4BPP | BG_REG_64x32;
        REG_DISPCNT= DCNT_MODE0 | DCNT_BG1 | DCNT_OBJ_1D;
        tte_init_chr4c(1, BG_CBB(1) | BG_SBB(30), 0xF000, 0x0201, CLR_ORANGE<<16|CLR_BLACK, &vwf_default, NULL);
        tte_erase_screen();
        tte_set_pos(0, 0);
        tte_write("Debug Menu\n");
        tte_write("Player position (");
        tte_write_var_int(playerX);
        tte_write(", ");
        tte_write_var_int(playerY);
        tte_write(")");
        gameState = STATE_MENU;
        break;
    }
}

void tte_write_var_int(int varToPrint)
{
    for (int digitPlace = 10000; digitPlace >= 1; digitPlace = digitPlace / 10)
    {
        int numberToPrint = varToPrint / digitPlace % 10;

        switch(numberToPrint)
        {
        case 9:
            tte_write("9");
            break;
        case 8:
            tte_write("8");
            break;
        case 7:
            tte_write("7");
            break;
        case 6:
            tte_write("6");
            break;
        case 5:
            tte_write("5");
            break;
        case 4:
            tte_write("4");
            break;
        case 3:
            tte_write("3");
            break;
        case 2:
            tte_write("2");
            break;
        case 1:
            tte_write("1");
            break;
        case 0:
            if (digitPlace < varToPrint)
                tte_write("0");
        }
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
    if (KEY_EQ(key_hit, KI_SELECT))
    {
        playerFacing = FACING_LEFT;
        playerX = 10;
        playerY = 8;
        playerScreenX = 7;
        playerScreenY = 5;
        screenOffsetX = 48;
        screenOffsetY = 48;
    }
    if (KEY_EQ(key_hit, KI_START))
    {
        doStateTransition(STATE_MENU);
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
