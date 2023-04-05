#include <tonc.h>
#include <string.h>
#include "constants.h"
#include "globals.h"
#include "gameMap.h"
#include "mgba.h"
#include "tileset_stone.h"
#include "tilemap_stone.h"

/******************************************************************/
/* Data Structures                                                */
/******************************************************************/
extern struct Tile gameMap[MAP_HEIGHT_TILES][MAP_WIDTH_TILES];

/******************************************************************/
/* Function Prototypes                                            */
/******************************************************************/
uint8_t getDynamicTileId(struct Tile tile);
int* getTilesetIndex(struct Tile tile, uint8_t screenEntryCorner);

/******************************************************************/
/* Function: getDynamicTileId                                     */
/*                                                                */
/* Takes a tile as an input and uses its position to get tileIds  */
/* for the surrounding tiles and uses that plus its own tileId to */
/* determine which variant of tile to return for being drawn.     */
/******************************************************************/
uint8_t getDynamicTileId(struct Tile tile)
{
    uint8_t tileId = tile.tileId;
    uint8_t positionX = tile.positionX;
    uint8_t positionY = tile.positionY;

    switch(tileId)
    {
    case ID_WALL:
        if(gameMap[positionY + 1][positionX].tileId != ID_WALL
        || positionY + 1 > MAP_HEIGHT_TILES -1)
            tileId = ID_WALL_FRONT;
        break;
    default:
    }
    return tileId;
}

/******************************************************************/
/* Function: getTilesetIndex                                      */
/*                                                                */
/* Matches a given tile's tileId and screenEntryCorner to pick the*/
/* index of the correct 8x8 bitmap and returns a pointer to the   */
/* bitmap so it may be drawn.                                     */
/******************************************************************/
int* getTilesetIndex(struct Tile tile, uint8_t screenEntryCorner)
{
    // Tile corners are arranged sequentially in memory.
    // Corner Top-Left, Top-Right, Bottom-Left, Bottom-Right
    uint8_t tileSubId = getDynamicTileId(tile);
    int *tilesetIndex = NULL;

    if (tile.sightStatus == TILE_NEVER_SEEN)
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
    case ID_FLOOR_BLANK:                           // ID_FLOOR_BLANK
        tilesetIndex = (int*)FLOOR_BLANK_ALL;
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

/******************************************************************/
/* Function: initGameMap                                          */
/*                                                                */
/* Initializes the gameMap by assigning the tiles' positions and  */
/* tileIds.                                                       */
/******************************************************************/
void initGameMap()
{
    for (int y = 0; y <= MAP_HEIGHT_TILES - 1; y++)
    {
        for (int x = 0; x <= MAP_WIDTH_TILES - 1; x++)
        {
            gameMap[y][x].positionX = x;
            gameMap[y][x].positionY = y;
            gameMap[y][x].sightStatus = TILE_NEVER_SEEN;

            // Map boundaries should always be a solid tile(wall)
            if (x == 0 || x == MAP_WIDTH_TILES - 1 || y == 0 
            || y == MAP_HEIGHT_TILES - 1) 
            {
                gameMap[y][x].tileId = ID_WALL;
            }
            else if (rand() % 5 == 0)
            {
                gameMap[y][x].tileId = ID_WALL;
            }
            else
            {
                if (rand() % 5 > 2)
                    gameMap[y][x].tileId = ID_FLOOR;
                else if (rand() % 5 == 1)
                    gameMap[y][x].tileId = ID_FLOOR_BIG;
                else
                    gameMap[y][x].tileId = ID_FLOOR_BLANK;
            }

        }
    }
}

/******************************************************************/
/* Function: loadGameMap                                          */
/*                                                                */
/* Fill all the screen entries for the gameplay screen blocks with*/
/* the correct 8x8 graphic tiles for the gameMap's tileIds.       */
/******************************************************************/
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
            if (x > 15)
            {
                screenBlock = GAME_MAP_SB2;
                screenEntryTL = y * SCREEN_BLOCK_SIZE * 2 + x * 2 - SCREEN_BLOCK_SIZE;
                screenEntryTR = screenEntryTL + 1;
                screenEntryBL = screenEntryTL + SCREEN_BLOCK_SIZE;
                screenEntryBR = screenEntryBL + 1;
            }
            else
            {
                screenBlock = GAME_MAP_SB1;
                screenEntryTL = y * SCREEN_BLOCK_SIZE * 2 + x * 2;
                screenEntryTR = screenEntryTL + 1;
                screenEntryBL = screenEntryTL + SCREEN_BLOCK_SIZE;
                screenEntryBR = screenEntryBL + 1;
            }

            // Get and copy the 8x8 tile into map memory
            tileToDraw = getTilesetIndex(gameMap[y][x], SCREEN_ENTRY_TL);
            memcpy(&se_mem[screenBlock][screenEntryTL], &tileToDraw, 2);

            tileToDraw = getTilesetIndex(gameMap[y][x], SCREEN_ENTRY_TR);
            memcpy(&se_mem[screenBlock][screenEntryTR], &tileToDraw, 2);

            tileToDraw = getTilesetIndex(gameMap[y][x], SCREEN_ENTRY_BL);
            memcpy(&se_mem[screenBlock][screenEntryBL], &tileToDraw, 2);

            tileToDraw = getTilesetIndex(gameMap[y][x], SCREEN_ENTRY_BR);
            memcpy(&se_mem[screenBlock][screenEntryBR], &tileToDraw, 2);
        }
    }
    #ifdef DEBUG
        mgba_printf(MGBA_LOG_INFO, "gameMap loaded");
    #endif
}

