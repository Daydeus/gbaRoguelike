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
    uint8_t tileId;
    uint8_t sightStatus;
};

//------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------
int8_t approachValue(int8_t currentValue, int8_t targetValue, int8_t increment);

//------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------
extern unsigned int frame;
extern enum state gameState;
extern struct Tile gameMap[MAP_HEIGHT_TILES][MAP_WIDTH_TILES];
extern struct Coord player;        // Player position on gameMap[][]
extern int sightRange;   // Range from player before shadow blending
extern int8_t dirX[5];      // Horizontal directional movement array
extern int8_t dirY[5];        // Vertical directional movement array
extern int16_t screenOffsetX, screenOffsetY; // Screen offset from background origin
extern enum direction playerFacing;
extern bool debugCollisionIsOff;
extern u32 eva, evb;                    // eva and evb are .4 fixeds

#endif // GLOBALS_H
