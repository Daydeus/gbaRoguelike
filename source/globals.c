#include <stdlib.h>
#include <tonc.h>
#include <string.h>
#include "constants.h"
#include "globals.h"

//------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------
int8_t const dirX[5] = {0, -1, 1, 0, 0};
int8_t const dirY[5] = {0, 0, 0, -1, 1};

//------------------------------------------------------------------
// Function: randomInRange
//
// Returns a random value between (and including) the given minimum
// and maximum value.
//------------------------------------------------------------------
u32 randomInRange(int minimumValue, int maximumValue)
{
    return rand() % (maximumValue - minimumValue + 1) + minimumValue;
}

//------------------------------------------------------------------
// Function: approachValue
//
// Increment the passed variable to the target goal
//------------------------------------------------------------------
int8_t approachValue(int8_t currentValue, int8_t const targetValue, int8_t const increment)
{
    if (currentValue < targetValue)
        currentValue += increment;
    else if (currentValue > targetValue)
        currentValue -= increment;

    return currentValue;
}