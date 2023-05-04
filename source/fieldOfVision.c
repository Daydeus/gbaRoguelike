#include <stdlib.h>
#include <string.h>
#include "../libtonc/include/tonc.h"
#include "constants.h"
#include "debug.h"
#include "entity.h"
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
        #ifdef DEBUG_LOS
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
static void drawFOV(int playerX, int playerY)
{
    int screenEntryTL = 0;                     // screenEntryTopLeft
    int *tileToDraw = NULL;
    int originTileX = playerX - SCREEN_WIDTH_TILES / 2;
    int originTileY = playerY - SCREEN_HEIGHT_TILES / 2 + 1;
    int currentTileX = originTileX, currentTileY = originTileY;

    for (int y = 0; y < SCREEN_HEIGHT_TILES; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH_TILES; x++)
        {
            screenEntryTL = y * SCREEN_BLOCK_SIZE * 2 + x * 2 + SCREEN_BLOCK_SIZE * 2;

            // Get tileset index of tile to draw
            if (getTileSight(currentTileX, currentTileY) == playerSightId)
                tileToDraw = (int*)TRANSPARENT;
            else
                tileToDraw = (int*)FOV_TINT_DARK;

            // Copy the 8x8 tile into map memory
            memcpy(&se_mem[FOV_SB][screenEntryTL], &tileToDraw, 2);
            memcpy(&se_mem[FOV_SB][screenEntryTL + 1], &tileToDraw, 2);
            memcpy(&se_mem[FOV_SB][screenEntryTL + SCREEN_BLOCK_SIZE], &tileToDraw, 2);
            memcpy(&se_mem[FOV_SB][screenEntryTL + 1 + SCREEN_BLOCK_SIZE], &tileToDraw, 2);

            // Move along the current row
            currentTileX++;
        }
        // Move to the next row
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
extern void doFOV(int const playerX, int const playerY, int const playerSightRange)
{
    // Check that playerSightId is about to overflow
    if (playerSightId == 255)
        resetFOV();

    // Top Boundary
    for (int x = playerX - playerSightRange; x <= playerX + playerSightRange; x++)
        markLOS(playerX, playerY, x, playerY - playerSightRange);

    // Bottom Boundary
    for (int x = playerX - playerSightRange; x <= playerX + playerSightRange; x++)
        markLOS(playerX, playerY, x, playerY + playerSightRange);

    // Left Boundary
    for (int y = playerY - playerSightRange; y <= playerY + playerSightRange; y++)
        markLOS(playerX, playerY, playerX - playerSightRange, y);

    // Right Boundary
    for (int y = playerY - playerSightRange; y <= playerY + playerSightRange; y++)
        markLOS(playerX, playerY, playerX + playerSightRange, y);

    drawFOV(playerX, playerY);
}

//------------------------------------------------------------------
// Function: checkLOS
// 
// Walks through a line using function getTileDirInline (which uses
// Bresenham's algorithm) and returns FALSE if any tiles block sight.
//------------------------------------------------------------------
extern boolean checkLOS(int startX, int startY, int const endX, int const endY)
{
    int currentX = startX, currentY = startY;
    enum direction direction = DIR_NULL;

    while(1)
    {
        #ifdef DEBUG_LOS
            mgba_printf(MGBA_LOG_DEBUG, "checkLOS (%d, %d)", currentX, currentY);
        #endif

        // Ensure that line-of-sight doesn't cross diagonally through walls
        if (direction == DIR_UP_LEFT && !isSolid(currentX, currentY)
        && isSolid(currentX + dirX[DIR_RIGHT], currentY) && isSolid(currentX, currentY + dirY[DIR_DOWN]))
            return FALSE;
        else if (direction == DIR_UP_RIGHT && !isSolid(currentX, currentY)
        && isSolid(currentX + dirX[DIR_LEFT], currentY) && isSolid(currentX, currentY + dirY[DIR_DOWN]))
            return FALSE;
        if (direction == DIR_DOWN_LEFT && !isSolid(currentX, currentY)
        && isSolid(currentX + dirX[DIR_RIGHT], currentY) && isSolid(currentX, currentY + dirY[DIR_UP]))
            return FALSE;
        if (direction == DIR_DOWN_RIGHT && !isSolid(currentX, currentY)
        && isSolid(currentX + dirX[DIR_LEFT], currentY) && isSolid(currentX, currentY + dirY[DIR_UP]))
            return FALSE;

        // Check the non-diagonal conditions for ending loop
        if (isSolid(currentX, currentY))
            return FALSE;
        else if (currentX == endX && currentY == endY)
            break;

        // Make the next tile on the line the current one
        direction = getTileDirInLine(currentX, currentY, endX, endY);
        currentX += dirX[direction];
        currentY += dirY[direction];
    }
    // Should only return TRUE if loop reached end position without being solid
    return TRUE;
}
