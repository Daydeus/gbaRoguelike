#include <stdlib.h>
#include <string.h>
#include <tonc.h>
#include "constants.h"
#include "debug.h"
#include "fieldOfVision.h"
#include "globals.h"
#include "mgba.h"
#include "tile.h"

//------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------
static void markLOS(int startX, int startY, int const endX, int const endY);
static void drawFOV(int positionX, int positionY);
static void resetFOV();

//------------------------------------------------------------------
// Function: markLOS
// 
// Walks through a line using function getTileDirInline (which uses
// Bresenham's algorithm) and marks all applicable tiles as in LOS.
//
// Used only for showing the tiles visible to the player, as non-marked
// tiles are shadow-blended.
//------------------------------------------------------------------
static void markLOS(int startX, int startY, int const endX, int const endY)
{
    int currentX = startX, currentY = startY;
    enum direction direction = DIR_NULL;

    while(1)
    {
        #ifdef DEBUG_FOV
            mgba_printf(MGBA_LOG_DEBUG, "markLOS (%d, %d)", currentX, currentY);
        #endif

        // Checks if we moved diagonally along the line, and if the current tile is non-solid but the
        // tiles to either side of the diagonal ARE solid, exit loop
        if (direction == DIR_UP_LEFT && !isSolid(currentX, currentY)
        && isSolid(currentX + dirX[DIR_RIGHT], currentY) && isSolid(currentX, currentY + dirY[DIR_DOWN]))
            break;
        else if (direction == DIR_UP_RIGHT && !isSolid(currentX, currentY)
        && isSolid(currentX + dirX[DIR_LEFT], currentY) && isSolid(currentX, currentY + dirY[DIR_DOWN]))
            break;
        if (direction == DIR_DOWN_LEFT && !isSolid(currentX, currentY)
        && isSolid(currentX + dirX[DIR_RIGHT], currentY) && isSolid(currentX, currentY + dirY[DIR_UP]))
            break;
        if (direction == DIR_DOWN_RIGHT && !isSolid(currentX, currentY)
        && isSolid(currentX + dirX[DIR_LEFT], currentY) && isSolid(currentX, currentY + dirY[DIR_UP]))
            break;
        else // Mark the current tile as visible(in line-of-sight)
            setTileSight(currentX, currentY, playerSightId);

        // Check the non-diagonal conditions for ending loop
        if ((currentX == endX && currentY == endY) || isSolid(currentX, currentY))
            break;

        // Make the next tile on the line the current one
        direction = getTileDirInLine(currentX, currentY, endX, endY);
        currentX += dirX[direction];
        currentY += dirY[direction];
    }
}

//------------------------------------------------------------------
// Function: drawFOV
// 
// Updates field-of-vision background layer by copying the correct
// 8x8 graphic based on tile.sightId.
//------------------------------------------------------------------
static void drawFOV(int positionX, int positionY)
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
            if (getTileSight(currentTileX, currentTileY) == playerSightId)
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
static void resetFOV()
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
                if (getTileSight(x, y) != TILE_NEVER_SEEN)
                    setTileSight(x, y, TILE_NOT_IN_SIGHT);
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
extern void initFOV()
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
extern void doFOV(int const positionX, int const positionY, int const sightRange)
{
    resetFOV();

    // Top Boundary
    for (int x = positionX - sightRange; x <= positionX + sightRange; x++)
        markLOS(positionX, positionY, x, positionY - sightRange);

    // Bottom Boundary
    for (int x = positionX - sightRange; x <= positionX + sightRange; x++)
        markLOS(positionX, positionY, x, positionY + sightRange);

    // Left Boundary
    for (int y = positionY - sightRange; y <= positionY + sightRange; y++)
        markLOS(positionX, positionY, positionX - sightRange, y);

    // Right Boundary
    for (int y = positionY - sightRange; y <= positionY + sightRange; y++)
        markLOS(positionX, positionY, positionX + sightRange, y);

    drawFOV(positionX, positionY);
}

//------------------------------------------------------------------
// Function: checkLOS
// 
// Walks through a line using function getTileDirInline (which uses
// Bresenham's algorithm) and returns false if any tiles block sight.
//------------------------------------------------------------------
extern bool checkLOS(int startX, int startY, int const endX, int const endY)
{
    int currentX = startX, currentY = startY;
    enum direction direction = DIR_NULL;

    while(1)
    {
        #ifdef DEBUG_FOV
            mgba_printf(MGBA_LOG_DEBUG, "checkLOS (%d, %d)", currentX, currentY);
        #endif

        // Ensure that line-of-sight doesn't cross diagonally through walls
        if (direction == DIR_UP_LEFT && !isSolid(currentX, currentY)
        && isSolid(currentX + dirX[DIR_RIGHT], currentY) && isSolid(currentX, currentY + dirY[DIR_DOWN]))
            return false;
        else if (direction == DIR_UP_RIGHT && !isSolid(currentX, currentY)
        && isSolid(currentX + dirX[DIR_LEFT], currentY) && isSolid(currentX, currentY + dirY[DIR_DOWN]))
            return false;
        if (direction == DIR_DOWN_LEFT && !isSolid(currentX, currentY)
        && isSolid(currentX + dirX[DIR_RIGHT], currentY) && isSolid(currentX, currentY + dirY[DIR_UP]))
            return false;
        if (direction == DIR_DOWN_RIGHT && !isSolid(currentX, currentY)
        && isSolid(currentX + dirX[DIR_LEFT], currentY) && isSolid(currentX, currentY + dirY[DIR_UP]))
            return false;

        // Check the non-diagonal conditions for ending loop
        if (isSolid(currentX, currentY))
            return false;
        else if (currentX == endX && currentY == endY)
            break;

        // Make the next tile on the line the current one
        direction = getTileDirInLine(currentX, currentY, endX, endY);
        currentX += dirX[direction];
        currentY += dirY[direction];
    }
    // Should only return true if loop reached end position without being solid
    return true;
}
