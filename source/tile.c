#include <string.h>
#include "../libtonc/include/tonc.h"
#include "constants.h"
#include "debug.h"
#include "globals.h"
#include "mapGeneration.h"
#include "mgba.h"
#include "tile.h"
#include "tileset_stone.h"
#include "tilemap_stone.h"

//------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------
static void drawTile(struct Tile* const tile, int const screenEntryTL);
static int* getTilesetIndex(struct Tile* const tile, uint8_t const screenEntryCorner);
static uint8_t getDynamicTerrainId(struct Tile* const tile);
static int getGameMapSEOrigin(enum playerAction playerWalkedDir);
static uint8_t getNumberNeighborsOfType(int const positionX, int const positionY, int const terrainId);
static struct Tile* getRandomTileOfType(uint8_t const terrainId);

//------------------------------------------------------------------
// Function: drawTile
// 
// Draws all four corners of a 16x16 tile using the given top-left screen
// entry.
//------------------------------------------------------------------
static void drawTile(struct Tile* const tile, int const screenEntryTL)
{
    int* tilesetIndex = NULL;

    // Top-left screen entry
    tilesetIndex = getTilesetIndex(tile, SCREEN_ENTRY_TL);
    memcpy(&se_mem[GAME_MAP_SB][screenEntryTL], &tilesetIndex, 2);

    // Top-right screen entry
    tilesetIndex = getTilesetIndex(tile, SCREEN_ENTRY_TR);
    memcpy(&se_mem[GAME_MAP_SB][screenEntryTL + 1], &tilesetIndex, 2);

    // Bottom-left screen entry
    tilesetIndex = getTilesetIndex(tile, SCREEN_ENTRY_BL);
    memcpy(&se_mem[GAME_MAP_SB][screenEntryTL + SCREEN_BLOCK_SIZE], &tilesetIndex, 2);

    // Bottom-right screen entry
    tilesetIndex = getTilesetIndex(tile, SCREEN_ENTRY_BR);
    memcpy(&se_mem[GAME_MAP_SB][screenEntryTL + SCREEN_BLOCK_SIZE + 1], &tilesetIndex, 2);
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
    uint8_t tileSubId = 0;
    int *tilesetIndex = NULL;

    // Conditions for immediate return
    if (tile == NULL)
        return (int*)BLANK_BLACK;
    else if (tile->sightId == TILE_NEVER_SEEN && debugMapIsVisible == FALSE)
        return (int*)BLANK_BLACK;

    tileSubId = getDynamicTerrainId(tile);

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
// Function: getGameMapSEOrigin
// 
// Using the gameMap background scrolling offsets and the given player
// movement direction, return the current screen entry at the origin.
// Used in redrawGameMapEdge to redraw the edge of the map where the
// player can't see.
//------------------------------------------------------------------
static int getGameMapSEOrigin(enum playerAction playerWalkedDir)
{
    int screenEntryTL = 0;

    switch (playerWalkedDir)
    {
    case PLAYER_WALKED_LEFT:
        screenEntryTL = screenOffsetX / TILE_SIZE;

        // Loop around to first column
        if (screenEntryTL >= SCREEN_BLOCK_SIZE / 2)
            screenEntryTL -= SCREEN_BLOCK_SIZE / 2;
        else if (screenEntryTL < 0)
            screenEntryTL = SCREEN_BLOCK_SIZE / 2 - 1;

        // Add the vertical offset
        screenEntryTL = (screenEntryTL + screenOffsetY / TILE_SIZE * SCREEN_BLOCK_SIZE) * 2;
        break;
    case PLAYER_WALKED_RIGHT:
        screenEntryTL = screenOffsetX / TILE_SIZE + SCREEN_WIDTH_TILES - 1;

        // Loop around to first column
        if (screenEntryTL >= SCREEN_BLOCK_SIZE / 2)
            screenEntryTL -= SCREEN_BLOCK_SIZE / 2;
        else if (screenEntryTL < 0)
            screenEntryTL = SCREEN_BLOCK_SIZE / 2 - 1;

        // Add the vertical offset
        screenEntryTL = (screenEntryTL + screenOffsetY / TILE_SIZE * SCREEN_BLOCK_SIZE) * 2;
        break;
    case PLAYER_WALKED_UP:
        screenEntryTL = (screenOffsetX / TILE_SIZE + screenOffsetY / TILE_SIZE * SCREEN_BLOCK_SIZE) * 2;

        // Loop around first row
        if (screenEntryTL > SCREEN_BLOCK_SIZE * SCREEN_BLOCK_SIZE)
            screenEntryTL -= SCREEN_BLOCK_SIZE * SCREEN_BLOCK_SIZE;
        else if (screenEntryTL < 0)
            screenEntryTL = SCREEN_BLOCK_SIZE * SCREEN_BLOCK_SIZE;
        break;
    case PLAYER_WALKED_DOWN:
        screenEntryTL = (screenOffsetX / TILE_SIZE + (screenOffsetY / TILE_SIZE + SCREEN_HEIGHT_TILES) * SCREEN_BLOCK_SIZE) * 2;

        // Loop around first row
        if (screenEntryTL > SCREEN_BLOCK_SIZE * SCREEN_BLOCK_SIZE)
            screenEntryTL -= SCREEN_BLOCK_SIZE * SCREEN_BLOCK_SIZE;
        else if (screenEntryTL < 0)
            screenEntryTL = SCREEN_BLOCK_SIZE * SCREEN_BLOCK_SIZE;
        break;
    default:
        #ifdef DEBUG_GRAPHICS
            mgba_printf(MGBA_LOG_DEBUG, "  getGameMapSEOrigin: DEFAULT");
        #endif
    }

    #ifdef DEBUG_GRAPHICS
        mgba_printf(MGBA_LOG_DEBUG, "  getGameMapSEOrigin: %d", screenEntryTL);
    #endif

    return screenEntryTL;
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

    #ifdef PRINT_MAZE_MARKING
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
// Function: drawGameMap
// 
// Fills all screen entries of the gameMap's screen block with the
// appropriate tiles based on the player's position.
//------------------------------------------------------------------
extern void drawGameMap()
{
    int screenEntryTL = 0;                     // screenEntryTopLeft
    int originTileX = playerX - SCREEN_WIDTH_TILES / 2;
    int originTileY = playerY - SCREEN_HEIGHT_TILES / 2;

    // Load palette (TODO: set outside function)
    memcpy(pal_bg_mem, tileset_stonePal, tileset_stonePalLen);

    // Load tiles into CBB 0 (TODO: set outside function)
    memcpy(&tile_mem[0][0], tileset_stoneTiles, tileset_stoneTilesLen);

    #ifdef PRINT_MAP_DRAW
        mgba_printf(MGBA_LOG_DEBUG, "drawGameMap START");
        mgba_printf(MGBA_LOG_DEBUG, "  player position: (%d, %d)", playerX, playerY);
        mgba_printf(MGBA_LOG_DEBUG, "  origin tile: (%d, %d)", originTileX, originTileY);
    #endif

    // Copy each tile's graphic to the appropriate screen entry
    for (int y = 0; y < SCREEN_BLOCK_SIZE / 2; y++)
    {
        for (int x = 0; x < SCREEN_BLOCK_SIZE / 2; x++)
        {
            struct Tile *currentTile = getTile(originTileX + x, originTileY + y);

            screenEntryTL = 2 * (y * SCREEN_BLOCK_SIZE + x);

            drawTile(currentTile, screenEntryTL);
        }
    }

    // (TODO: set outside function)
    screenOffsetX = 0;
    screenOffsetY = 0;

    #ifdef PRINT_MAP_DRAW
        mgba_printf(MGBA_LOG_DEBUG, "drawGameMap END");
    #endif
}

//------------------------------------------------------------------
// Function: redrawGameMapEdge
// 
// Draws over the gameMap screenblock's farthest column or row depending
// on the player's movement direction.
//------------------------------------------------------------------
extern void redrawGameMapEdge(enum playerAction playerWalkedDir)
{
    int screenEntryTL = getGameMapSEOrigin(playerWalkedDir);
    int startingRow = screenEntryTL / SCREEN_BLOCK_SIZE;
    int tileToDrawX = 0, tileToDrawY = 0;

    #ifdef PRINT_MAP_DRAW
        mgba_printf(MGBA_LOG_DEBUG, "redrawGameMapEdge");
    #endif

    // Set the first tile to draw based on walking direction
    switch (playerWalkedDir)
    {
    case PLAYER_WALKED_LEFT:
        tileToDrawX = playerX - SCREEN_WIDTH_TILES / 2;
        tileToDrawY = playerY - SCREEN_HEIGHT_TILES / 2;
        break;
    case PLAYER_WALKED_RIGHT:
        tileToDrawX = playerX + SCREEN_WIDTH_TILES / 2;
        tileToDrawY = playerY - SCREEN_HEIGHT_TILES / 2;
        break;
    case PLAYER_WALKED_UP:
        tileToDrawX = playerX - SCREEN_WIDTH_TILES / 2;
        tileToDrawY = playerY - SCREEN_HEIGHT_TILES / 2;
        break;
    case PLAYER_WALKED_DOWN:
        tileToDrawX = playerX - SCREEN_WIDTH_TILES / 2;
        tileToDrawY = playerY + SCREEN_HEIGHT_TILES / 2;
        break;
    default:
    }

    // Loop through all tiles to be drawn
    for (int c = 0; c < SCREEN_BLOCK_SIZE / 2; c++)
    {
        int currentRow = 0;
        struct Tile *currentTile = NULL;

        // Current tile loops along either column or row
        if (playerWalkedDir == PLAYER_WALKED_LEFT || playerWalkedDir == PLAYER_WALKED_RIGHT)
            currentTile = getTile(tileToDrawX, tileToDrawY + c);
        else if (playerWalkedDir == PLAYER_WALKED_UP || playerWalkedDir == PLAYER_WALKED_DOWN)
            currentTile = getTile(tileToDrawX + c, tileToDrawY);

        drawTile(currentTile, screenEntryTL);

        #ifdef PRINT_MAP_DRAW
            mgba_printf(MGBA_LOG_DEBUG, "    screenEntryTL: %d", screenEntryTL);
        #endif

        // Move screenEntryTL to the next draw position
        if (playerWalkedDir == PLAYER_WALKED_LEFT || playerWalkedDir == PLAYER_WALKED_RIGHT)
        {
            screenEntryTL += SCREEN_BLOCK_SIZE * 2;

            // Ensure we loop around to the top of the column if necessary
            if (screenEntryTL > SCREEN_BLOCK_SIZE * SCREEN_BLOCK_SIZE)
                screenEntryTL -= SCREEN_BLOCK_SIZE * SCREEN_BLOCK_SIZE;
        }
        else if (playerWalkedDir == PLAYER_WALKED_UP || playerWalkedDir == PLAYER_WALKED_DOWN)
        {
            screenEntryTL += 2;

            // Ensure we continue drawing on the same row
            currentRow = screenEntryTL / SCREEN_BLOCK_SIZE;
            if (currentRow > startingRow) screenEntryTL -= SCREEN_BLOCK_SIZE;
            else if (currentRow < startingRow) screenEntryTL += SCREEN_BLOCK_SIZE;
        }
        else
        {
            #ifdef PRINT_MAP_DRAW
                mgba_printf(MGBA_LOG_DEBUG, "    playerWalkedDir not cardinal direction");
            #endif
        }
    }
}

//------------------------------------------------------------------
// Function: updateGameMapSight
// 
// Redraws tiles that are within sightRange of the player and either
// are visible now or were last turn. Used for drawing the terrain of
// previously unexplored tiles and tiles the player has modified.
//------------------------------------------------------------------
extern void updateGameMapSight()
{
    int distFromScreenOriginX = (SCREEN_WIDTH_TILES / 2) - sightRange;
    int distFromScreenOriginY = (SCREEN_HEIGHT_TILES / 2) - sightRange;
    int sightOriginScreenEntry = 0, currentScreenEntry = 0, currentRow = 0;

    #ifdef DEBUG_FOV
        mgba_printf(MGBA_LOG_DEBUG, "updateGameMapSight");
    #endif

    // Update sightOriginScreenEntry to point to the origin of the player's sightRange
    sightOriginScreenEntry = (screenOffsetX / TILE_SIZE) * 2 + (screenOffsetY / TILE_SIZE * SCREEN_BLOCK_SIZE) * 2;
    sightOriginScreenEntry += distFromScreenOriginX * 2 + distFromScreenOriginY * SCREEN_BLOCK_SIZE * 2;

    // Loop through tiles in the player's sight range
    for (int y = playerY - sightRange; y <= playerY + sightRange; y++)
    {
        // Loop around to the top of the screen block if necessary
        if (sightOriginScreenEntry >= SCREEN_BLOCK_SIZE * SCREEN_BLOCK_SIZE)
            sightOriginScreenEntry -= SCREEN_BLOCK_SIZE * SCREEN_BLOCK_SIZE;

        // Ensure we stay on the current row of screen entries
        currentRow = sightOriginScreenEntry / SCREEN_BLOCK_SIZE;
        if (currentRow % 2 != 0) currentRow -= 1;

        // Set the currentScreenEntry to the first in the current row
        currentScreenEntry = sightOriginScreenEntry;

        // Loop along the current row
        for (int x = playerX - sightRange; x <= playerX + sightRange; x++)
        {
            // Only update tiles that are/were visible this turn or last turn
            if (getTileSight(x, y) == playerSightId || getTileSight(x, y) == playerSightId - 1)
            {
                // If the currentScreenEntry would loop to the next row, draw it one row higher
                if (currentScreenEntry / SCREEN_BLOCK_SIZE != currentRow)
                    drawTile(getTile(x, y), currentScreenEntry - SCREEN_BLOCK_SIZE);
                else
                    drawTile(getTile(x, y), currentScreenEntry);
            }
            // Move to the next top-left screen entry in the current row
            currentScreenEntry += 2;
        }
        // Move the sightOriginScreenEntry to the next row
        sightOriginScreenEntry += SCREEN_BLOCK_SIZE * 2;
    }
}

//------------------------------------------------------------------
// Function: isOutOfBounds
// 
// Returns whether the tile at the given position is out of bounds
// for the player or not.
//------------------------------------------------------------------
extern boolean isOutOfBounds(uint8_t const positionX, uint8_t const positionY)
{
    if (positionX < 0 || positionX > MAP_WIDTH_TILES - 1
    || positionY < 0 || positionY > MAP_HEIGHT_TILES - 1)
        return TRUE;
    else
        return FALSE;
}

//------------------------------------------------------------------
// Function: isSolid
// 
// Returns whether the tile at the given position is solid or not.
// NOTE: Solid tiles should block player movement.
//------------------------------------------------------------------
extern boolean isSolid(uint8_t const positionX, uint8_t const positionY)
{
    if(getTileTerrain(positionX, positionY) != ID_WALL)
        return FALSE;
    else if (debugCollisionIsOff == TRUE)
        return FALSE;
    else
        return TRUE;
}
