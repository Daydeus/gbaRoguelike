#ifndef DEBUG_H
#define DEBUG_H

void tte_write_var_int(int varToPrint);
void getTileStatusDebug(int positionX, int positionY);
bool doDebugMenuInput();
void drawDebugMenu();
void doStateTransition(enum state targetState);

#endif // DEBUG_H
