#include <stdlib.h>
#include <tonc.h>
#include "constants.h"
#include "gameMap.h"
#include "globals.h"
#include "mapGeneration.h"
#include "mgba.h"

//------------------------------------------------------------------
// Function: addEndNode
// 
// Adds a new node at the end of the given linked list.
//------------------------------------------------------------------
void addEndNode(struct Node* listHead, struct Tile* tile)
{
    struct Node *currentEnd, *newEnd;

    // Iterate currentEnd until it points at the current end node
    currentEnd = listHead;
    while (currentEnd->linkedNode != NULL)
    {
        currentEnd = currentEnd->linkedNode;
    }

    newEnd = (struct Node*)malloc(sizeof(struct Node));
    newEnd->tile = tile;
    newEnd->linkedNode = NULL;

    currentEnd->linkedNode = newEnd;
}

//------------------------------------------------------------------
// Function: deleteEndNode
// 
// Deletes the node at the end of the linked list.
//------------------------------------------------------------------
void deleteEndNode(struct Node *listHead)
{
    if (listHead == NULL)
    {
        #ifdef DEBUG_MAP_GEN
            mgba_printf(MGBA_LOG_DEBUG, "In function deleteEndNode: listHead = NULL");
        #endif
    }
    else if (listHead->linkedNode == NULL)
    {
        free(listHead);
        listHead = NULL;
        #ifdef DEBUG_MAP_GEN
            mgba_printf(MGBA_LOG_DEBUG, "In function deleteEndNode: deleted listHead");
        #endif
    }
    else
    {
        struct Node *temp = listHead, *temp2 = listHead;

        while (temp->linkedNode != NULL)
        {
            temp2 = temp;
            temp = temp->linkedNode;
        }
        temp2->linkedNode = NULL;
        free(temp);
        temp = NULL;
    }
}

//------------------------------------------------------------------
// Function: deleteAllNodes
// 
// Delete all nodes in the given linked list.
//------------------------------------------------------------------
struct Node* deleteAllNodes(struct Node *listHead)
{
    struct Node *temp = listHead;
    if (listHead == NULL)
    {
        #ifdef DEBUG_MAP_GEN
            mgba_printf(MGBA_LOG_DEBUG, "In function deleteAllNodes: listHead = NULL");
        #endif
    }
    while (temp != NULL)
    {
        temp = temp->linkedNode;
        free(listHead);
        listHead = temp;
    }
    return listHead;
}

//------------------------------------------------------------------
// Function: getNodeCount
// 
// Count and return the number of nodes in the given linked list.
//------------------------------------------------------------------
u32 getNodeCount(struct Node* listHead)
{
    u32 nodeCount = 0;
    if (listHead == NULL)
    {
        #ifdef DEBUG_MAP_GEN
            mgba_printf(MGBA_LOG_DEBUG, "In function countNodes: listHead = NULL");
        #endif
        return nodeCount;
    }
    struct Node *temp = NULL;
    temp = listHead;
    while (temp != NULL)
    {
        nodeCount++;
        temp = temp->linkedNode;
    }
    return nodeCount;
}
