#ifndef MAP_GEN_H
#define MAP_GEN_H

//------------------------------------------------------------------
// Data Structures
//------------------------------------------------------------------
struct Node
{
    struct Tile* tile;
    struct Node* linkedNode;
    enum direction tileDirection;
};

//------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------
extern void generateGameMap();

#endif
