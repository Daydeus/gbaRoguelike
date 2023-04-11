#include <tonc.h>
#include <string.h>
#include "constants.h"
#include "globals.h"
#include "gameMap.h"
#include "mapGeneration.h"
#include "mgba.h"
#include "tileset_stone.h"
#include "tilemap_stone.h"

//------------------------------------------------------------------
// Data Structures
//------------------------------------------------------------------
extern struct Tile gameMap[MAP_HEIGHT_TILES][MAP_WIDTH_TILES];

//------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------
uint8_t getDynamicTerrainId(struct Tile* const tile);
int* getTilesetIndex(struct Tile* const tile, uint8_t const screenEntryCorner);
u8 getNumberNeighborsOfType(int const positionX, int const positionY, int const terrainId);
struct Tile* getRandomTileOfType(uint8_t const terrainId);
void drawGameMapScreenEntry(struct Tile* const tile);

//------------------------------------------------------------------
// Function: getDynamicTerrainId
// 
// Takes a tile as an input and uses its position to get terrainIds
// for the surrounding tiles and uses that plus its own terrainId to
// determine which variant of tile to return for being drawn.
//------------------------------------------------------------------
uint8_t getDynamicTerrainId(struct Tile* const tile)
{
    uint8_t terrainId = tile->terrainId;

    switch(terrainId)
    {
    case ID_WALL:
        if(gameMap[tile->posY + 1][tile->posX].terrainId != ID_WALL || tile->posY + 1 > MAP_HEIGHT_TILES -1)
            terrainId = ID_WALL_FRONT;
        break;
    default:
    }
    return terrainId;
}

