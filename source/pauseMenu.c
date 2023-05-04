#include <string.h>
#include "../libtonc/include/tonc.h"
#include "constants.h"
#include "debug.h"
#include "entity.h"
#include "globals.h"
#include "fieldOfVision.h"
#include "mgba.h"
#include "pauseMenu.h"
#include "tile.h"

//------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------
static void tte_write_var_int(int const varToPrint);

//------------------------------------------------------------------
// Function: tte_write_var_int
// 
// Tonc's Text Engine: write variable integers
// Prints as 8x8 graphical tiles each digit of the given number.
// Used in the debug menu to print variable integers.
//------------------------------------------------------------------
static void tte_write_var_int(int const varToPrint)
{
    for (int digitPlace = 1000000000; digitPlace >= 1; digitPlace = digitPlace / 10)
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

//------------------------------------------------------------------
// Function: doPauseMenuInput
// 
// Changes variable values by reading keys pressed with key_poll()
// Only used while in STATE_MENU
//------------------------------------------------------------------
extern boolean doPauseMenuInput()
{
    struct Entity *player = getEntity(PLAYER_INDEX);

    if (KEY_EQ(key_hit, KI_A))
    {
        debugCollisionIsOff = (debugCollisionIsOff == FALSE) ? TRUE : FALSE;
        return TRUE;
    }
    if (KEY_EQ(key_hit, KI_B))
    {
        debugMapIsVisible = (debugMapIsVisible == FALSE) ? TRUE : FALSE;
        return TRUE;
    }
    if (KEY_EQ(key_hit, KI_LEFT))
    {
        blendingValue -= 20;
        return TRUE;
    }
    if (KEY_EQ(key_hit, KI_RIGHT))
    {
        blendingValue += 20;
        return TRUE;
    }
    if (KEY_EQ(key_hit, KI_UP))
    {
        setEntitySightRange(player, clamp(player->sightRange + 1, SIGHT_RANGE_SELF, SIGHT_RANGE_MAX + 1));
        return TRUE;
    }
    if (KEY_EQ(key_hit, KI_DOWN))
    {
        setEntitySightRange(player, clamp(player->sightRange - 1, SIGHT_RANGE_SELF, SIGHT_RANGE_MAX + 1));
        return TRUE;
    }
    if (KEY_EQ(key_hit, KI_START))
    {
        doStateTransition(STATE_GAMEPLAY);
        return FALSE;
    }
    blendingValue = clamp(blendingValue, 0, 0x81);
    return FALSE;
}

//------------------------------------------------------------------
// Function: drawPauseMenu
// 
// Prints the menu's text as 8x8 graphical tiles into the menu's
// screen entries
//------------------------------------------------------------------
extern void drawPauseMenu()
{
    struct Entity *player = getEntity(PLAYER_INDEX);

    tte_erase_screen();
    tte_set_pos(0, 0);
    tte_write("Pause Menu\t\t\tRNG Seed: ");
    tte_write_var_int(randomSeed);
    tte_write("\nPlayer position (");
    tte_write_var_int(player->posX);
    tte_write(", ");
    tte_write_var_int(player->posY);
    tte_write(")");
    tte_write("\nPlayer sightId: ");
    tte_write_var_int(playerSightId);
    tte_write("\nUP/DOWN\tSight Range: ");
    tte_write_var_int(player->sightRange);
    tte_write("\nLEFT/RIGHT\tBG Blending: ");
    tte_write_var_int(blendingValue);
    tte_write("\nA-BUTTON\tCollision: ");
    (debugCollisionIsOff == TRUE) ? tte_write("OFF") : tte_write("ON");
    tte_write("\nB-BUTTON\tMapVisible: ");
    (debugMapIsVisible == TRUE) ? tte_write("ON") : tte_write("OFF");
}

//------------------------------------------------------------------
// Function: doStateTransition
// 
// Sets the correct graphics mode for the new gameState and any
// necessary variable values.
//------------------------------------------------------------------
extern void doStateTransition(enum state const targetState)
{
    switch(targetState)
    {
    case STATE_TITLE_SCREEN:
        gameState = STATE_TITLE_SCREEN;
        break;
    case STATE_GAMEPLAY:
        struct Entity *player = getEntity(PLAYER_INDEX);

        REG_BG0CNT= BG_CBB(0) | BG_SBB(GAME_HUD_SB) | BG_4BPP | BG_REG_32x32;
        REG_BG1CNT= BG_CBB(0) | BG_SBB(FOV_SB) | BG_4BPP | BG_REG_32x32;
        REG_BG2CNT= BG_CBB(0) | BG_SBB(GAME_MAP_SB) | BG_4BPP | BG_REG_32x32;
        REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;

        doFOV(player->posX, player->posY, player->sightRange);
        drawGameMap(player->posX - SCREEN_WIDTH_TILES / 2, player->posY - SCREEN_HEIGHT_TILES / 2);
        gameState = STATE_GAMEPLAY;
        break;
    case STATE_MENU:
        REG_BG1CNT= BG_CBB(0) | BG_SBB(PAUSE_MENU_SB) | BG_4BPP | BG_REG_32x32;
        REG_DISPCNT= DCNT_MODE0 | DCNT_BG1 | DCNT_OBJ_1D;

        tte_init_chr4c(SCREEN_BG_1, BG_CBB(1) | BG_SBB(PAUSE_MENU_SB), 0xF000, 0x0201, CLR_ORANGE<<16|CLR_BLACK, &vwf_default, NULL);
        drawPauseMenu(blendingValue);
        gameState = STATE_MENU;
        break;
    default:
        break;
    }

    #ifdef DEBUG
        mgba_printf(MGBA_LOG_DEBUG, "doStateTransition: targetState: %d", targetState);
    #endif
}
