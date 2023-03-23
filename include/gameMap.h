#ifndef GAME_MAP_H
#define GAME_MAP_H

#include "constants.h"

struct Tile
{
    uint8_t positionX;
    uint8_t positionY;
    uint8_t tileId;
};
extern struct Tile gameMap[MAP_HEIGHT][MAP_WIDTH];

extern void initGameMap();
extern void loadGameMap();
extern uint8_t getDynamicTileId(struct Tile tile);
extern int* getTilesetIndex(struct Tile tile, uint8_t screenEntryCorner);

#endif // GAME_MAP_H