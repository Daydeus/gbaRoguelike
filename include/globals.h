#ifndef GLOBALS_H
#define GLOBALS_H

//------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------
u32 randomInRange(int minimumValue, int maximumValue);
int8_t approachValue(int8_t currentValue, int8_t const targetValue, int8_t const increment);
boolean isNumberEven(int value);

//------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------
extern unsigned int frameCount;
extern unsigned int randomSeed;
extern enum state gameState;
extern int turnOfEntityIndex;
extern uint8_t playerSightId;      // Value visible tiles are set to
extern int8_t const dirX[9];            // Horizontal movement array
extern int8_t const dirY[9];              // Vertical movement array
extern int16_t screenOffsetX, screenOffsetY;     // BG origin offset
extern int8_t playerMoveOffsetX, playerMoveOffsetY;
extern boolean debugCollisionIsOff, debugMapIsVisible;
extern u32 blendingValue;

#endif // GLOBALS_H