//------------------------------------------------------------------
// Function: getTilesetIndex
// 
// Matches a given tile's terrainId and screenEntryCorner to pick the
// index of the correct 8x8 bitmap and returns a pointer to the
// bitmap so it may be drawn.
//------------------------------------------------------------------
int* getTilesetIndex(struct Tile* const tile, uint8_t const screenEntryCorner)
{
    // Tile corners are arranged sequentially in memory.
    // Corner Top-Left, Top-Right, Bottom-Left, Bottom-Right
    uint8_t tileSubId = getDynamicTerrainId(tile);
    int *tilesetIndex = NULL;

    if (tile->sightId == TILE_NEVER_SEEN && debugMapIsVisible == false)
        return (int*)BLANK_BLACK;

    switch (tileSubId)
    {
    case ID_FLOOR:                                       // ID_FLOOR
        switch (screenEntryCorner)
        {
        case SCREEN_ENTRY_TL:
            tilesetIndex = (int*)FLOOR_FOUR_TL;
            break;
        case SCREEN_ENTRY_TR:
            tilesetIndex = (int*)FLOOR_FOUR_TR;
            break;
        case SCREEN_ENTRY_BL:
            tilesetIndex = (int*)FLOOR_FOUR_BL;
            break;
        case SCREEN_ENTRY_BR:
            tilesetIndex = (int*)FLOOR_FOUR_BR;
            break;
        default:
            break;
        }
        break;
    case ID_FLOOR_BIG:                               // ID_FLOOR_BIG
        switch (screenEntryCorner)
        {
        case SCREEN_ENTRY_TL:
            tilesetIndex = (int*)FLOOR_BIG_TL;
            break;
        case SCREEN_ENTRY_TR:
            tilesetIndex = (int*)FLOOR_BIG_TR;
            break;
        case SCREEN_ENTRY_BL:
            tilesetIndex = (int*)FLOOR_BIG_BL;
            break;
        case SCREEN_ENTRY_BR:
            tilesetIndex = (int*)FLOOR_BIG_BR;
            break;
        default:
            break;
        }
        break;
    case ID_FLOOR_MOSSY:                           // ID_FLOOR_MOSSY
        switch (screenEntryCorner)
        {
        case SCREEN_ENTRY_TL:
            tilesetIndex = (int*)FLOOR_MOSSY;
            break;
        case SCREEN_ENTRY_TR:
            tilesetIndex = (int*)FLOOR_MOSSY_2;
            break;
        case SCREEN_ENTRY_BL:
            tilesetIndex = (int*)FLOOR_MOSS_CHIP;
            break;
        case SCREEN_ENTRY_BR:
            tilesetIndex = (int*)FLOOR_V_MOSS_CHIP;
            break;
        default:
            break;
        }
        break;
    case ID_FLOOR_CHIP:                             // ID_FLOOR_CHIP
        switch (screenEntryCorner)
        {
        case SCREEN_ENTRY_TL:
            tilesetIndex = (int*)FLOOR_FOUR_TR;
            break;
        case SCREEN_ENTRY_TR:
            tilesetIndex = (int*)FLOOR_CHIP;
            break;
        case SCREEN_ENTRY_BL:
            tilesetIndex = (int*)FLOOR_V_CHIP;
            break;
        case SCREEN_ENTRY_BR:
            tilesetIndex = (int*)FLOOR_FOUR_BR;
            break;
        default:
            break;
        }
        break;
    case ID_WALL:                                         // ID_WALL
        switch (screenEntryCorner)
        {
        case SCREEN_ENTRY_TL:
            tilesetIndex = (int*)WALL_TOP_TL;
            break;
        case SCREEN_ENTRY_TR:
            tilesetIndex = (int*)WALL_TOP_TR;
            break;
        case SCREEN_ENTRY_BL:
            tilesetIndex = (int*)WALL_TOP_BL;
            break;
        case SCREEN_ENTRY_BR:
            tilesetIndex = (int*)WALL_TOP_BR;
            break;
        default:
            break;
        }
        break;
    case ID_WALL_FRONT:                             // ID_WALL_FRONT
        switch (screenEntryCorner)
        {
        case SCREEN_ENTRY_TL:
            tilesetIndex = (int*)WALL_FRONT_TL;
            break;
        case SCREEN_ENTRY_TR:
            tilesetIndex = (int*)WALL_FRONT_TR;
            break;
        case SCREEN_ENTRY_BL:
            tilesetIndex = (int*)WALL_FRONT_BL;
            break;
        case SCREEN_ENTRY_BR:
            tilesetIndex = (int*)WALL_FRONT_BR;
            break;
        default:
            break;
        }
        break;
    default:
        tilesetIndex = (int*)TRANSPARENT;             // TRANSPARENT
        break;
    }

    return tilesetIndex;
}

//------------------------------------------------------------------
// Function: getNumberNeighborsOfType
// 
// Checks each cardinal direction and returns the number of tiles
// that have the given type.
//------------------------------------------------------------------
u8 getNumberNeighborsOfType(int const positionX, int const positionY, int const terrainId)
{
    u8 numberNeighbors = 0;
    for (int direction = DIR_LEFT; direction <= DIR_DOWN; direction++)
    {
        if (gameMap[positionY + dirY[direction]][positionX + dirX[direction]].terrainId == terrainId)
            numberNeighbors++;
    }
    return numberNeighbors;
}

