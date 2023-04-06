#include <tonc.h>
#include <string.h>
#include "constants.h"
#include "globals.h"

//------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------
int8_t dirX[5] = {0, -1, 1, 0, 0};
int8_t dirY[5] = {0, 0, 0, -1, 1};

//------------------------------------------------------------------
// Function: approachValue
//
// Increment the passed variable to the target goal
//------------------------------------------------------------------
int8_t approachValue(int8_t currentValue, int8_t targetValue, int8_t increment)
{
    if (currentValue < targetValue)
        currentValue += increment;
    else if (currentValue > targetValue)
        currentValue -= increment;

    return currentValue;
}