#include <stdlib.h>
#include <tonc.h>
#include <string.h>
#include "constants.h"
#include "globals.h"

//------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------
int8_t const dirX[9] = {0, -1, 1, 0, 0, -1, 1, -1, 1};
int8_t const dirY[9] = {0, 0, 0, -1, 1, -1, -1, 1, 1};

//------------------------------------------------------------------
// Function: randomInRange
//
// Returns a random value between (and including) the given minimum
// and maximum value.
//------------------------------------------------------------------
extern u32 randomInRange(int minimumValue, int maximumValue)
{
    return rand() % (maximumValue - minimumValue + 1) + minimumValue;
}

//------------------------------------------------------------------
// Function: approachValue
//
// Increment the passed variable to the target goal.
//------------------------------------------------------------------
extern int8_t approachValue(int8_t currentValue, int8_t const targetValue, int8_t const increment)
{
    if (currentValue < targetValue)
        currentValue += increment;
    else if (currentValue > targetValue)
        currentValue -= increment;

    return currentValue;
}

//------------------------------------------------------------------
// Function: isNumberEven
//
// Returns whether the given number is evenly divisble by two.
//------------------------------------------------------------------
extern bool isNumberEven(int value)
{
    if (value % 2 == 0)
        return true;
    else
        return false;
}
