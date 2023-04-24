#include <stdlib.h>
#include <string.h>
#include <tonc.h>
#include "constants.h"
#include "debug.h"
#include "globals.h"
#include "mgba.h"
#include "tile.h"

//------------------------------------------------------------------
// Function: printMapInLog
//
// Prints an ascii representation of the gameMap, including the player's
// current position, in the mgba console.
//------------------------------------------------------------------
extern void printMapInLog()
{
    for (int y = 0; y < MAP_HEIGHT_TILES; y++)
    {
        char mapRow[MAP_WIDTH_TILES + 1];
        for (int c = 0; c < MAP_WIDTH_TILES + 1; c++)
            mapRow[c] = '\0';

        for (int x = 0; x < MAP_WIDTH_TILES; x++)
        {
            if (playerX == x && playerY == y)
                strcat(mapRow, "@");
            else
            {
                switch (getTile(x, y)->terrainId)
                {
                case ID_FLOOR:
                case ID_FLOOR_MOSSY:
                case ID_FLOOR_BIG:
                case ID_FLOOR_CHIP:
                    strcat(mapRow, ".");
                    break;
                case ID_WALL:
                case ID_WALL_FRONT:
                    strcat(mapRow, "#");
                    break;
                case ID_STAIRS:
                    strcat(mapRow, "S");
                    break;
                default:
                    strcat(mapRow, "?");
                    break;
                }
            }

        }
        mgba_printf(MGBA_LOG_INFO, mapRow);
    }
}