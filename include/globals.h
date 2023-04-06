#ifndef GLOBALS_H
#define GLOBALS_H

extern unsigned int frame;
extern enum state gameState;

// Player position on gameMap[][]
extern int playerX, playerY, sightRange;

// Screen offset from TopLeft corner of gameMap[][]
extern int16_t screenOffsetX, screenOffsetY; 

extern enum direction playerFacing;
extern bool debugCollisionIsOff;

// eva and evb are .4 fixeds
extern u32 eva, evb;

#endif // GLOBALS_H
