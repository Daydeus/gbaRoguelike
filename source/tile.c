#include <tonc.h>
#include <string.h>
#include "constants.h"
#include "globals.h"
#include "mapGeneration.h"
#include "mgba.h"
#include "tile.h"
#include "tileset_stone.h"
#include "tilemap_stone.h"

//------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------
static void drawGameMapScreenEntry(struct Tile* const tile);
static int getScreenBlock(int const positionX, int const positionY);
static int getScreenEntryTL(int const positionX, int const positionY, int const screenBlock);
static int* getTilesetIndex(struct Tile* const tile, uint8_t const screenEntryCorner);
static uint8_t getDynamicTerrainId(struct Tile* const tile);
static uint8_t getNumberNeighborsOfType(int const positionX, int const positionY, int const terrainId);
static struct Tile* getRandomTileOfType(uint8_t const terrainId);

//------------------------------------------------------------------
// Function: drawGameMapScreenEntry
// 
// Update the four screen entries of a given tile in the gameMap[][].
//------------------------------------------------------------------
static void drawGameMapScreenEntry(struct Tile* tile)
{
    // Find where (and on which screen block) to draw the tile
    int screenBlock = getScreenBlock(tile->posX, tile->posY);
    int screenEntryTL = getScreenEntryTL(tile->posX, tile->posY, screenBlock); // screenEntryTopLeft
    int screenEntryTR = screenEntryTL + 1;                                    // screenEntryTopRight
    int screenEntryBL = screenEntryTL + SCREEN_BLOCK_SIZE;                  // screenEntryBottomLeft
    int screenEntryBR = screenEntryBL + 1;                                 // screenEntryBottomRight
    int *tileToDraw = NULL;

    // Get and copy the 8x8 tile into map memory
    tileToDraw = getTilesetIndex(tile, SCREEN_ENTRY_TL);
    memcpy(&se_mem[screenBlock][screenEntryTL], &tileToDraw, 2);

    tileToDraw = getTilesetIndex(tile, SCREEN_ENTRY_TR);
    memcpy(&se_mem[screenBlock][screenEntryTR], &tileToDraw, 2);

    tileToDraw = getTilesetIndex(tile, SCREEN_ENTRY_BL);
    memcpy(&se_mem[screenBlock][screenEntryBL], &tileToDraw, 2);

    tileToDraw = getTilesetIndex(tile, SCREEN_ENTRY_BR);
    memcpy(&se_mem[screenBlock][screenEntryBR], &tileToDraw, 2);
}

//------------------------------------------------------------------
// Function: getScreenBlock
// 
// Returns on which screen block a tile should be drawn by using the
// given position.
//------------------------------------------------------------------
static int getScreenBlock(int const positionX, int const positionY)
{
    if (positionX <= MAP_WIDTH_TILES / 2 - 1 && positionY <= MAP_HEIGHT_TILES / 2 - 1)
        return GAME_MAP_SB1;
    else if (positionX > MAP_WIDTH_TILES / 2 - 1 && positionY <= MAP_HEIGHT_TILES / 2 - 1)
        return GAME_MAP_SB2;
    else if (positionX <= MAP_WIDTH_TILES / 2 - 1 && positionY > MAP_HEIGHT_TILES / 2 - 1)
        return GAME_MAP_SB3;
    else
        return GAME_MAP_SB4;
}

//------------------------------------------------------------------
// Function: getScreenEntryTL
// 
// Returns where the top-left screen entry of a tile at the given
// position, within the given screen block, should be drawn.
//------------------------------------------------------------------
static int getScreenEntryTL(int const positionX, int const positionY, int const screenBlock)
{
    int screenEntry = 0;

    switch (screenBlock)
    {
    case GAME_MAP_SB1: screenEntry = 2* (positionY * SCREEN_BLOCK_SIZE + positionX); break;
    case GAME_MAP_SB2: screenEntry = 2* (positionY * SCREEN_BLOCK_SIZE + positionX) - SCREEN_BLOCK_SIZE; break;
    case GAME_MAP_SB3: screenEntry = 2* (positionY * SCREEN_BLOCK_SIZE + positionX) - SCREEN_BLOCK_SIZE*SCREEN_BLOCK_SIZE; break;
    case GAME_MAP_SB4: screenEntry = 2* (positionY * SCREEN_BLOCK_SIZE + positionX) - SCREEN_BLOCK_SIZE*(SCREEN_BLOCK_SIZE + 1); break;
    default:
    }

    return screenEntry;
}

