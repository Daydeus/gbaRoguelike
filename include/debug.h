#ifndef DEBUG_H
#define DEBUG_H

#define DEBUG
#ifdef DEBUG
    #define DEBUG_ENTITY
        #ifdef DEBUG_ENTITY
            #define DEBUG_PLAYER
        #endif

    //#define DEBUG_MAP_GEN
        #ifdef DEBUG_MAP_GEN
            //#define PRINT_ROOM_PLACEMENT
            //#define PRINT_MAZE_MARKING
        #endif

    //#define DEBUG_GRAPHICS
        #ifdef DEBUG_GRAPHICS
            //#define PRINT_MAP_DRAW
            //#define PRINT_SIGHT_DRAW
        #endif

    //#define DEBUG_FOV
        #ifdef DEBUG_FOV
            //#define DEBUG_LOS
        #endif
#endif

//------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------
extern void printMapInLog();
extern void printTileSightInLog();

#endif
