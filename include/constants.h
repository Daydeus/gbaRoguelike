#ifndef CONSTANTS_H
#define CONSTANTS_H

#define MAP_WIDTH  32 // Width in 16x16 tiles
#define MAP_HEIGHT 16 // Height in 16x16 tiles
#define TILE_SIZE  16

#define SCREEN_BLOCK_WIDTH 32

enum
{
    ID_TRANSPARENT = 0,
    ID_FLOOR,
    ID_FLOOR_BLANK,     // Just for visual variation
    ID_FLOOR_BIG,       // Just for visual variation
    ID_WALL,
    ID_WALL_FRONT       // Just for visual variation
};

enum
{
    SCREEN_ENTRY_TL = 1,
    SCREEN_ENTRY_TR,
    SCREEN_ENTRY_BL,
    SCREEN_ENTRY_BR
};

#endif // CONSTANTS_H