//------------------------------------------------------------------
// Function: loadGameMap
// 
// Fill all the screen entries for the gameplay screen blocks with
// the correct 8x8 graphic tiles for the gameMap's terrainIds.
//------------------------------------------------------------------
void loadGameMap()
{
    int screenBlock = GAME_MAP_SB1;
    int screenEntryTL = 0;                     // screenEntryTopLeft
    int screenEntryTR = 0;                    // screenEntryTopRight
    int screenEntryBL = 0;                  // screenEntryBottomLeft
    int screenEntryBR = 0;                 // screenEntryBottomRight
    int *tileToDraw = NULL;
    
    // Load palette
    memcpy(pal_bg_mem, tileset_stonePal, tileset_stonePalLen);
    // Load tiles into CBB 0
    memcpy(&tile_mem[0][0], tileset_stoneTiles, tileset_stoneTilesLen);

    // For each gameMap[y][x], there are four screen entries to
    // fill. We need to draw an 8x8 tile from the tileset for each
    // screen entry.
    for (int y = 0; y <= MAP_HEIGHT_TILES - 1; y++)
    {
        for (int x = 0; x <= MAP_WIDTH_TILES - 1; x++)
        {
            if (x <= 15 && y <= 15)
            {
                screenBlock = GAME_MAP_SB1;
                screenEntryTL = 2 * (y * SCREEN_BLOCK_SIZE + x);
            }
            else if (x > 15 && y <= 15)
            {
                screenBlock = GAME_MAP_SB2;
                screenEntryTL = 2 * (y * SCREEN_BLOCK_SIZE + x) - SCREEN_BLOCK_SIZE;
            }
            else if (x <= 15 && y > 15)
            {
                screenBlock = GAME_MAP_SB3;
                screenEntryTL = 2 * (y * SCREEN_BLOCK_SIZE + x) - SCREEN_BLOCK_SIZE*SCREEN_BLOCK_SIZE;
            }
            else if (x > 15 && y > 15)
            {
                screenBlock = GAME_MAP_SB4;
                screenEntryTL = 2 * (y * SCREEN_BLOCK_SIZE + x) - SCREEN_BLOCK_SIZE*(SCREEN_BLOCK_SIZE + 1);
            }
            screenEntryTR = screenEntryTL + 1;
            screenEntryBL = screenEntryTL + SCREEN_BLOCK_SIZE;
            screenEntryBR = screenEntryBL + 1;

            // Get and copy the 8x8 tile into map memory
            tileToDraw = getTilesetIndex(&gameMap[y][x], SCREEN_ENTRY_TL);
            memcpy(&se_mem[screenBlock][screenEntryTL], &tileToDraw, 2);

            tileToDraw = getTilesetIndex(&gameMap[y][x], SCREEN_ENTRY_TR);
            memcpy(&se_mem[screenBlock][screenEntryTR], &tileToDraw, 2);

            tileToDraw = getTilesetIndex(&gameMap[y][x], SCREEN_ENTRY_BL);
            memcpy(&se_mem[screenBlock][screenEntryBL], &tileToDraw, 2);

            tileToDraw = getTilesetIndex(&gameMap[y][x], SCREEN_ENTRY_BR);
            memcpy(&se_mem[screenBlock][screenEntryBR], &tileToDraw, 2);
        }
    }
    #ifdef DEBUG_MAP_GEN
        mgba_printf(MGBA_LOG_INFO, "gameMap loaded");
    #endif
}

//------------------------------------------------------------------
// Function: isSolid
// 
// Returns whether the tile at the given position is solid or not.
// NOTE: Solid tiles should block player movement.
//------------------------------------------------------------------
bool isSolid(uint8_t const positionX, uint8_t const positionY)
{
    if(gameMap[positionY][positionX].terrainId != ID_WALL)
        return false;
    else if (debugCollisionIsOff == true)
        return false;
    else
        return true;
}

//------------------------------------------------------------------
// Function: isOutOfBounds
// 
// Returns whether the tile at the given position is out of bounds
// for the player or not.
//------------------------------------------------------------------
bool isOutOfBounds(uint8_t const positionX, uint8_t const positionY)
{
    if (positionX < 0 || positionX > MAP_WIDTH_TILES - 1
    || positionY < 0 || positionY > MAP_HEIGHT_TILES - 1)
        return true;
    else
        return false;
}

//------------------------------------------------------------------
// Function: updateGameMapSight
// 
// Redraws all tiles within sightRange of the player. Used for updating
// wether a tile has been seen before.
//------------------------------------------------------------------
void updateGameMapSight(int playerX, int playerY)
{
    for (int y = playerY - sightRange; y <= playerY + sightRange; y++)
    {
        for (int x = playerX - sightRange; x <= playerX + sightRange; x++)
        {
            if (gameMap[y][x].sightId == playerSightId)
                drawGameMapScreenEntry(&gameMap[y][x]);
        }
    }
}