//------------------------------------------------------------------
// Function: getTilesetIndex
// 
// Uses a given tile's terrainId and screenEntryCorner to pick the
// index of the correct 8x8 bitmap and returns a pointer to the
// bitmap so it may be drawn.
//------------------------------------------------------------------
static int* getTilesetIndex(struct Tile* const tile, uint8_t const screenEntryCorner)
{
    // Tile corners are arranged sequentially in memory.
    // Corner Top-Left, Top-Right, Bottom-Left, Bottom-Right
    uint8_t tileSubId = getDynamicTerrainId(tile);
    int *tilesetIndex = NULL;

    // If player has never seen tile, tile's terrain doesn't matter
    if (tile->sightId == TILE_NEVER_SEEN && debugMapIsVisible == false)
        return (int*)BLANK_BLACK;

    switch (tileSubId)
    {
    case ID_FLOOR:                                       // ID_FLOOR
        switch (screenEntryCorner)
        {
        case SCREEN_ENTRY_TL:
            tilesetIndex = (int*)FLOOR_FOUR_TL;      break;
        case SCREEN_ENTRY_TR:
            tilesetIndex = (int*)FLOOR_FOUR_TR;      break;
        case SCREEN_ENTRY_BL:
            tilesetIndex = (int*)FLOOR_FOUR_BL;      break;
        case SCREEN_ENTRY_BR:
            tilesetIndex = (int*)FLOOR_FOUR_BR;      break;
        default:                                     break;
        }
        break;
    case ID_FLOOR_BIG:                               // ID_FLOOR_BIG
        switch (screenEntryCorner)
        {
        case SCREEN_ENTRY_TL:
            tilesetIndex = (int*)FLOOR_BIG_TL;       break;
        case SCREEN_ENTRY_TR:
            tilesetIndex = (int*)FLOOR_BIG_TR;       break;
        case SCREEN_ENTRY_BL:
            tilesetIndex = (int*)FLOOR_BIG_BL;       break;
        case SCREEN_ENTRY_BR:
            tilesetIndex = (int*)FLOOR_BIG_BR;       break;
        default:                                     break;
        }
        break;
    case ID_FLOOR_MOSSY:                           // ID_FLOOR_MOSSY
        switch (screenEntryCorner)
        {
        case SCREEN_ENTRY_TL:
            tilesetIndex = (int*)FLOOR_MOSSY;        break;
        case SCREEN_ENTRY_TR:
            tilesetIndex = (int*)FLOOR_MOSSY_2;      break;
        case SCREEN_ENTRY_BL:
            tilesetIndex = (int*)FLOOR_MOSS_CHIP;    break;
        case SCREEN_ENTRY_BR:
            tilesetIndex = (int*)FLOOR_V_MOSS_CHIP;  break;
        default:
            break;
        }
        break;
    case ID_FLOOR_CHIP:                             // ID_FLOOR_CHIP
        switch (screenEntryCorner)
        {
        case SCREEN_ENTRY_TL:
            tilesetIndex = (int*)FLOOR_FOUR_TR;      break;
        case SCREEN_ENTRY_TR:
            tilesetIndex = (int*)FLOOR_CHIP;         break;
        case SCREEN_ENTRY_BL:
            tilesetIndex = (int*)FLOOR_V_CHIP;       break;
        case SCREEN_ENTRY_BR:
            tilesetIndex = (int*)FLOOR_FOUR_BR;      break;
        default:                                     break;
        }
        break;
    case ID_WALL:                                         // ID_WALL
        switch (screenEntryCorner)
        {
        case SCREEN_ENTRY_TL:
            tilesetIndex = (int*)WALL_TOP_TL;        break;
        case SCREEN_ENTRY_TR:
            tilesetIndex = (int*)WALL_TOP_TR;        break;
        case SCREEN_ENTRY_BL:
            tilesetIndex = (int*)WALL_TOP_BL;        break;
        case SCREEN_ENTRY_BR:
            tilesetIndex = (int*)WALL_TOP_BR;        break;
        default:                                     break;
        }
        break;
    case ID_WALL_FRONT:                             // ID_WALL_FRONT
        switch (screenEntryCorner)
        {
        case SCREEN_ENTRY_TL:
            tilesetIndex = (int*)WALL_FRONT_TL;      break;
        case SCREEN_ENTRY_TR:
            tilesetIndex = (int*)WALL_FRONT_TR;      break;
        case SCREEN_ENTRY_BL:
            tilesetIndex = (int*)WALL_FRONT_BL;      break;
        case SCREEN_ENTRY_BR:
            tilesetIndex = (int*)WALL_FRONT_BR;      break;
        default:                                     break;
        }
        break;
    case ID_STAIRS:                                     // ID_STAIRS
        switch (screenEntryCorner)
        {
        case SCREEN_ENTRY_TL:
            tilesetIndex = (int*)STAIRS_TL;          break;
        case SCREEN_ENTRY_TR:
            tilesetIndex = (int*)STAIRS_TR;          break;
        case SCREEN_ENTRY_BL:
            tilesetIndex = (int*)STAIRS_BL;          break;
        case SCREEN_ENTRY_BR:
            tilesetIndex = (int*)STAIRS_BR;          break;
        default:                                     break;
        }
        break;
    default:
        tilesetIndex = (int*)TRANSPARENT;             // TRANSPARENT
        break;
    }

    return tilesetIndex;
}

