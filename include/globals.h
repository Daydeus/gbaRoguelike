#ifndef GLOBALS_H
#define GLOBALS_H

//------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------
u32 randomInRange(int minimumValue, int maximumValue);
int8_t approachValue(int8_t currentValue, int8_t const targetValue, int8_t const increment);
bool isNumberEven(int value);

//------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------
extern unsigned int frameCount;
extern unsigned int randomSeed;
extern enum state gameState;
extern int playerX, playerY;                      // Player position
extern uint8_t playerSightId;      // Value visible tiles are set to
extern int sightRange;                // Blends shadow outside range
extern int8_t const dirX[9];            // Horizontal movement array
extern int8_t const dirY[9];              // Vertical movement array
extern int16_t screenOffsetX, screenOffsetY;     // BG origin offset
extern enum direction playerFacing;
extern bool debugCollisionIsOff, debugMapIsVisible;
extern u32 blendingValue;

#endif // GLOBALS_H