//------------------------------------------------------------------
// Function: drawGameMapScreenEntry
// 
// Update the four screen entries of a given tile in the gameMap[][]
//------------------------------------------------------------------
void drawGameMapScreenEntry(struct Tile* tile)
{
    int screenBlock = GAME_MAP_SB1;
    int screenEntryTL = 0;                     // screenEntryTopLeft
    int screenEntryTR = 0;                    // screenEntryTopRight
    int screenEntryBL = 0;                  // screenEntryBottomLeft
    int screenEntryBR = 0;                 // screenEntryBottomRight
    int *tileToDraw = NULL;

    if (tile->posX <= 15 && tile->posY <= 15)
    {
        screenBlock = GAME_MAP_SB1;
        screenEntryTL = 2* (tile->posY * SCREEN_BLOCK_SIZE + tile->posX);
    }
    else if (tile->posX > 15 && tile->posY <= 15)
    {
        screenBlock = GAME_MAP_SB2;
        screenEntryTL = 2* (tile->posY * SCREEN_BLOCK_SIZE + tile->posX) - SCREEN_BLOCK_SIZE;
    }
    else if (tile->posX <= 15 && tile->posY > 15)
    {
        screenBlock = GAME_MAP_SB3;
        screenEntryTL = 2* (tile->posY * SCREEN_BLOCK_SIZE + tile->posX) - SCREEN_BLOCK_SIZE*SCREEN_BLOCK_SIZE;
    }
    else if (tile->posX > 15 && tile->posY > 15)
    {
        screenBlock = GAME_MAP_SB4;
        screenEntryTL = 2* (tile->posY * SCREEN_BLOCK_SIZE + tile->posX) - SCREEN_BLOCK_SIZE*(SCREEN_BLOCK_SIZE + 1);
    }
    screenEntryTR = screenEntryTL + 1;
    screenEntryBL = screenEntryTL + SCREEN_BLOCK_SIZE;
    screenEntryBR = screenEntryBL + 1;

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
// Function: getMapSector
// 
// Check which mapSector the position is in for determining
// whether to offset the background scroll or player sprite
//------------------------------------------------------------------
uint8_t getMapSector(int const positionX, int const positionY)
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

//------------------------------------------------------------------
// Function: getRandomTileOfType
// 
// Returns a pointer to a randomly positioned tile with the given terrainId
//------------------------------------------------------------------
struct Tile* getRandomTileOfType(uint8_t const terrainId)
{
    int iterationCount = 0;
    int positionX = randomInRange(1, MAP_WIDTH_TILES - 1);
    int positionY = randomInRange(1, MAP_HEIGHT_TILES - 1);

    while (gameMap[positionY][positionX].terrainId != terrainId)
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
    return &gameMap[positionY][positionX];
}

//------------------------------------------------------------------
// Function: getTileDirection
// 
// Returns the cardinal direction required to reach the endingTile
// from the startingTile.
//------------------------------------------------------------------
enum direction getTileDirection(int startX, int startY, int endX, int endY)
{
    enum direction direction = DIR_NULL;
    int distanceX = endX - startX;
    int distanceY = endY - startY;

    // Set direction
    if (distanceY > 0) direction = DIR_DOWN;
    else if (distanceY < 0) direction = DIR_UP;
    if (distanceX > 0) direction = DIR_RIGHT;
    else if (distanceX < 0) direction = DIR_LEFT;

    // Error check for non-cardinal direction
    if (distanceY > 0 && distanceX > 0)
    {
        direction = DIR_NULL;

        #ifdef DEBUG_MAP_GEN
            mgba_printf(MGBA_LOG_DEBUG, "getTileDirection returned NULL. Non-cardinal direction detected.");
        #endif
    }

    return direction;
}
