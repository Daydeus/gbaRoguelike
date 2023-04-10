#ifndef GLOBALS_H
#define GLOBALS_H

//------------------------------------------------------------------
// Data Structures
//------------------------------------------------------------------
struct Coord
{
    uint8_t x;
    uint8_t y;
};

struct Tile
{
    struct Coord pos;
    uint8_t terrainId;
    uint8_t sightStatus;
};

//------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------
u32 randomInRange(int minimumValue, int maximumValue);
int8_t approachValue(int8_t currentValue, int8_t const targetValue, int8_t const increment);

//------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------
extern unsigned int frameCount;
extern unsigned int randomSeed;
extern enum state gameState;
extern struct Tile gameMap[MAP_HEIGHT_TILES][MAP_WIDTH_TILES];
extern struct Coord player;        // Player position on gameMap[][]
extern int sightRange;   // Range from player before shadow blending
extern int8_t const dirX[5];      // Horizontal directional movement array
extern int8_t const dirY[5];        // Vertical directional movement array
extern int16_t screenOffsetX, screenOffsetY; // Screen offset from background origin
extern enum direction playerFacing;
extern bool debugCollisionIsOff, debugMapIsVisible;
extern u32 blendingValue;

#endif // GLOBALS_H
