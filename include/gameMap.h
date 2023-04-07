#ifndef GAME_MAP_H
#define GAME_MAP_H

//------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------
void initGameMap();
void loadGameMap();
void doFOV(int const positionX, int const positionY, int const sightRange);
void setTileSeenStatus(uint8_t positionX, uint8_t positionY, uint8_t sightStatus);
bool isSolid(uint8_t const positionX, uint8_t const positionY);
bool isOutOfBounds(uint8_t const positionX, uint8_t const positionY);
void initFOV();
uint8_t getMapSector(int const positionX, int const positionY);

#endif // GAME_MAP_H
