#ifndef GAME_MAP_H
#define GAME_MAP_H

//------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------
void loadGameMap();
bool isSolid(uint8_t const positionX, uint8_t const positionY);
bool isOutOfBounds(uint8_t const positionX, uint8_t const positionY);
uint8_t getMapSector(int const positionX, int const positionY);
enum direction getTileDirection(int startX, int startY, int endX, int endY);
void updateGameMapSight(int playerX, int playerY);

#endif // GAME_MAP_H