//------------------------------------------------------------------
// Function: doFOV
// 
// Perform a line-of-sight check on every bounding tile at the edge
// of the player's sight range.
//------------------------------------------------------------------
void doFOV(int positionX, int positionY)
{
    clearFOV(positionX, positionY);

    for (int x = positionX - MAX_SIGHT_RADIUS; x <= positionX + MAX_SIGHT_RADIUS; x++)
        checkLOS(positionX, positionY, x, positionY - MAX_SIGHT_RADIUS);
    for (int x = positionX - MAX_SIGHT_RADIUS; x <= positionX + MAX_SIGHT_RADIUS; x++)
        checkLOS(positionX, positionY, x, positionY + MAX_SIGHT_RADIUS);
    for (int y = positionY - MAX_SIGHT_RADIUS; y <= positionY + MAX_SIGHT_RADIUS; y++)
        checkLOS(positionX, positionY, positionX - MAX_SIGHT_RADIUS, y);
    for (int y = positionY - MAX_SIGHT_RADIUS; y <= positionY + MAX_SIGHT_RADIUS; y++)
        checkLOS(positionX, positionY, positionX + MAX_SIGHT_RADIUS, y);

    drawFOV(positionX, positionY);
}

/******************************************************************/
/* Function: setTileSeenStatus                                    */
/*                                                                */
/* Sets the seenStatus of the tile at the given position to the   */
/* given status.                                                  */
/******************************************************************/
void setTileSeenStatus(uint8_t positionX, uint8_t positionY, uint8_t sightStatus)
{
    gameMap[positionY][positionX].sightStatus = sightStatus;
}

/******************************************************************/
/* Function: isSolid                                              */
/*                                                                */
/* Returns whether the tile at the given position is solid or not.*/
/* Solid tiles block player movement.                             */
/******************************************************************/
bool isSolid(uint8_t positionX, uint8_t positionY)
{
    if(gameMap[positionY][positionX].tileId != ID_WALL)
        return false;
    else if (debugCollisionIsOff == true)
        return false;
    else
        return true;
}

/******************************************************************/
/* Function: isOutOfBounds                                        */
/*                                                                */
/* Returns whether the tile at the given position is out of bounds*/
/* for the player or not.                                         */
/******************************************************************/
bool isOutOfBounds(uint8_t positionX, uint8_t positionY)
{
    if (positionX < 0 || positionX > MAP_WIDTH_TILES - 1
    || positionY < 0 || positionY > MAP_HEIGHT_TILES - 1)
        return true;
    else
        return false;
}

