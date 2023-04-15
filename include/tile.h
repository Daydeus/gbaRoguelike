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
extern void initTilePosition(int const positionX, int const positionY);
extern struct Tile* getTile(int const positionX, int const positionY);
extern uint8_t getTileTerrain(int const positionX, int const positionY);
extern void setTileTerrain(int const positionX, int const positionY, uint8_t const terrainId);
extern uint8_t getTileSight(int const positionX, int const positionY);
extern void setTileSight(int const positionX, int const positionY, uint8_t const sightId);
extern enum direction getTileDirection(int const startX, int const startY, int const endX, int const endY);
extern enum direction getTileDirInLine(int const startX, int const startY, int const endX, int const endY);

extern void loadGameMap();
extern void updateGameMapSight(int const playerX, int const playerY);

extern bool isOutOfBounds(uint8_t const positionX, uint8_t const positionY);
extern bool isSolid(uint8_t const positionX, uint8_t const positionY);
extern uint8_t getMapSector(int const positionX, int const positionY);

#endif // TILE_H
