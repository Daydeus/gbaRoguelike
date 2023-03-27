#ifndef GLOBALS_H
#define GLOBALS_H

extern unsigned int frame;
extern enum state gameState;
extern int playerX, playerY;             // Player position on gameMap[][]
extern int playerScreenX, playerScreenY; // Position of player sprite drawn on screen
extern int screenOffsetX, screenOffsetY; // Screen offset from TopLeft corner of gameMap[][]
extern enum facing playerFacing;
extern u32 eva, evb;                     // eva and evb are .4 fixeds

#endif // GLOBALS_H