//------------------------------------------------------------------
// Function: getDynamicTerrainId
// 
// Takes a tile as an input and uses its position to get terrainIds
// for the surrounding tiles and uses that plus its own terrainId to
// determine which variant of tile to return for being drawn.
//------------------------------------------------------------------
static uint8_t getDynamicTerrainId(struct Tile* const tile)
{
    uint8_t terrainId = tile->terrainId;

    switch(terrainId)
    {
    case ID_WALL:
        if (getTileTerrain(tile->posX, tile->posY + 1) != ID_WALL || tile->posY + 1 > MAP_HEIGHT_TILES - 1)
            terrainId = ID_WALL_FRONT;
        break;
    default:
    }

    return terrainId;
}

//------------------------------------------------------------------
// Function: getNumberNeighborsOfType
// 
// Checks each cardinal direction and returns the number of tiles
// that have the given type.
//------------------------------------------------------------------
static uint8_t getNumberNeighborsOfType(int const positionX, int const positionY, int const terrainId)
{
    uint8_t numberNeighbors = 0;

    for (int direction = DIR_LEFT; direction <= DIR_DOWN; direction++)
    {
        if (gameMap[positionY + dirY[direction]][positionX + dirX[direction]].terrainId == terrainId)
            numberNeighbors++;
    }

    return numberNeighbors;
}

//------------------------------------------------------------------
// Function: getRandomTileOfType
// 
// Returns a pointer to a random tile with the given terrainId.
//------------------------------------------------------------------
static struct Tile* getRandomTileOfType(uint8_t const terrainId)
{
    int iterationCount = 0;
    int positionX = randomInRange(1, MAP_WIDTH_TILES - 1);
    int positionY = randomInRange(1, MAP_HEIGHT_TILES - 1);

    while (getTileTerrain(positionX, positionY) != terrainId)
    {
        if (iterationCount > 100)
        {
            #ifdef DEBUG_MAP_GEN
                mgba_printf(MGBA_LOG_DEBUG, "    getRandomTileOfType(%d) returned NULL", terrainId);
            #endif
            return NULL;
        }

        positionX = randomInRange(1, MAP_WIDTH_TILES - 1);
        positionY = randomInRange(1, MAP_HEIGHT_TILES - 1);
        iterationCount++;
    }

    return getTile(positionX, positionY);
}

//------------------------------------------------------------------
// Function: initTilePosition
// 
// Used only to set the tiles' positions to their index in gameMap[][].
//------------------------------------------------------------------
extern void initTilePosition(int const positionX, int const positionY)
{
    gameMap[positionY][positionX].posX = positionX;
    gameMap[positionY][positionX].posY = positionY;
}

