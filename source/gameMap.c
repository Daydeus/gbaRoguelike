#include <tonc.h>
#include <string.h>
#include "constants.h"
#include "globals.h"
#include "gameMap.h"
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
uint8_t getDynamicTileId(struct Tile* const tile);
int* getTilesetIndex(struct Tile* const tile, uint8_t const screenEntryCorner);
void initFOV();
void clearFOV(int const positionX, int const positionY);
void drawFOV(int const positionX, int const positionY);
void drawGameMapScreenEntry(struct Tile* const tile);
void checkLOS(int x1, int y1, int const x2, int const y2);

//------------------------------------------------------------------
// Function: getDynamicTileId
// 
// Takes a tile as an input and uses its position to get tileIds
// for the surrounding tiles and uses that plus its own tileId to
// determine which variant of tile to return for being drawn.
//------------------------------------------------------------------
uint8_t getDynamicTileId(struct Tile* const tile)
{
    uint8_t tileId = tile->tileId;

    switch(tileId)
    {
    case ID_WALL:
        if(gameMap[tile->pos.y + 1][tile->pos.x].tileId != ID_WALL || tile->pos.y + 1 > MAP_HEIGHT_TILES -1)
            tileId = ID_WALL_FRONT;
        break;
    default:
    }
    return tileId;
}

//------------------------------------------------------------------
// Function: getTilesetIndex
// 
// Matches a given tile's tileId and screenEntryCorner to pick the
// index of the correct 8x8 bitmap and returns a pointer to the
// bitmap so it may be drawn.
//------------------------------------------------------------------
int* getTilesetIndex(struct Tile* const tile, uint8_t const screenEntryCorner)
{
    // Tile corners are arranged sequentially in memory.
    // Corner Top-Left, Top-Right, Bottom-Left, Bottom-Right
    uint8_t tileSubId = getDynamicTileId(tile);
    int *tilesetIndex = NULL;

    if (tile->sightStatus == TILE_NEVER_SEEN && debugMapIsVisible == false)
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
// Function: initGameMap
// 
// Initializes the gameMap by assigning the tiles' positions and
// tileIds.
//------------------------------------------------------------------
void initGameMap()
{
    for (int y = 0; y <= MAP_HEIGHT_TILES - 1; y++)
    {
        for (int x = 0; x <= MAP_WIDTH_TILES - 1; x++)
        {
            gameMap[y][x].pos.x = x;
            gameMap[y][x].pos.y = y;
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
                switch(rand() % 5)
                {
                case 0:
                case 1: gameMap[y][x].tileId = ID_FLOOR; break;
                case 2: gameMap[y][x].tileId = ID_FLOOR_BIG; break;
                case 3: gameMap[y][x].tileId = ID_FLOOR_MOSSY; break;
                case 4: gameMap[y][x].tileId = ID_FLOOR_CHIP; break;
                }
            }

        }
    }
}

//------------------------------------------------------------------
// Function: loadGameMap
// 
// Fill all the screen entries for the gameplay screen blocks with
// the correct 8x8 graphic tiles for the gameMap's tileIds.
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
    #ifdef DEBUG
        mgba_printf(MGBA_LOG_DEBUG, "gameMap loaded");
    #endif
}

//------------------------------------------------------------------
// Function: doFOV
// 
// Perform a line-of-sight check on every bounding tile at the edge
// of the player's sight range.
//------------------------------------------------------------------
void doFOV(int const positionX, int const positionY, int const sightRange)
{
    clearFOV(positionX, positionY);

    for (int x = positionX - sightRange; x <= positionX + sightRange; x++)
        checkLOS(positionX, positionY, x, positionY - sightRange);
    for (int x = positionX - sightRange; x <= positionX + sightRange; x++)
        checkLOS(positionX, positionY, x, positionY + sightRange);
    for (int y = positionY - sightRange; y <= positionY + sightRange; y++)
        checkLOS(positionX, positionY, positionX - sightRange, y);
    for (int y = positionY - sightRange; y <= positionY + sightRange; y++)
        checkLOS(positionX, positionY, positionX + sightRange, y);

    drawFOV(positionX, positionY);
}

//------------------------------------------------------------------
// Function: setTileSeenStatus
// 
// Sets the seenStatus of the tile at the given position to the
// given status.
//------------------------------------------------------------------
void setTileSeenStatus(uint8_t positionX, uint8_t positionY, uint8_t sightStatus)
{
    gameMap[positionY][positionX].sightStatus = sightStatus;
}

//------------------------------------------------------------------
// Function: isSolid
// 
// Returns whether the tile at the given position is solid or not.
// NOTE: Solid tiles should block player movement.
//------------------------------------------------------------------
bool isSolid(uint8_t const positionX, uint8_t const positionY)
{
    if(gameMap[positionY][positionX].tileId != ID_WALL)
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
// Function: clearFOV
// 
// Sets all tiles near the given position to TILE_NOT_IN_SIGHT
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
                tileToDraw = (int*)FOV_TINT_DARK;
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
// Uses Bresenham's algorithm to step through a line from (x1, y1)
// to (x2, y2). At each step, it sets the sight status of the tile.
//------------------------------------------------------------------
void checkLOS(int x1, int y1, int const x2, int const y2)
{
    int dx =  ABS(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -ABS(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2; /* error value e_xy */
 
    for (;;)
    {  /* loop */
        if (!isOutOfBounds(x1, y1))
        {
            if (gameMap[y1][x1].sightStatus == TILE_NEVER_SEEN)
            {
                gameMap[y1][x1].sightStatus = TILE_NOT_LIT;
                drawGameMapScreenEntry(&gameMap[y1][x1]);
            }
            else
                gameMap[y1][x1].sightStatus = TILE_NOT_LIT;
        }

        if ((x1 == x2 && y1 == y2) // Line has reached point (x2,y2)
        || isSolid(x1, y1)         // Has encountered solid tile
        // If NOT a horizontal or vertical line and is crossing a
        // diagonal between two solid tiles
        || ((x1 != x2 && y1 != y2) && (isSolid(x1 + sx, y1) && isSolid(x1, y1 +sy))))
            break;

        e2 = 2 * err;
        if (e2 >= dy)
        {
            err += dy;
            x1 += sx;
        } /* e_xy+e_x > 0 */

        if (e2 <= dx)
        {
            err += dx;
            y1 += sy;
        } /* e_xy+e_y < 0 */
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

    if (tile->pos.x <= 15 && tile->pos.y <= 15)
    {
        screenBlock = GAME_MAP_SB1;
        screenEntryTL = 2* (tile->pos.y * SCREEN_BLOCK_SIZE + tile->pos.x);
    }
    else if (tile->pos.x > 15 && tile->pos.y <= 15)
    {
        screenBlock = GAME_MAP_SB2;
        screenEntryTL = 2* (tile->pos.y * SCREEN_BLOCK_SIZE + tile->pos.x) - SCREEN_BLOCK_SIZE;
    }
    else if (tile->pos.x <= 15 && tile->pos.y > 15)
    {
        screenBlock = GAME_MAP_SB3;
        screenEntryTL = 2* (tile->pos.y * SCREEN_BLOCK_SIZE + tile->pos.x) - SCREEN_BLOCK_SIZE*SCREEN_BLOCK_SIZE;
    }
    else if (tile->pos.x > 15 && tile->pos.y > 15)
    {
        screenBlock = GAME_MAP_SB4;
        screenEntryTL = 2* (tile->pos.y * SCREEN_BLOCK_SIZE + tile->pos.x) - SCREEN_BLOCK_SIZE*(SCREEN_BLOCK_SIZE + 1);
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
