#include <stdlib.h>
#include <string.h>
#include <tonc.h>
#include "constants.h"
#include "globals.h"
#include "mapGeneration.h"
#include "mgba.h"
#include "tile.h"

//------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------
static void initGameMap();
static bool placeRoom(int const startingX, int const startingY, int const width, int const height);
static void carveMaze();
static void ensureMapBoundarySolid();
static void placeStairs();
static struct Tile* getUnmarkedTile(struct Tile* const tile);
static void addEndNode(struct Node* const listHead, struct Tile *tile);
static void markEndNode(struct Node* const listHead);
static struct Node* deleteEndNode(struct Node *listHead);
static struct Node* deleteAllNodes(struct Node *listHead);
static u32 getNodeCount(struct Node* const listHead);

//------------------------------------------------------------------
// Function: initGameMap
// 
// Initializes the gameMap by assigning the tiles' positions and
// other variables.
//------------------------------------------------------------------
static void initGameMap()
{
    for (int y = 0; y <= MAP_HEIGHT_TILES - 1; y++)
    {
        for (int x = 0; x <= MAP_WIDTH_TILES - 1; x++)
        {
            initTilePosition(x, y);
            setTileTerrain(x, y, ID_WALL);
            setTileSight(x, y, TILE_NEVER_SEEN);
        }
    }

    #ifdef DEBUG_MAP_GEN
        mgba_printf(MGBA_LOG_DEBUG, "initGameMap");
    #endif
}

//------------------------------------------------------------------
// Function: placeRoom
// 
// From the given starting position, forms a rectangle with the given
// width and height. All tiles within the rectangle have their terrainId
// set to ID_FLOOR.
//------------------------------------------------------------------
static bool placeRoom(int const startingX, int const startingY, int const width, int const height)
{
    #ifdef PRINT_ROOM_PLACEMENT
        mgba_printf(MGBA_LOG_DEBUG, "placeRoom: (%d, %d), width: %d, height: %d", startingX, startingY, width, height);
    #endif

    // Check if room placement should fail (room outside map or overlapping another room)
    if (isOutOfBounds(startingX, startingY) || isOutOfBounds(startingX + width, startingY + height)
    || getTileTerrain(startingX, startingY) != ID_WALL
    || getTileTerrain(startingX, startingY + height) != ID_WALL
    || getTileTerrain(startingX + width, startingY) != ID_WALL
    || getTileTerrain(startingX + width, startingY + height) != ID_WALL)
    {
        #ifdef PRINT_ROOM_PLACEMENT
            mgba_printf(MGBA_LOG_DEBUG, "    placeRoom FAILED");
        #endif

        return false;
    }

    // Turn all tiles in rectangle to floor tiles
    for (int y = startingY; y < startingY + height; y++)
    {
        for (int x = startingX; x < startingX + width; x++)
        {
            setTileTerrain(x, y, ID_FLOOR);
        }
    }

    #ifdef PRINT_ROOM_PLACEMENT
        mgba_printf(MGBA_LOG_DEBUG, "    placeRoom SUCCEEDED");
    #endif

    return true;
}

//------------------------------------------------------------------
// Function: carveMaze
// 
// Carves a maze out of a map filled with wall tiles.
//------------------------------------------------------------------
static void carveMaze()
{
    struct Tile *currentTile = NULL;
    struct Node *listHead = (struct Node*)malloc(sizeof(struct Node));

    #ifdef DEBUG_MAP_GEN
        int nodeCount = 0, highestNodeCount = 0;
    #endif

    // Set the first node's tile
    listHead->tile = getTile(randomInRange(1, MAP_WIDTH_TILES - 2), randomInRange(1, MAP_HEIGHT_TILES - 2));

    // Randomize the starting tile
    while (!isSolid(listHead->tile->posX, listHead->tile->posY))
        listHead->tile = getTile(randomInRange(1, MAP_WIDTH_TILES - 2), randomInRange(1, MAP_HEIGHT_TILES - 2));

    // Initialize starting node's tile data
    listHead->tile->terrainId = ID_FLOOR;
    listHead->linkedNode = NULL;
    listHead->tileDirection = DIR_NULL;

    #ifdef DEBUG_MAP_GEN
        mgba_printf(MGBA_LOG_DEBUG, "carveMaze START");
    #endif

    // While there are still nodes(tiles) to check
    while (listHead != NULL)
    {
        #ifdef PRINT_MAZE_MARKING
            mgba_printf(MGBA_LOG_DEBUG, "carveMaze TOP OF LOOP");
        #endif

        // Iterate endNode until it points at the end of the list
        struct Node *endNode = listHead;
        while (endNode->linkedNode != NULL)
            endNode = endNode->linkedNode;

        #ifdef PRINT_MAZE_MARKING
            if (currentTile == NULL)
            {
                mgba_printf(MGBA_LOG_DEBUG, "currentTile is NULL");
                mgba_printf(MGBA_LOG_DEBUG, "previous linkedNode->tile is (%d, %d)", endNode->tile->posX, endNode->tile->posY);
            }
            else
                mgba_printf(MGBA_LOG_DEBUG, "currentTile is (%d, %d)", currentTile->posX, currentTile->posY);
        #endif

        // Set currentTile to be one of endNode's tile's unmarked targets
        currentTile = getUnmarkedTile(endNode->tile);

        // If endNode's tile has no unmarked targets
        if (currentTile == NULL)
        {
            // Delete endNode(and endNode's tile) from list
            listHead = deleteEndNode(listHead);
        }
        else
        {
            // Create a new end node and set currentTile to be its tile
            addEndNode(listHead, currentTile);
            markEndNode(listHead);
        }

        #ifdef PRINT_MAZE_MARKING
            nodeCount = getNodeCount(listHead);
            mgba_printf(MGBA_LOG_DEBUG, "Current node count: %d\n", nodeCount);
            if (nodeCount > highestNodeCount)
                highestNodeCount = nodeCount;
        #endif
    }

    #ifdef DEBUG_MAP_GEN
        mgba_printf(MGBA_LOG_DEBUG, "carveMaze END");
        #ifdef PRINT_MAZE_MARKING
            mgba_printf(MGBA_LOG_DEBUG, "Highest node count: %d", highestNodeCount);
        #endif
    #endif
}

