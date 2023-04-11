#ifndef PAUSEMENU_H
#define PAUSEMENU_H

//------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------
extern bool doPauseMenuInput();
extern void drawPauseMenu();
extern void doStateTransition(enum state const targetState);

#endif // PAUSEMENU_H