//------------------------------------------------------------------
// Function: initFOV                                        
// 
// Initialize field-of-vision background layer by filling with black
// 8x8 graphic tiles.
//------------------------------------------------------------------
void initFOV()
{
    int *tileToDraw = (int*)BLANK_BLACK;

    for (int y = 0; y < SCREEN_BLOCK_SIZE; y++)
    {
        for (int x = 0; x < SCREEN_BLOCK_SIZE; x++)
        {
            memcpy(&se_mem[FOV_SB][y * SCREEN_BLOCK_SIZE + x], &tileToDraw, 2);
        }
    }
}

//------------------------------------------------------------------
// Function: clearFOV                                        
// 
// Sets all tiles within MAX_SIGHT_RADIUS of the given position to 
// not in sight.
//------------------------------------------------------------------
void clearFOV(int positionX, int positionY)
{
    for (int y = positionY - SCREEN_HEIGHT_TILES / 2 - 1; y < positionY + SCREEN_HEIGHT_TILES / 2 + 1; y++)
    {
        for (int x = positionX - SCREEN_WIDTH_TILES / 2 - 1; x < positionX + SCREEN_WIDTH_TILES / 2 + 1; x++)
        {
            if (!isOutOfBounds(x, y) && gameMap[y][x].sightStatus != TILE_NEVER_SEEN)
                gameMap[y][x].sightStatus = TILE_NOT_IN_SIGHT;
        }
    }

    #ifdef DEBUG
        mgba_printf(MGBA_LOG_DEBUG, "FOV cleared");
    #endif
}

//------------------------------------------------------------------
// Function: drawFOV
// 
// Updates field-of-vision background layer by copying the correct
// 8x8 graphic based on tile.sightStatus.
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
        originTileY = positionY - 4;
        break;
    case SECTOR_BOT_LEFT:                         // SECTOR_BOT_LEFT
        originTileX = 0;
        originTileY = 7;
        break;
    case SECTOR_TOP_MID:                           // SECTOR_TOP_MID
        originTileX = positionX - 7;
        originTileY = 0;
        break;
    case SECTOR_MID_MID:                           // SECTOR_MID_MID
        originTileX = positionX - 7;
        originTileY = positionY - 4;
        break;
    case SECTOR_BOT_MID:                           // SECTOR_BOT_MID
        originTileX = positionX - 7;
        originTileY = 7;
        break;
    case SECTOR_TOP_RIGHT:                       // SECTOR_TOP_RIGHT
        originTileX = 17;
        originTileY = 0;
        break;
    case SECTOR_MID_RIGHT:                       // SECTOR_MID_RIGHT
        originTileX = 17;
        originTileY = positionY - 4;
        break;
    case SECTOR_BOT_RIGHT:                       // SECTOR_BOT_RIGHT
        originTileX = 17;
        originTileY = 7;
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


            // Get index of tile to draw from tileset
            switch (gameMap[currentTileY][currentTileX].sightStatus)
            {
            case TILE_LIT:
            case TILE_NOT_LIT:
                tileToDraw = (int*)TRANSPARENT;
                break;
            case TILE_NOT_IN_SIGHT:
            case TILE_NEVER_SEEN:
            default:
                tileToDraw = (int*)BLANK_BLACK;
            }

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
    #ifdef DEBUG
        mgba_printf(MGBA_LOG_DEBUG, "FOV drawn");
    #endif
}

