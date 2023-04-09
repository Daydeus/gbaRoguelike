#ifndef MAP_GEN_H
#define MAP_GEN_H

//------------------------------------------------------------------
// Data Structures
//------------------------------------------------------------------
struct Node
{
    struct Tile* tile;
    struct Node* linkedNode;
};

//------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------
void addEndNode(struct Node *listHead, struct Tile *tile);
void deleteEndNode(struct Node *listHead);
struct Node* deleteAllNodes(struct Node *listHead);
u32 getNodeCount(struct Node* listHead);

#endif
