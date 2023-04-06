#include <tonc.h>
#include <string.h>
#include "constants.h"
#include "gameMap.h"
#include "globals.h"
#include "pauseMenu.h"

//------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------
void tte_write_var_int(int varToPrint);
void toggleValue(int value);

//------------------------------------------------------------------
// Function: doPauseMenuInput
// 
// Changes variable values by reading keys pressed with key_poll()
// Only used while in STATE_MENU
//------------------------------------------------------------------
bool doPauseMenuInput()
{
    if (KEY_EQ(key_hit, KI_A))
    {
        debugCollisionIsOff = (debugCollisionIsOff == false) ? true : false;
        return true;
    }
    if (KEY_EQ(key_hit, KI_B))
    {
        sightRange = (sightRange == SIGHT_RANGE_MIN) ? SIGHT_RANGE_MAX : SIGHT_RANGE_MIN;
        return true;
    }
    if (KEY_EQ(key_hit, KI_LEFT))
    {
        evb -= 20;
        return true;
    }
    if (KEY_EQ(key_hit, KI_RIGHT))
    {
        evb += 20;
        return true;
    }
    if (KEY_EQ(key_hit, KI_UP))
    {
        eva += 20;
        return true;
    }
    if (KEY_EQ(key_hit, KI_DOWN))
    {
        eva -= 20;
        return true;
    }
    if (KEY_EQ(key_hit, KI_START))
    {
        doStateTransition(STATE_GAMEPLAY);
        return false;
    }
    eva = clamp(eva, 0, 0x81);
    evb = clamp(evb, 0, 0x81);
    return false;
}

//------------------------------------------------------------------
// Function: drawPauseMenu
// 
// Prints the menu's text as 8x8 graphical tiles into the menu's
// screen entries
//------------------------------------------------------------------
void drawPauseMenu()
{
    tte_erase_screen();
    tte_set_pos(0, 0);
    tte_write("Pause Menu\n");
    tte_write("Player position (");
    tte_write_var_int(player.x);
    tte_write(", ");
    tte_write_var_int(player.y);
    tte_write(")\n");
    tte_write("UP/DOWN\teva: ");
    tte_write_var_int(eva);
    tte_write("\nLEFT/RIGHT\tevb: ");
    tte_write_var_int(evb);
    tte_write("\nA-BUTTON\tCollision: ");
    (debugCollisionIsOff == true) ? tte_write("OFF") : tte_write("ON");
    tte_write("\nB-BUTTON\tSight Range: ");
    (sightRange == SIGHT_RANGE_MIN) ? tte_write("MIN") : tte_write("MAX");
}

//------------------------------------------------------------------
// Function: doStateTransition
// 
// Sets the correct graphics mode for the new gameState and any
// necessary variable values.
//------------------------------------------------------------------
void doStateTransition(enum state targetState)
{
    switch(targetState)
    {
    case STATE_GAMEPLAY:
        REG_BG0CNT= BG_CBB(0) | BG_SBB(GAME_HUD_SB) | BG_4BPP | BG_REG_32x32;
        REG_BG1CNT= BG_CBB(0) | BG_SBB(FOV_SB) | BG_4BPP | BG_REG_32x32;
        REG_BG2CNT= BG_CBB(0) | BG_SBB(GAME_MAP_SB1) | BG_4BPP | BG_REG_64x32;
        REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;
        doFOV(player.x, player.y, sightRange);
        gameState = STATE_GAMEPLAY;
        break;
    case STATE_MENU:
        REG_BG1CNT= BG_CBB(0) | BG_SBB(PAUSE_MENU_SB) | BG_4BPP | BG_REG_32x32;
        REG_DISPCNT= DCNT_MODE0 | DCNT_BG1 | DCNT_OBJ_1D;
        tte_init_chr4c(SCREEN_BG_1, BG_CBB(1) | BG_SBB(PAUSE_MENU_SB), 0xF000, 0x0201, CLR_ORANGE<<16|CLR_BLACK, &vwf_default, NULL);
        drawPauseMenu(eva, evb);
        gameState = STATE_MENU;
        break;
    }
}

//------------------------------------------------------------------
// Function: tte_write_var_int
// 
// Tonc's Text Engine: write variable integers
// Prints as 8x8 graphical tiles each digit of the given number.
// Used in the debug menu to print variable integers.
//------------------------------------------------------------------
void tte_write_var_int(int varToPrint)
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