//------------------------------------------------------------------
// Function: checkLOS
// 
// Uses Bresenham's algorithm to step through a line.
//------------------------------------------------------------------
void checkLOS(int x1, int y1, int const x2, int const y2)
{
    // If x1 == x2, then it does not matter what we set here
    int delta_x = x2 - x1;
    s8 const ix = ((delta_x > 0) - (delta_x < 0));
    delta_x = ABS(delta_x) << 1;

    // If y1 == y2, then it does not matter what we set here
    int delta_y = y2 - y1;
    s8 const iy= ((delta_y > 0) - (delta_y < 0));
    delta_y = ABS(delta_y) << 1;

    gameMap[y1][x1].sightStatus = TILE_NOT_LIT;
    drawGameMapScreenEntry(gameMap[y1][x1]);
    //updateFOVScreenEntry(gameMap[y1][x1], playerX, playerY);

    if (delta_x >= delta_y)
    {
        // error may go below zero
        int error = (delta_y - (delta_x >> 1));
 
        while (x1 != x2)
        {
            // Reduce error, while taking into account the corner case of error == 0
            if ((error > 0) || (!error && (ix > 0)))
            {
                error -= delta_x;
                y1 += iy;
            }
            // Else do nothing

            error += delta_y;
            x1 += ix;

            if (!isOutOfBounds(x1, y1))
            {
                if (gameMap[y1][x1].sightStatus == TILE_NEVER_SEEN)
                {
                    gameMap[y1][x1].sightStatus = TILE_NOT_LIT;
                    drawGameMapScreenEntry(gameMap[y1][x1]);
                }
                gameMap[y1][x1].sightStatus = TILE_NOT_LIT;
                if (isSolid(x1, y1))
                    return;
            }
        }
    }
    else
    {
        // error may go below zero
        int error = (delta_x - (delta_y >> 1));

        while (y1 != y2)
        {
            // Reduce error, while taking into account the corner case of error == 0
            if ((error > 0) || (!error && (iy > 0)))
            {
                error -= delta_y;
                x1 += ix;
            }
            // Else do nothing

            error += delta_x;
            y1 += iy;
 
            if (!isOutOfBounds(x1, y1))
            {
                if (gameMap[y1][x1].sightStatus == TILE_NEVER_SEEN)
                {
                    gameMap[y1][x1].sightStatus = TILE_NOT_LIT;
                    drawGameMapScreenEntry(gameMap[y1][x1]);
                }
                gameMap[y1][x1].sightStatus = TILE_NOT_LIT;
                if (isSolid(x1, y1))
                    return;
            }
        }
    }
}

//------------------------------------------------------------------
// Function: drawGameMapScreenEntry
// 
// Update the four screen entries of a given tile for the gameMap
//------------------------------------------------------------------
void drawGameMapScreenEntry(struct Tile tile)
{
    int screenBlock = GAME_MAP_SB1;
    int screenEntryTL = 0;                     // screenEntryTopLeft
    int screenEntryTR = 0;                    // screenEntryTopRight
    int screenEntryBL = 0;                  // screenEntryBottomLeft
    int screenEntryBR = 0;                 // screenEntryBottomRight
    int *tileToDraw = NULL;

    if (tile.positionX > 15)
    {
        screenBlock = GAME_MAP_SB2;
        screenEntryTL = tile.positionY * SCREEN_BLOCK_SIZE * 2 + tile.positionX * 2 - SCREEN_BLOCK_SIZE;
        screenEntryTR = screenEntryTL + 1;
        screenEntryBL = screenEntryTL + SCREEN_BLOCK_SIZE;
        screenEntryBR = screenEntryBL + 1;
    }
    else
    {
        screenBlock = GAME_MAP_SB1;
        screenEntryTL = tile.positionY * SCREEN_BLOCK_SIZE * 2 + tile.positionX * 2;
        screenEntryTR = screenEntryTL + 1;
        screenEntryBL = screenEntryTL + SCREEN_BLOCK_SIZE;
        screenEntryBR = screenEntryBL + 1;
    }

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

/******************************************************************/
/* Function: getMapSector                                         */
/*                                                                */
/* Check which mapSector the position is in for determining       */
/* whether to offset the background scroll or player sprite       */
/******************************************************************/
uint8_t getMapSector(int positionX, int positionY)
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
