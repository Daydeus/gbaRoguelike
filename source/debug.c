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

//------------------------------------------------------------------
// Function: printTileSightInLog
//
// Prints an ascii representation of the gameMap, including the player's
// current position, in the mgba console but replaces the terrainIds
// with the tile's sight status.
//------------------------------------------------------------------
extern void printTileSightInLog()
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
                switch (getTile(x, y)->sightId)
                {
                case 0:
                    strcat(mapRow, "0");
                    break;
                case 1:
                    strcat(mapRow, "1");
                    break;
                case 2:
                    strcat(mapRow, "2");
                    break;
                case 3:
                    strcat(mapRow, "3");
                    break;
                case 4:
                    strcat(mapRow, "4");
                    break;
                case 5:
                    strcat(mapRow, "5");
                    break;
                case 6:
                    strcat(mapRow, "6");
                    break;
                case 7:
                    strcat(mapRow, "7");
                    break;
                case 8:
                    strcat(mapRow, "8");
                    break;
                case 9:
                    strcat(mapRow, "9");
                    break;
                default:
                    strcat(mapRow, "~");
                    break;
                }
            }

        }
        mgba_printf(MGBA_LOG_INFO, mapRow);
    }
}
