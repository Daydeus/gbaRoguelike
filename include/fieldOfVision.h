#ifndef FOV_H
#define FOV_H

//------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------
extern void initFOV();
extern void doFOV(int const positionX, int const positionY, int const sightRange);
extern boolean checkLOS(int startX, int startY, int const endX, int const endY);

#endif // FOV_H
