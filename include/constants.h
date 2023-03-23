#ifndef CONSTANTS_H
#define CONSTANTS_H

#define MAP_WIDTH  32 // Width in 16x16 tiles
#define MAP_HEIGHT 16 // Height in 16x16 tiles
#define TILE_SIZE  16

#define SCREEN_BLOCK_WIDTH 32

// Tileset Indexes
#define TRANSPARENT_ALL 0x06000000
#define FLOOR_BLANK_ALL 0x06000001
#define FLOOR_FOUR_TL   0x06000002
#define FLOOR_FOUR_TR   0x06000003
#define FLOOR_FOUR_BL   0x06000004
#define FLOOR_FOUR_BR   0x06000005
#define FLOOR_BIG_TL    0x06000006
#define FLOOR_BIG_TR    0x06000007
#define FLOOR_BIG_BL    0x06000008
#define FLOOR_BIG_BR    0x06000009
#define WALL_TOP_TL     0x0600000A
#define WALL_TOP_TR     0x0600000B
#define WALL_TOP_BL     0x0600000C
#define WALL_TOP_BR     0x0600000D
#define WALL_FRONT_TL   0x0600000E
#define WALL_FRONT_TR   0x0600000F
#define WALL_FRONT_BL   0x06000010
#define WALL_FRONT_BR   0x06000011

// Player Sprite Indexs
#define PLAYER_FACING_LEFT_FR1  0
#define PLAYER_FACING_LEFT_FR2  4
#define PLAYER_FACING_UP_FR1    8
#define PLAYER_FACING_UP_FR2   12
#define PLAYER_FACING_DOWN_FR1 16
#define PLAYER_FACING_DOWN_FR2 20

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

enum facing
{
    FACING_LEFT,
    FACING_RIGHT,
    FACING_UP,
    FACING_DOWN
};

#endif // CONSTANTS_H