#include <tonc.h>
#include <string.h>
#include "constants.h"
#include "globals.h"

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