//------------------------------------------------------------------
// Function: ensureMapBoundarySolid
// 
// At each boundary of the game map, place a wall to ensure that the
// player cannot leave the map.
//------------------------------------------------------------------
static void ensureMapBoundarySolid()
{
    // Top Boundary: Coord (0, 0) to (MAP_WIDTH_TILES - 1, 0)
    for (int x = 0; x <= MAP_WIDTH_TILES - 1; x++)
        setTileTerrain(x, 0, ID_WALL);

    // Bottom Boundary: Coord (0, MAP_HEIGHT_TILES - 1) to (MAP_WIDTH_TILES - 1, MAP_HEIGHT_TILES - 1)
    for (int x = 0; x <= MAP_WIDTH_TILES - 1; x++)
        setTileTerrain(x, MAP_HEIGHT_TILES - 1, ID_WALL);

    // Left Boundary: Coord (0, 0) to (0, MAP_HEIGHT_TILES - 1)
    for (int y = 0; y <= MAP_HEIGHT_TILES - 1; y++)
        setTileTerrain(0, y, ID_WALL);

    // Right Boundary: Coord (MAP_WIDTH_TILES - 1, 0) to (MAP_WIDTH_TILES - 1, MAP_HEIGHT_TILES - 1)
    for (int y = 0; y <= MAP_HEIGHT_TILES - 1; y++)
        setTileTerrain(MAP_WIDTH_TILES - 1, y, ID_WALL);
}

//------------------------------------------------------------------
// Function: placeStairs
// 
// Place the tile that allows transition to the next map.
//------------------------------------------------------------------
static void placeStairs()
{
        int positionX = 0;
        int positionY = 0;

    do
    {
        positionX = randomInRange(1, MAP_WIDTH_TILES - 1);
        positionY = randomInRange(1, MAP_HEIGHT_TILES - 1);

        if (!isSolid(positionX, positionY))
        {
            setTileTerrain(positionX, positionY, ID_STAIRS);

            #ifdef DEBUG_MAP_GEN
                mgba_printf(MGBA_LOG_DEBUG, "placeStairs: (%d, %d)", positionX, positionY);
            #endif
        }
    } while (isSolid(positionX, positionY));
}

//------------------------------------------------------------------
// Function: getUnmarkedTile
// 
// Returns a tile two spaces in a random cardinal direction from the
// given tile that hasn't been marked yet.
//------------------------------------------------------------------
static struct Tile* getUnmarkedTile(struct Tile* const tile)
{
    int positionX = 0, positionY = 0, direction = randomInRange(1, 4);
    bool checkedLeft = false, checkedRight = false, checkedUp = false, checkedDown = false;

