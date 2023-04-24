#ifndef DEBUG_H
#define DEBUG_H

#define DEBUG
    #ifdef DEBUG

    #define DEBUG_PLAYER

   #define DEBUG_MAP_GEN
        #ifdef DEBUG_MAP_GEN
            //#define PRINT_ROOM_PLACEMENT
            //#define PRINT_MAZE_MARKING
            #define PRINT_MAP
        #endif
    #define DEBUG_GRAPHICS
        #ifdef DEBUG_GRAPHICS
            #define PRINT_MAP_DRAW
        #endif
    //#define DEBUG_FOV
#endif

//------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------
extern void printMapInLog();

#endif
