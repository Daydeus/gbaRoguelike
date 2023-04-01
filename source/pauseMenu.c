#include <tonc.h>
#include <string.h>
#include "constants.h"
#include "gameMap.h"
#include "globals.h"
#include "pauseMenu.h"

/******************************************************************/
/* Function Prototypes                                            */
/******************************************************************/
void tte_write_var_int(int varToPrint);
void getTileStatusDebug(int positionX, int positionY);

/******************************************************************/
/* Function: doPauseMenuInput                                     */
/*                                                                */
/* Changes variable values by reading keys pressed with key_poll()*/
/* Only used while in STATE_MENU                                  */
/******************************************************************/
bool doPauseMenuInput()
{
    if (KEY_EQ(key_hit, KI_A))
    {
        debugCollisionIsOff = true;;
        return true;
    }
    if (KEY_EQ(key_hit, KI_B))
    {
        debugCollisionIsOff = false;
        return true;
    }
    if (KEY_EQ(key_hit, KI_LEFT))
    {
        eva -= 20;
        return true;
    }
    if (KEY_EQ(key_hit, KI_RIGHT))
    {
        eva += 20;
        return true;
    }
    if (KEY_EQ(key_hit, KI_UP))
    {
        evb += 20;
        return true;
    }
    if (KEY_EQ(key_hit, KI_DOWN))
    {
        evb -= 20;
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

/******************************************************************/
/* Function: drawPauseMenu                                        */
/*                                                                */
/* Prints the menu's text as 8x8 graphical tiles into the menu's  */
/* screen entries                                                 */
/******************************************************************/
void drawPauseMenu()
{
    tte_erase_screen();
    tte_set_pos(0, 0);
    tte_write("Pause Menu\n");
    tte_write("Player position (");
    tte_write_var_int(playerX);
    tte_write(", ");
    tte_write_var_int(playerY);
    tte_write(")\n");
    tte_write("eva: ");
    tte_write_var_int(eva);
    tte_write("\nevb: ");
    tte_write_var_int(evb);
    tte_write("\nCollision: ");
    (debugCollisionIsOff == true) ? tte_write("OFF") : tte_write("ON");
    tte_write("\nTile Status: ");
    getTileStatusDebug(playerX, playerY);
}

/******************************************************************/
/* Function: doStateTransition                                    */
/*                                                                */
/* Sets the correct graphics mode for the new gameState and any   */
/* necessary variable values.                                     */
/******************************************************************/
void doStateTransition(enum state targetState)
{
    switch(targetState)
    {
    case STATE_GAMEPLAY:
        REG_BG0CNT= BG_CBB(0) | BG_SBB(GAME_HUD_SB) | BG_4BPP | BG_REG_32x32;
        REG_BG2CNT= BG_CBB(0) | BG_SBB(GAME_MAP_SB1) | BG_4BPP | BG_REG_64x32;
        REG_DISPCNT= DCNT_MODE0 | DCNT_BG0 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;
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

/******************************************************************/
/* Function: getTileStatusDebug                                   */
/*                                                                */
/* At the given position, prints a message corresponding to the   */
/* tile's seen/lit status.                                        */
/* TODO: Implement tile seen/lit status.                          */
/******************************************************************/
void getTileStatusDebug(int positionX, int positionY)
{
    switch(gameMap[positionY][positionX].sightStatus)
    {
    case TILE_NEVER_SEEN:
        tte_write("NEVER SEEN\n");
        break;
    case TILE_NOT_IN_SIGHT:
        tte_write("NOT IN SIGHT\n");
        break;
    case TILE_SEEN_NOT_LIT:
        tte_write("SEEN NOT LIT\n");
        break;
    case TILE_LIT:
        tte_write("LIT\n");
        break;
    }
}

/******************************************************************/
/* Function: tte_write_var_int                                    */
/*                                                                */
/* Tonc's Text Engine: write variable integers                    */
/* Prints as 8x8 graphical tiles each digit of the given number.  */
/* Used in the debug menu to print variable integers.             */
/******************************************************************/
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