    // Loop until every cardinal direction is checked
    while (!checkedLeft || !checkedRight || !checkedUp || !checkedDown)
    {
        // Move two tiles at a time to ensure we don't just remove every wall
        positionX = tile->posX + dirX[direction] * 2;
        positionY = tile->posY + dirY[direction] * 2;

        #ifdef PRINT_MAZE_MARKING
            mgba_printf(MGBA_LOG_DEBUG, "    getUnmarkedTile checking direction: %d", direction);
            mgba_printf(MGBA_LOG_DEBUG, "    getUnmarkedTile checking tile: (%d, %d)", positionX, positionY);
        #endif

        // Return tile if in bounds and unmarked
        if (!isOutOfBounds(positionX, positionY) && getTileTerrain(positionX, positionY) == ID_WALL)
        {
            #ifdef PRINT_MAZE_MARKING
                mgba_printf(MGBA_LOG_DEBUG, "    getUnmarkedTile returned value: (%d, %d)", positionX, positionY);
            #endif

            return getTile(positionX, positionY);
        }

        // Set direction checked
        switch (direction)
        {
        case DIR_LEFT: checkedLeft = true;    break;
        case DIR_RIGHT: checkedRight = true;  break;
        case DIR_UP: checkedUp = true;        break;
        case DIR_DOWN: checkedDown = true;    break;
        default:
        }

        // Pick new random direction
        direction = randomInRange(1, 4);
    }

    #ifdef PRINT_MAZE_MARKING
        mgba_printf(MGBA_LOG_DEBUG, "    getUnmarkedTile returned NULL.");
    #endif

    // No unmarked tiles found
    return NULL;
}

//------------------------------------------------------------------
// Function: addEndNode
// 
// Adds a new node at the end of the given linked list.
//------------------------------------------------------------------
static void addEndNode(struct Node* listHead, struct Tile* tile)
{
    struct Node *currentEnd = NULL, *newEnd = NULL;

    // Placeholder variables used to find newEnd's tileDirection
    int startX = 0, startY = 0, endX = 0, endY = 0;

    // Iterate currentEnd until it points at the end of the list
    currentEnd = listHead;
    while (currentEnd->linkedNode != NULL)
    {
        currentEnd = currentEnd->linkedNode;
    }

    // Store new end node's data
    newEnd = (struct Node*)malloc(sizeof(struct Node));
    newEnd->tile = tile;
    newEnd->linkedNode = NULL;
    
    // Set placeholder variables for finding tileDirection
    startX = currentEnd->tile->posX;
    startY = currentEnd->tile->posY;
    endX = newEnd->tile->posX;
    endY = newEnd->tile->posY;

    // Store newEnd's tileDirection
    newEnd->tileDirection = getTileDirection(startX, startY, endX, endY);

    // Link new end node to the end of the list
    currentEnd->linkedNode = newEnd;
}

//------------------------------------------------------------------
// Function: markEndNode
// 
// As function getUnmarkedTile checks tiles two spaces away, it skips
// over a tile as it moves along its path.
// 
// This function (markEndNode) marks tiles by setting their terrainId
// to ID_FLOOR. It does this for both the end node's tile and the
// skipped-over tile by checking backwards in the end node's tileDirection.
//------------------------------------------------------------------
static void markEndNode(struct Node* listHead)
{
    int skippedX = 0, skippedY = 0;
    struct Node *endNode = listHead;

    // Iterate endNode until it points at the end of the list
    while (endNode->linkedNode != NULL)
        endNode = endNode->linkedNode;

    // Set the end node's tile's terrainId
    endNode->tile->terrainId = ID_FLOOR;

    #ifdef PRINT_MAZE_MARKING
        mgba_printf(MGBA_LOG_DEBUG, "    markEndNode endNode tile: (%d, %d)", endNode->tile->posX, endNode->tile->posY);
    #endif

    // Get the skipped-over tile's position
    skippedX = endNode->tile->posX - dirX[endNode->tileDirection];
    skippedY = endNode->tile->posY - dirY[endNode->tileDirection];

    // Set the skipped-over tile's terrainId
    setTileTerrain(skippedX, skippedY, ID_FLOOR);

    #ifdef PRINT_MAZE_MARKING
        mgba_printf(MGBA_LOG_DEBUG, "    markEndNode skipped-over tile: (%d, %d)", skippedX, skippedY);
    #endif
}

//------------------------------------------------------------------
// Function: deleteEndNode
// 
// Deletes the node at the end of the given linked list.
//------------------------------------------------------------------
static struct Node* deleteEndNode(struct Node *listHead)
{
    // If listHead is NULL, there is no linked list
    if (listHead == NULL)
    {
        #ifdef PRINT_MAZE_MARKING
            mgba_printf(MGBA_LOG_DEBUG, "    In function deleteEndNode: listHead = NULL");
        #endif
    }
    // If listHead has no linkedNode, then it is the end node and
    // we are deleting the linked list
    else if (listHead->linkedNode == NULL)
    {
        free(listHead);
        listHead = NULL;

        #ifdef PRINT_MAZE_MARKING
            mgba_printf(MGBA_LOG_DEBUG, "    In function deleteEndNode: listHead DELETED");
        #endif
    }
    else
    {
        struct Node *endNode = listHead, *penUltNode = listHead;

        // We need to set the penultimate node's linkedNode to NULL and free the end node
        while (endNode->linkedNode != NULL)
        {
            penUltNode = endNode;
            endNode = endNode->linkedNode;
        }

        // Set the penultimate node's link to NULL
        penUltNode->linkedNode = NULL;

        // Now we can safely free the end node
        free(endNode);
        endNode = NULL;
    }

