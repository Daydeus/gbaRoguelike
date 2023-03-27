#ifndef GAME_MAP_H
#define GAME_MAP_H

#include "constants.h"

struct Tile
{
    uint8_t positionX;
    uint8_t positionY;
    uint8_t tileId;
    uint8_t sightStatus;
};
extern struct Tile gameMap[MAP_HEIGHT][MAP_WIDTH];

void initGameMap();
void loadGameMap();
uint8_t getDynamicTileId(struct Tile tile);
int* getTilesetIndex(struct Tile tile, uint8_t screenEntryCorner);
bool isSolid(uint8_t positionX, uint8_t positionY);

#endif // GAME_MAP_H
