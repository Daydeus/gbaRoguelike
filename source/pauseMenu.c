#include <tonc.h>
#include <string.h>
#include "constants.h"
#include "globals.h"
#include "fieldOfVision.h"
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
extern bool doPauseMenuInput()
{
    if (KEY_EQ(key_hit, KI_A))
    {
        debugCollisionIsOff = (debugCollisionIsOff == false) ? true : false;
        return true;
    }
    if (KEY_EQ(key_hit, KI_B))
    {
        debugMapIsVisible = (debugMapIsVisible == false) ? true : false;
        return true;
    }
    if (KEY_EQ(key_hit, KI_LEFT))
    {
        blendingValue -= 20;
        return true;
    }
    if (KEY_EQ(key_hit, KI_RIGHT))
    {
        blendingValue += 20;
        return true;
    }
    if (KEY_EQ(key_hit, KI_UP))
    {
        sightRange += 1;
        return true;
    }
    if (KEY_EQ(key_hit, KI_DOWN))
    {
        sightRange -= 1;
        return true;
    }
    if (KEY_EQ(key_hit, KI_START))
    {
        doStateTransition(STATE_GAMEPLAY);
        return false;
    }
    sightRange = clamp(sightRange, SIGHT_RANGE_SELF, SIGHT_RANGE_MAX + 1);
    blendingValue = clamp(blendingValue, 0, 0x81);
    return false;
}

//------------------------------------------------------------------
// Function: drawPauseMenu
// 
// Prints the menu's text as 8x8 graphical tiles into the menu's
// screen entries
//------------------------------------------------------------------
extern void drawPauseMenu()
{
    tte_erase_screen();
    tte_set_pos(0, 0);
    tte_write("Pause Menu\t\t\tRNG Seed: ");
    tte_write_var_int(randomSeed);
    tte_write("\nPlayer position (");
    tte_write_var_int(playerX);
    tte_write(", ");
    tte_write_var_int(playerY);
    tte_write(")");
    tte_write("\nPlayer sightId: ");
    tte_write_var_int(playerSightId);
    tte_write("\nUP/DOWN\tSight Range: ");
    tte_write_var_int(sightRange);
    tte_write("\nLEFT/RIGHT\tBG Blending: ");
    tte_write_var_int(blendingValue);
    tte_write("\nA-BUTTON\tCollision: ");
    (debugCollisionIsOff == true) ? tte_write("OFF") : tte_write("ON");
    tte_write("\nB-BUTTON\tMapVisible: ");
    (debugMapIsVisible == true) ? tte_write("ON") : tte_write("OFF");
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
        REG_BG0CNT= BG_CBB(0) | BG_SBB(GAME_HUD_SB) | BG_4BPP | BG_REG_32x32;
        REG_BG1CNT= BG_CBB(0) | BG_SBB(FOV_SB) | BG_4BPP | BG_REG_32x32;
        REG_BG2CNT= BG_CBB(0) | BG_SBB(GAME_MAP_SB) | BG_4BPP | BG_REG_32x32;
        REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;
        //doFOV(playerX, playerY, sightRange);
        //drawGameMap();
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
}