    // If listHead is NULL, we will know that the linked list was deleted
    return listHead;
}

//------------------------------------------------------------------
// Function: deleteAllNodes
// 
// Delete all nodes in the given linked list.
//------------------------------------------------------------------
static struct Node* deleteAllNodes(struct Node *listHead)
{
    struct Node *temp = listHead;

    // If listHead is NULL, there is no linked list
    if (listHead == NULL)
    {
        #ifdef PRINT_MAZE_MARKING
            mgba_printf(MGBA_LOG_DEBUG, "    In function deleteAllNodes: listHead = NULL");
        #endif
    }

    // temp holds the reference to the next node while we free listHead,
    // then we set listHead equal to temp and jump temp forward again
    // When temp is NULL, the list has been deleted
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
static u32 getNodeCount(struct Node* listHead)
{
    struct Node *temp = listHead;
    u32 nodeCount = 0;

    // If listHead is NULL, there is no linked list
    if (listHead == NULL)
    {
        #ifdef PRINT_MAZE_MARKING
            mgba_printf(MGBA_LOG_DEBUG, "    In function countNodes: listHead = NULL");
        #endif

        return nodeCount;
    }

    // Count nodes
    while (temp != NULL)
    {
        nodeCount++;
        temp = temp->linkedNode;
    }

    return nodeCount;
}

//------------------------------------------------------------------
// Function: generateGameMap
// 
// Fills the gameMap[][] with content for the player to interact with.
//------------------------------------------------------------------
extern void generateGameMap()
{
    int placeRoomFailures = 0;

    #ifdef DEBUG_MAP_GEN
        mgba_printf(MGBA_LOG_DEBUG, "createGameMap");
    #endif

    // Set starting values for gameMap[][]
    initGameMap();

    // Place Rooms
    while (placeRoomFailures < 20)
    {
        int startingX = randomInRange(1, MAP_WIDTH_TILES - 1), startingY = randomInRange(1, MAP_HEIGHT_TILES - 1);
        int width = randomInRange(4, 12), height = randomInRange(4, 12);

        // If placeRoom failed (due to outOfBounds or overlapping)
        if (!placeRoom(startingX, startingY, width, height))
        {
            placeRoomFailures++;

            #ifdef PRINT_ROOM_PLACEMENT
                mgba_printf(MGBA_LOG_DEBUG, "placeRoomFailures: %d\n", placeRoomFailures);
            #endif
        }
    }

    // Carve a maze in the space between rooms
    carveMaze();

    // Diversify floor tiles
    for (int y = 1; y < MAP_HEIGHT_TILES - 1; y++)
    {
        for (int x = 1; x < MAP_WIDTH_TILES - 1; x++)
        {
            if (getTileTerrain(x, y) == ID_FLOOR)
            {
                switch(randomInRange(0, 5))
                {
                case 1: 
                case 2: setTileTerrain(x, y, ID_FLOOR_MOSSY);   break;
                case 3: setTileTerrain(x, y, ID_FLOOR_BIG);     break;
                case 4: setTileTerrain(x, y, ID_FLOOR_CHIP);    break;
                }
            }
        }
    }

    ensureMapBoundarySolid();
    placeStairs();

    #ifdef PRINT_MAP
        for (int y = 0; y < MAP_HEIGHT_TILES; y++)
        {
            char mapRow[MAP_WIDTH_TILES + 1];
            for (int c = 0; c < MAP_WIDTH_TILES + 1; c++)
                mapRow[c] = '\0';

            for (int x = 0; x < MAP_WIDTH_TILES; x++)
            {
                switch (getTile(x, y)->terrainId)
                {
                case ID_FLOOR:
                case ID_FLOOR_MOSSY:
                case ID_FLOOR_BIG:
                case ID_FLOOR_CHIP:
                    strcat(mapRow, ".");
                    break;
                case ID_WALL:
                case ID_WALL_FRONT:
                    strcat(mapRow, "#");
                    break;
                case ID_STAIRS:
                    strcat(mapRow, "S");
                    break;
                default:
                    strcat(mapRow, "?");
                    break;
                }

            }
            mgba_printf(MGBA_LOG_INFO, mapRow);
        }
    #endif
}
