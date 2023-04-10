#include <stdlib.h>
#include <string.h>
#include <tonc.h>
#include "constants.h"
#include "fieldOfVision.h"
#include "gameMap.h"
#include "globals.h"
#include "mgba.h"

//------------------------------------------------------------------
// Data Structures
//------------------------------------------------------------------
extern struct Tile gameMap[MAP_HEIGHT_TILES][MAP_WIDTH_TILES];

//------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------
void checkLOS(int x1, int y1, int const x2, int const y2);
void drawFOV(int positionX, int positionY);
void resetFOV();

//------------------------------------------------------------------
// Function: checkLOS
// 
// Uses Bresenham's algorithm to step through a line from (x1, y1)
// to (x2, y2). At each step, it sets the sightId of the tile.
//------------------------------------------------------------------
void checkLOS(int x1, int y1, int const x2, int const y2)
{
    int dx =  ABS(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -ABS(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2; // error value e_xy
 
    for (;;)
    {
        if (!isOutOfBounds(x1, y1))
        {
            #ifdef DEBUG_FOV
                mgba_printf(MGBA_LOG_DEBUG, "checkLOS (%d, %d)", x1, y1);
            #endif

            gameMap[y1][x1].sightId = playerSightId;
        }

        if ((x1 == x2 && y1 == y2) || isSolid(x1, y1)
        // If NOT a horizontal or vertical line and is crossing a
        // diagonal between two solid tiles
        || ((x1 != x2 && y1 != y2) && (isSolid(x1 + sx, y1) && isSolid(x1, y1 +sy))))
        {
            // Exit loop
            break;
        }

        e2 = 2 * err;
        if (e2 >= dy)
        {
            err += dy;
            x1 += sx;
        } // e_xy+e_x > 0

        if (e2 <= dx)
        {
            err += dx;
            y1 += sy;
        } // e_xy+e_y < 0
    }
}

//------------------------------------------------------------------
// Function: drawFOV
// 
// Updates field-of-vision background layer by copying the correct
// 8x8 graphic based on tile.sightId.
//------------------------------------------------------------------
void drawFOV(int positionX, int positionY)
{
    int screenEntryTL = 0;                     // screenEntryTopLeft
    int screenEntryTR = 0;                    // screenEntryTopRight
    int screenEntryBL = 0;                  // screenEntryBottomLeft
    int screenEntryBR = 0;                 // screenEntryBottomRight
    int *tileToDraw = NULL;
    int originTileX = 0, originTileY = 0;
    int currentTileX = 0, currentTileY = 0;
    
    // Get coordinates of screen entry origin
    switch (getMapSector(positionX, positionY))
    {
    case SECTOR_TOP_LEFT:                         // SECTOR_TOP_LEFT
        originTileX = 0;
        originTileY = 0;
        break;
    case SECTOR_MID_LEFT:                         // SECTOR_MID_LEFT
        originTileX = 0;
        originTileY = positionY - (SCREEN_HEIGHT_TILES / 2 - 1);
        break;
    case SECTOR_BOT_LEFT:                         // SECTOR_BOT_LEFT
        originTileX = 0;
        originTileY = MAP_HEIGHT_TILES - (SCREEN_HEIGHT_TILES - 1);
        break;
    case SECTOR_TOP_MID:                           // SECTOR_TOP_MID
        originTileX = positionX - SCREEN_WIDTH_TILES / 2;
        originTileY = 0;
        break;
    case SECTOR_MID_MID:                           // SECTOR_MID_MID
        originTileX = positionX - SCREEN_WIDTH_TILES / 2;
        originTileY = positionY - (SCREEN_HEIGHT_TILES / 2 - 1);
        break;
    case SECTOR_BOT_MID:                           // SECTOR_BOT_MID
        originTileX = positionX - SCREEN_WIDTH_TILES / 2;
        originTileY = MAP_HEIGHT_TILES - (SCREEN_HEIGHT_TILES - 1);
        break;
    case SECTOR_TOP_RIGHT:                       // SECTOR_TOP_RIGHT
        originTileX = MAP_WIDTH_TILES - SCREEN_WIDTH_TILES;
        originTileY = 0;
        break;
    case SECTOR_MID_RIGHT:                       // SECTOR_MID_RIGHT
        originTileX = MAP_WIDTH_TILES - SCREEN_WIDTH_TILES;
        originTileY = positionY - (SCREEN_HEIGHT_TILES / 2 - 1);
        break;
    case SECTOR_BOT_RIGHT:                       // SECTOR_BOT_RIGHT
        originTileX = MAP_WIDTH_TILES - SCREEN_WIDTH_TILES;
        originTileY = MAP_HEIGHT_TILES - (SCREEN_HEIGHT_TILES - 1);
        break;
    default:
    }

    currentTileX = originTileX;
    currentTileY = originTileY;
    for (int y = 0; y < SCREEN_HEIGHT_TILES; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH_TILES; x++)
        {
            screenEntryTL = y * SCREEN_BLOCK_SIZE * 2 + x * 2 + SCREEN_BLOCK_SIZE * 2;
            screenEntryTR = screenEntryTL + 1;
            screenEntryBL = screenEntryTL + SCREEN_BLOCK_SIZE;
            screenEntryBR = screenEntryBL + 1;

            // Get tileset index of tile to draw
            if (gameMap[currentTileY][currentTileX].sightId == playerSightId)
                tileToDraw = (int*)TRANSPARENT;
            else
                tileToDraw = (int*)FOV_TINT_DARK;

            // Copy the 8x8 tile into map memory
            memcpy(&se_mem[FOV_SB][screenEntryTL], &tileToDraw, 2);
            memcpy(&se_mem[FOV_SB][screenEntryTR], &tileToDraw, 2);
            memcpy(&se_mem[FOV_SB][screenEntryBL], &tileToDraw, 2);
            memcpy(&se_mem[FOV_SB][screenEntryBR], &tileToDraw, 2);

            currentTileX++;
        }
        currentTileX = originTileX;
        currentTileY++;
    }
    #ifdef DEBUG_FOV
        mgba_printf(MGBA_LOG_INFO, "FOV drawn");
    #endif
}

//------------------------------------------------------------------
// Function: resetFOV
// 
// In the event that variable playerSight is going to overflow, this
// function sets it to 2 and sets all tiles' sightId back to 1
//------------------------------------------------------------------
void resetFOV()
{
    // Check that playerSightId is about to overflow
    if (playerSightId == 255)
    {
        // Set all tile's sightId back to TILE_NOT_IN_SIGHT(1)
        for (int y = 0; y < MAP_HEIGHT_TILES; y++)
        {
            for (int x = 0; x < MAP_WIDTH_TILES; x++)
            {
                // Don't make TILE_NEVER_SEEN(0) suddenly visible
                if (gameMap[y][x].sightId != TILE_NEVER_SEEN)
                    gameMap[y][x].sightId = TILE_NOT_IN_SIGHT;
            }
        }

        // Set playerSightId to new lowest value: TILE_IN_SIGHT(2)
        playerSightId = TILE_IN_SIGHT;

        #ifdef DEBUG_FOV
            mgba_printf(MGBA_LOG_INFO, "FOV reset");
        #endif
    }
}

//------------------------------------------------------------------
// Function: initFOV
// 
// Initialize field-of-vision background layer by filling with black
// 8x8 graphic tiles.
//------------------------------------------------------------------
void initFOV()
{
    int *tileToDraw = (int*)FOV_TINT_DARK;

    for (int y = 0; y < SCREEN_BLOCK_SIZE; y++)
    {
        for (int x = 0; x < SCREEN_BLOCK_SIZE; x++)
        {
            memcpy(&se_mem[FOV_SB][y * SCREEN_BLOCK_SIZE + x], &tileToDraw, 2);
        }
    }
}

//------------------------------------------------------------------
// Function: doFOV
// 
// Performs a line-of-sight check on every bounding tile at the edge
// of the player's sight range.
//------------------------------------------------------------------
void doFOV(int const positionX, int const positionY, int const sightRange)
{
    resetFOV();

    // Top Boundary
    for (int x = positionX - sightRange; x <= positionX + sightRange; x++)
        checkLOS(positionX, positionY, x, positionY - sightRange);

    // Bottom Boundary
    for (int x = positionX - sightRange; x <= positionX + sightRange; x++)
        checkLOS(positionX, positionY, x, positionY + sightRange);

    // Left Boundary
    for (int y = positionY - sightRange; y <= positionY + sightRange; y++)
        checkLOS(positionX, positionY, positionX - sightRange, y);

    // Right Boundary
    for (int y = positionY - sightRange; y <= positionY + sightRange; y++)
        checkLOS(positionX, positionY, positionX + sightRange, y);

    drawFOV(positionX, positionY);
}