//------------------------------------------------------------------
// Function: getTile
// 
// Returns a pointer to the tile at the given position.
//------------------------------------------------------------------
extern struct Tile* getTile(int const positionX, int const positionY)
{
    if (!isOutOfBounds(positionX, positionY))
        return &gameMap[positionY][positionX];
    else
        return NULL;
}

//------------------------------------------------------------------
// Function: getTileTerrain
// 
// Returns the tile terrain at the given position.
//------------------------------------------------------------------
extern uint8_t getTileTerrain(int const positionX, int const positionY)
{
    if (!isOutOfBounds(positionX, positionY))
        return gameMap[positionY][positionX].terrainId;
    else
        return ID_TRANSPARENT;
}

//------------------------------------------------------------------
// Function: setTileTerrain
// 
// Sets the terrain of the tile at the given position.
//------------------------------------------------------------------
extern void setTileTerrain(int const positionX, int const positionY, uint8_t const terrainId)
{
    if (!isOutOfBounds(positionX, positionY))
        gameMap[positionY][positionX].terrainId = terrainId;
}

//------------------------------------------------------------------
// Function: getTileSight
// 
// Returns the sightId of the tile at the given position.
//------------------------------------------------------------------
extern uint8_t getTileSight(int const positionX, const int positionY)
{
    if (!isOutOfBounds(positionX, positionY))
        return gameMap[positionY][positionX].sightId;
    else
        return TILE_NEVER_SEEN;
}

//------------------------------------------------------------------
// Function: setTileSight
// 
// Sets the sightId of the tile at the given position.
//------------------------------------------------------------------
extern void setTileSight(int const positionX, int const positionY, uint8_t const sightId)
{
    if (!isOutOfBounds(positionX, positionY))
        gameMap[positionY][positionX].sightId = sightId;
}

//------------------------------------------------------------------
// Function: getTileDirection
// 
// Given two positions, returns the direction of travel to get from
// the start to the end. Used only for adjacent tiles.
//------------------------------------------------------------------
extern enum direction getTileDirection(int const startX, int const startY, int const endX, int const endY)
{
    enum direction direction = DIR_NULL;
    int distanceX = endX - startX;
    int distanceY = endY - startY;

    // Set direction
    if (distanceY < 0)
    {
        if (distanceX < 0)
            direction = DIR_UP_LEFT;
        else if (distanceX == 0)
            direction = DIR_UP;
        else
            direction = DIR_UP_RIGHT;
    }
    else if (distanceY == 0)
    {
        if (distanceX < 0)
            direction = DIR_LEFT;
        else if (distanceX > 0)
            direction = DIR_RIGHT;
        else
            direction = DIR_NULL;
    }
    else if (distanceY > 0)
    {
        if (distanceX < 0)
            direction = DIR_DOWN_LEFT;
        else if (distanceX == 0)
            direction = DIR_DOWN;
        else
            direction = DIR_DOWN_RIGHT;
    }

    #ifdef DEBUG_MAP_GEN
        mgba_printf(MGBA_LOG_DEBUG, "getTileDirection returned NULL.");
    #endif

    return direction;
}

//------------------------------------------------------------------
// Function: getTileDirInLine
// 
// Given a starting and ending position, calculate the line between
// the two using Bresenham's algorithm and return the direction of
// the next tile along the line.
//
// Call in a loop to traverse and perform an action along the whole line.
//------------------------------------------------------------------
enum direction getTileDirInLine(int startX, int startY, int const endX, int const endY)
{
    int changeInX =  ABS(endX - startX);
    int changeInY = -ABS(endY - startY);
    int stepOfX = startX < endX ? dirX[DIR_RIGHT] : dirX[DIR_LEFT];
    int stepOfY = startY < endY ? dirY[DIR_DOWN] : dirY[DIR_UP];
    int err = changeInX + changeInY, e2; // error value e_xy
    int nextX = startX, nextY = startY;

    e2 = 2 * err;

    // Move along x-axis
    if (e2 >= changeInY)
    {
        err += changeInY;
        nextX += stepOfX;
    } // e_xy+e_x > 0

