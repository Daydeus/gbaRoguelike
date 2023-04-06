#ifndef GLOBALS_H
#define GLOBALS_H

//------------------------------------------------------------------
// Data Structures
//------------------------------------------------------------------
struct Tile
{
    uint8_t positionX;
    uint8_t positionY;
    uint8_t tileId;
    uint8_t sightStatus;
};
extern struct Tile gameMap[MAP_HEIGHT_TILES][MAP_WIDTH_TILES];

//------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------
int8_t approachValue(int8_t currentValue, int8_t targetValue, int8_t increment);

//------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------
extern unsigned int frame;
extern enum state gameState;

// Player position on gameMap[][]
extern int playerX, playerY, sightRange;

// Screen offset from TopLeft corner of gameMap[][]
extern int16_t screenOffsetX, screenOffsetY; 

extern enum direction playerFacing;
extern bool debugCollisionIsOff;

// eva and evb are .4 fixeds
extern u32 eva, evb;

#endif // GLOBALS_H
