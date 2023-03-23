#include <tonc.h>
#include <string.h>
#include "gameMap.h"
#include "tileset_stone.h"
#include "tilemap_stone.h"

extern struct Tile gameMap[MAP_HEIGHT][MAP_WIDTH];

void initGameMap()
{
    for (int y = 0; y <= MAP_HEIGHT - 1; y++)
    {
        for (int x = 0; x <= MAP_WIDTH - 1; x++)
        {
            gameMap[y][x].positionX = x;
            gameMap[y][x].positionY = y;

            if (x == 0 || x == MAP_WIDTH - 1 || y == 0 || y == MAP_HEIGHT - 1) 
                gameMap[y][x].tileId = ID_WALL;
            else
                gameMap[y][x].tileId = ID_FLOOR;
        }
    }
}

void loadGameMap()
{
    int screenBlock = 30;
    int screenEntryTL = 0;  // screenEntryTopLeft
    int screenEntryTR = 0;  // screenEntryTopRight
    int screenEntryBL = 0;  // screenEntryBottomLeft
    int screenEntryBR = 0;  // screenEntryBottomRight
    int *tileToDraw = NULL;
    
    // Load palette
    memcpy(pal_bg_mem, tileset_stonePal, tileset_stonePalLen);
    // Load tiles into CBB 0
    memcpy(&tile_mem[0][0], tileset_stoneTiles, tileset_stoneTilesLen);

    // For each gameMap[y][x], there are four screen entries to fill. We need to draw an 8x8 tile 
    //   from the tileset for each screen entry.
    for (int y = 0; y <= MAP_HEIGHT - 1; y++)
    {
        for (int x = 0; x <= MAP_WIDTH - 1; x++)
        {
            if (x > 15)
            {
                screenBlock = 31;
                screenEntryTL = y * SCREEN_BLOCK_WIDTH * 2 + x * 2 - SCREEN_BLOCK_WIDTH;
                screenEntryTR = screenEntryTL + 1;
                screenEntryBL = screenEntryTL + SCREEN_BLOCK_WIDTH;
                screenEntryBR = screenEntryBL + 1;
            }
            else
            {
                screenBlock = 30;
                screenEntryTL = y * SCREEN_BLOCK_WIDTH * 2 + x * 2;
                screenEntryTR = screenEntryTL + 1;
                screenEntryBL = screenEntryTL + SCREEN_BLOCK_WIDTH;
                screenEntryBR = screenEntryBL + 1;
            }

            // Get the 8x8 tile to draw for the screen entry and then copy it to map memory
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

uint8_t getDynamicTileId(struct Tile tile)
{
    uint8_t tileId = tile.tileId;
    uint8_t positionX = tile.positionX;
    uint8_t positionY = tile.positionY;

    switch(tileId)
    {
    case ID_WALL:
        if(gameMap[positionY + 1][positionX].tileId != ID_WALL)
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

int* getTilesetIndex(struct Tile tile, uint8_t screenEntryCorner)
{
    // Tile corners are arranged sequentially in memory. Corner Top-Left, Top-Right, Bottom-Left, Bottom-Right
    uint8_t tileId = getDynamicTileId(tile);
    int *tilesetIndex = NULL;
    
    switch (tileId)
    {
    case ID_FLOOR:
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
    case ID_FLOOR_BIG:
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
    case ID_FLOOR_BLANK:
        tilesetIndex = (int*)FLOOR_BLANK_ALL;
        break;
    case ID_WALL:
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
    case ID_WALL_FRONT:
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
        tilesetIndex = (int*)TRANSPARENT_ALL;
        break;
    }

    return tilesetIndex;
}

bool isSolid(uint8_t positionX, uint8_t positionY)
{
    if(gameMap[positionY][positionX].tileId == ID_FLOOR)
        return false;
    else
        return true;
}