    // Move along y-axis
    if (e2 <= changeInX)
    {
        err += changeInX;
        nextY += stepOfY;
    } // e_xy+e_y < 0

    return getTileDirection(startX, startY, nextX, nextY);
}

//------------------------------------------------------------------
// Function: loadGameMap
// 
// Calls function drawGameMapScreenEntry for each tile in the gameMap.
//------------------------------------------------------------------
extern void loadGameMap()
{
    // Load palette
    memcpy(pal_bg_mem, tileset_stonePal, tileset_stonePalLen);

    // Load tiles into CBB 0
    memcpy(&tile_mem[0][0], tileset_stoneTiles, tileset_stoneTilesLen);

    // Copy each tile's graphic to the appropriate screen entry
    for (int y = 0; y <= MAP_HEIGHT_TILES - 1; y++)
    {
        for (int x = 0; x <= MAP_WIDTH_TILES - 1; x++)
        {
            drawGameMapScreenEntry(getTile(x, y));
        }
    }

    #ifdef DEBUG_MAP_GEN
        mgba_printf(MGBA_LOG_INFO, "loadGameMap");
    #endif
}

//------------------------------------------------------------------
// Function: updateGameMapSight
// 
// Redraws all tiles within sightRange of the player. Used for updating
// whether a tile has been seen before.
//------------------------------------------------------------------
extern void updateGameMapSight(int const playerX, int const playerY)
{
    // Only check tiles the player could possibly see
    for (int y = playerY - sightRange; y <= playerY + sightRange; y++)
    {
        for (int x = playerX - sightRange; x <= playerX + sightRange; x++)
        {
            // Only update tiles that need it
            if (getTileSight(x, y) == playerSightId || getTileSight(x, y) == playerSightId - 1)
            {
                drawGameMapScreenEntry(getTile(x, y));

                #ifdef DEBUG_FOV
                    mgba_printf(MGBA_LOG_DEBUG, "updateGameMapSight: (%d, %d)", x, y);
                #endif
            }
        }
    }
}

//------------------------------------------------------------------
// Function: isOutOfBounds
// 
// Returns whether the tile at the given position is out of bounds
// for the player or not.
//------------------------------------------------------------------
extern bool isOutOfBounds(uint8_t const positionX, uint8_t const positionY)
{
    if (positionX < 0 || positionX > MAP_WIDTH_TILES - 1
    || positionY < 0 || positionY > MAP_HEIGHT_TILES - 1)
        return true;
    else
        return false;
}

//------------------------------------------------------------------
// Function: isSolid
// 
// Returns whether the tile at the given position is solid or not.
// NOTE: Solid tiles should block player movement.
//------------------------------------------------------------------
extern bool isSolid(uint8_t const positionX, uint8_t const positionY)
{
    if(getTileTerrain(positionX, positionY) != ID_WALL)
        return false;
    else if (debugCollisionIsOff == true)
        return false;
    else
        return true;
}

//------------------------------------------------------------------
// Function: getMapSector
// 
// Check which mapSector the position is in for determining
// whether to offset the background scroll or player sprite
//------------------------------------------------------------------
extern uint8_t getMapSector(int const positionX, int const positionY)
{
    if (4 >= positionY)
    {
        if (7 >= positionX)
            return SECTOR_TOP_LEFT;
        else if (MAP_WIDTH_TILES - 8 <= positionX)
            return SECTOR_TOP_RIGHT;
        else
            return SECTOR_TOP_MID;
    }
    else if (MAP_HEIGHT_TILES - 5 <= positionY)
    {
        if (7 >= positionX)
            return SECTOR_BOT_LEFT;
        else if (MAP_WIDTH_TILES - 8 <= positionX)
            return SECTOR_BOT_RIGHT;
        else
            return SECTOR_BOT_MID;
    }
    else
    {
        if (7 >= positionX)
            return SECTOR_MID_LEFT;
        else if (MAP_WIDTH_TILES - 8 <= positionX)
            return SECTOR_MID_RIGHT;
        else
            return SECTOR_MID_MID;
    }

    return SECTOR_ERROR;
}
