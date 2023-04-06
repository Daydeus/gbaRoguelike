#ifndef GAME_MAP_H
#define GAME_MAP_H

//------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------
void initGameMap();
void loadGameMap();
void doFOV(int positionX, int positionY, int sightRange);
void setTileSeenStatus(uint8_t positionX, uint8_t positionY, uint8_t sightStatus);
bool isSolid(uint8_t positionX, uint8_t positionY);
bool isOutOfBounds(uint8_t positionX, uint8_t positionY);
void initFOV();
uint8_t getMapSector(int positionX, int positionY);

#endif // GAME_MAP_H
