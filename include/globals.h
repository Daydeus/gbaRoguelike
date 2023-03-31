#ifndef GLOBALS_H
#define GLOBALS_H

extern unsigned int frame;
extern enum state gameState;

// Player position on gameMap[][]
extern int playerX, playerY;

// Position of player sprite drawn on screen
extern int playerScreenX, playerScreenY;

// Screen offset from TopLeft corner of gameMap[][]
extern int screenOffsetX, screenOffsetY; 

extern enum direction playerFacing;
extern bool debugCollisionIsOff;

// eva and evb are .4 fixeds
extern u32 eva, evb;

#endif // GLOBALS_H
