#ifndef TILE_H
#define TILE_H

//------------------------------------------------------------------
// Data Structures
//------------------------------------------------------------------
struct Tile
{
    uint8_t posX, posY;
    uint8_t terrainId;
    uint8_t sightId;
};
extern struct Tile gameMap[MAP_HEIGHT_TILES][MAP_WIDTH_TILES];

//------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------
void initTilePosition(int const positionX, int const positionY);
struct Tile* getTile(int const positionX, int const positionY);
uint8_t getTileTerrain(int const positionX, int const positionY);
void setTileTerrain(int const positionX, int const positionY, uint8_t const terrainId);
uint8_t getTileSight(int const positionX, int const positionY);
void setTileSight(int const positionX, int const positionY, uint8_t const sightId);
enum direction getTileDirection(int const startX, int const startY, int const endX, int const endY);

void loadGameMap();
void updateGameMapSight(int const playerX, int const playerY);

bool isOutOfBounds(uint8_t const positionX, uint8_t const positionY);
bool isSolid(uint8_t const positionX, uint8_t const positionY);
uint8_t getMapSector(int const positionX, int const positionY);

#endif // TILE_H
