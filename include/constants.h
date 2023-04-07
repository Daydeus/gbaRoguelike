#ifndef CONSTANTS_H
#define CONSTANTS_H

//#define DEBUG

#define SCREEN_BG_0 0
#define SCREEN_BG_1 1
#define SCREEN_BG_2 2
#define SCREEN_BG_3 3
#define BG_0_BLEND_UP 0x80

#define TILE_SIZE  16
#define MAP_WIDTH_TILES  32
#define MAP_HEIGHT_TILES 16
#define SCREEN_WIDTH_TILES     SCREEN_WIDTH / TILE_SIZE        // 15
#define SCREEN_HEIGHT_TILES    SCREEN_HEIGHT / TILE_SIZE       // 10

#define SCREEN_BLOCK_SIZE 32

#define GAME_HUD_SB   25
#define FOV_SB        26        // First Screen Block for Fog of War
#define GAME_MAP_SB1  28           // First Screen Block for gameMap
#define GAME_MAP_SB2  29          // Second Screen Block for gameMap
#define PAUSE_MENU_SB 20

// Tileset Indices
#define TRANSPARENT       0x06000000
#define BLANK_BLACK       0x06000001
#define FLOOR_FOUR_TL     0x06000002
#define FLOOR_FOUR_TR     0x06000003
#define FLOOR_FOUR_BL     0x06000004
#define FLOOR_FOUR_BR     0x06000005
#define FLOOR_CHIP        0x06000006
#define FLOOR_V_CHIP      0x06000007
#define FLOOR_MOSSY       0x06000008
#define FLOOR_MOSSY_2     0x06000009
#define FLOOR_MOSS_CHIP   0x0600000A
#define FLOOR_V_MOSS_CHIP 0x0600000B
#define FLOOR_BIG_TL      0x0600000C
#define FLOOR_BIG_TR      0x0600000D
#define FLOOR_BIG_BL      0x0600000E
#define FLOOR_BIG_BR      0x0600000F
#define WALL_TOP_TL       0x06000010
#define WALL_TOP_TR       0x06000011
#define WALL_TOP_BL       0x06000012
#define WALL_TOP_BR       0x06000013
#define WALL_FRONT_TL     0x06000014
#define WALL_FRONT_TR     0x06000015
#define WALL_FRONT_BL     0x60000016
#define WALL_FRONT_BR     0x60000017
#define HEART_TL          0x60000018
#define HEART_TR          0x60000019
#define HEART_BL          0x6000001A
#define HEART_BR          0x6000001B
#define FOV_TINT_DARK     0x6000001C
#define FOV_TINT_LIGHT    0x6000001D

// Player Sprite Indices
#define PLAYER_FACING_LEFT_FR1  0
#define PLAYER_FACING_LEFT_FR2  4
#define PLAYER_FACING_UP_FR1    8
#define PLAYER_FACING_UP_FR2   12
#define PLAYER_FACING_DOWN_FR1 16
#define PLAYER_FACING_DOWN_FR2 20

enum state
{
    STATE_TITLE_SCREEN,
    STATE_GAMEPLAY,
    STATE_MENU
};

enum
{
    ID_TRANSPARENT = 0,
    ID_FLOOR,
    ID_FLOOR_BIG,                       // Just for visual variation
    ID_FLOOR_MOSSY,                     // Just for visual variation
    ID_FLOOR_CHIP,                      // Just for visual variation
    ID_WALL,
    ID_WALL_FRONT                       // Just for visual variation
};

enum
{
    TILE_NEVER_SEEN = 0,
    TILE_NOT_IN_SIGHT,
    TILE_NOT_LIT,
    TILE_LIT
};

enum
{
    SCREEN_ENTRY_TL = 1,
    SCREEN_ENTRY_TR,
    SCREEN_ENTRY_BL,
    SCREEN_ENTRY_BR
};

enum direction
{
    DIR_NULL,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_UP,
    DIR_DOWN
};

enum
{
    DIM_HEIGHT,
    DIM_WIDTH
};

enum mapSector
{
    SECTOR_ERROR,
    SECTOR_TOP_LEFT,
    SECTOR_TOP_MID,
    SECTOR_TOP_RIGHT,
    SECTOR_MID_LEFT,
    SECTOR_MID_MID,
    SECTOR_MID_RIGHT,
    SECTOR_BOT_LEFT,
    SECTOR_BOT_MID,
    SECTOR_BOT_RIGHT
};

enum
{
    SIGHT_RANGE_MIN = 1,
    SIGHT_RANGE_MAX = 4
};

enum playerAction
{   PLAYER_NO_ACTION = 0,
    PLAYER_WALKED_LEFT,
    PLAYER_WALKED_RIGHT,
    PLAYER_WALKED_UP,
    PLAYER_WALKED_DOWN
};

#endif // CONSTANTS_H
