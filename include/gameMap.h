#ifndef GAME_MAP_H
#define GAME_MAP_H

#include "constants.h"

/******************************************************************/
/* Data Structures                                                */
/******************************************************************/
struct Tile
{
    uint8_t positionX;
    uint8_t positionY;
    uint8_t tileId;
    uint8_t sightStatus;
};
extern struct Tile gameMap[MAP_HEIGHT][MAP_WIDTH];

/******************************************************************/
/* Function Prototypes                                            */
/******************************************************************/
void initGameMap();
void loadGameMap();
bool isSolid(uint8_t positionX, uint8_t positionY);
bool isOutOfBounds(uint8_t positionX, uint8_t positionY);

#endif // GAME_MAP_H
