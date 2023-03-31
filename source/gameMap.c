#include <tonc.h>
#include <string.h>
#include "constants.h"
#include "globals.h"
#include "gameMap.h"
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
    case ID_FLOOR:
        if (rand() % 5 == 3)
            tileId = ID_FLOOR_BIG;
        else if (rand() % 5 == 4)
            tileId = ID_FLOOR_BLANK;
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
        tilesetIndex = (int*)BLANK_BLACK;          // ID_BLANK_BLACK
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
                gameMap[y][x].tileId = ID_FLOOR;
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
                screenEntryTL = y * SCREEN_BLOCK_WIDTH * 2 + x * 2 - SCREEN_BLOCK_WIDTH;
                screenEntryTR = screenEntryTL + 1;
                screenEntryBL = screenEntryTL + SCREEN_BLOCK_WIDTH;
                screenEntryBR = screenEntryBL + 1;
            }
            else
            {
                screenBlock = GAME_MAP_SB1;
                screenEntryTL = y * SCREEN_BLOCK_WIDTH * 2 + x * 2;
                screenEntryTR = screenEntryTL + 1;
                screenEntryBL = screenEntryTL + SCREEN_BLOCK_WIDTH;
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
}

/******************************************************************/
/* Function: isSolid                                              */
/*                                                                */
/* Returns whether the tile at the given position is solid or not.*/
/* Solid tiles block player movement.                             */
/******************************************************************/
bool isSolid(uint8_t positionX, uint8_t positionY)
{
    if(gameMap[positionY][positionX].tileId == ID_FLOOR)
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
    if (positionX <= 0 || positionX >= MAP_WIDTH_TILES - 1
    || positionY <= 0 || positionY >= MAP_HEIGHT_TILES - 1)
        return true;
    else
        return false;
}
