#include "../libtonc/include/tonc.h"
#include "constants.h"
#include "debug.h"
#include "entity.h"
#include "globals.h"
#include "mgba.h"
#include "pauseMenu.h"
#include "tile.h"

//------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------


//------------------------------------------------------------------
// Function: initEntities
// 
// Initializes all entity variables. Should be called upon entrance
// to a new map.
//------------------------------------------------------------------
extern void initEntities()
{
    #ifdef DEBUG_ENTITY
        mgba_printf(MGBA_LOG_DEBUG, "initEntities");
    #endif

    for (int index = 0; index < NUM_MAX_ENTITIES; index++)
    {
        entity[index].facing = DIR_DOWN;
        entity[index].sightRange = SIGHT_RANGE_STANDARD;
        entity[index].lastAction = NO_ACTION;

        // Randomize entity positions
        do
        {
            entity[index].posX = randomInRange(1, MAP_WIDTH_TILES - 1);
            entity[index].posY = randomInRange(1, MAP_HEIGHT_TILES - 1);
        } while (isSolid(entity[index].posX, entity[index].posY));

        #ifdef DEBUG_ENTITY
            mgba_printf(MGBA_LOG_DEBUG, "  index %d", index);
            mgba_printf(MGBA_LOG_DEBUG, "    position: (%d, %d)", entity[index].posX, entity[index].posY);
        #endif
    }
}

//------------------------------------------------------------------
// Function: getEntity
// 
// Returns a pointer to the given entity using its entity[index].
//------------------------------------------------------------------
extern struct Entity* getEntity(const int entityIndex)
{
    return &entity[entityIndex];
}

//------------------------------------------------------------------
// Function: setEntityPos
// 
// Sets the given entity's position to the given coordinates.
//------------------------------------------------------------------
extern void setEntityPos(struct Entity *entity, const int positionX, const int positionY)
{
    #ifdef DEBUG_ENTITY
        mgba_printf(MGBA_LOG_DEBUG, "  setEntityPos");
        mgba_printf(MGBA_LOG_DEBUG, "    currentPos: (%d, %d)", entity->posX, entity->posY);
        mgba_printf(MGBA_LOG_DEBUG, "    newPos (%d, %d)", positionX, positionY);
    #endif

    entity->posX = positionX;
    entity->posY = positionY;
}

//------------------------------------------------------------------
// Function: getEntitySightRange
// 
// Gets the given entity's sight range. Used in checking LOS.
//------------------------------------------------------------------
extern int getEntitySightRange(const struct Entity *entity)
{
    #ifdef DEBUG_ENTITY
        mgba_printf(MGBA_LOG_DEBUG, "  getEntitySightRange: %d", entity->sightRange);
    #endif

    return entity->sightRange;
}

//------------------------------------------------------------------
// Function: setEntitySightRange
// 
// Sets the given entity's sight range.
//------------------------------------------------------------------
extern void setEntitySightRange(struct Entity *entity, const int sightRange)
{
    #ifdef DEBUG_ENTITY
        mgba_printf(MGBA_LOG_DEBUG, "  setEntitySightRange");
        mgba_printf(MGBA_LOG_DEBUG, "    current range: %d", entity->sightRange);
        mgba_printf(MGBA_LOG_DEBUG, "    new range: %d", sightRange);
    #endif

    entity->sightRange = sightRange;
}

//------------------------------------------------------------------
// Function: getEntityFacing
// 
// Gets the given entity's facing direction.
//------------------------------------------------------------------
extern int getEntityFacing(const struct Entity *entity)
{
    #ifdef DEBUG_ENTITY
        mgba_printf(MGBA_LOG_DEBUG, "  getEntityFacing: %d", entity->facing);
    #endif

    return entity->facing;
}

//------------------------------------------------------------------
// Function: setEntityFacing
// 
// Sets the given entity's facing direction.
//------------------------------------------------------------------
extern void setEntityFacing(struct Entity *entity, const enum direction direction)
{
    #ifdef DEBUG_ENTITY
        mgba_printf(MGBA_LOG_DEBUG, "  setEntityFacing");
        mgba_printf(MGBA_LOG_DEBUG, "    currentFacing: %d", entity->facing);
        mgba_printf(MGBA_LOG_DEBUG, "    newFacing: %d", direction);
    #endif

    entity->facing = direction;
}

//------------------------------------------------------------------
// Function: getEntityLastAction
// 
// Gets the given entity's last performed action.
//------------------------------------------------------------------
extern int getEntityLastAction(struct Entity *entity)
{
    #ifdef DEBUG_ENTITY
        mgba_printf(MGBA_LOG_DEBUG, "  getEntityLastAction: %d", entity->lastAction);
    #endif

    return entity->lastAction;
}

//------------------------------------------------------------------
// Function: setEntityLastAction
// 
// Sets the given entity's last performed action.
//------------------------------------------------------------------
extern void setEntityLastAction(struct Entity *entity, const enum entityAction action)
{
    #ifdef DEBUG_ENTITY
        mgba_printf(MGBA_LOG_DEBUG, "  setEntityLastAction");
        mgba_printf(MGBA_LOG_DEBUG, "    previous action: %d", entity->lastAction);
        mgba_printf(MGBA_LOG_DEBUG, "    new action: %d", action);
    #endif

    entity->lastAction = action;
    
}

//------------------------------------------------------------------
// Function: entityWalk
// 
// Makes the given entity perform the "walk" action in the given
// direction.
//------------------------------------------------------------------
extern boolean entityWalk(struct Entity *entity, const enum direction direction)
{
    int targetPosX = entity->posX + dirX[direction];
    int targetPosY = entity->posY + dirY[direction];

    #ifdef DEBUG_ENTITY
        mgba_printf(MGBA_LOG_DEBUG, "entityWalk");
    #endif

    setEntityFacing(entity, direction);

    // Fail conditions
    if ((isSolid(targetPosX, targetPosY) && debugCollisionIsOff == FALSE) || isOutOfBounds(targetPosX, targetPosY))
        return FALSE;

    setEntityPos(entity, targetPosX, targetPosY);
    switch (direction)
    {
    case DIR_LEFT:
        setEntityLastAction(entity, WALKED_LEFT);
        break;
    case DIR_RIGHT:
        setEntityLastAction(entity, WALKED_RIGHT);
        break;
    case DIR_UP:
        setEntityLastAction(entity, WALKED_UP);
        break;
    case DIR_DOWN:
        setEntityLastAction(entity, WALKED_DOWN);
        break;
    default:
    }

    turnOfEntityIndex++;
    return TRUE;
}

//------------------------------------------------------------------
// Function: entityEarthBend
// 
// Makes the given entity perform the "earthbend" action in the direction
// they are facing.
//------------------------------------------------------------------
extern boolean entityEarthBend(struct Entity *entity)
{
    int targetPosX = entity->posX + dirX[entity->facing];
    int targetPosY = entity->posY + dirY[entity->facing];
    int terrainOfTarget = getTileTerrain(targetPosX, targetPosY);

    if (isOutOfBounds(targetPosX, targetPosY))
        return FALSE;

    #ifdef DEBUG_ENTITY
        mgba_printf(MGBA_LOG_DEBUG, "entityEarthBend");
    #endif

    switch (terrainOfTarget)
    {
    case ID_STAIRS:
        return FALSE;
    case ID_WALL:
        setTileTerrain(targetPosX, targetPosY, ID_FLOOR_BIG);
        break;
    default:
        setTileTerrain(targetPosX, targetPosY, ID_WALL);
    }

    setEntityLastAction(entity, EARTH_BEND);

    turnOfEntityIndex++;
    return TRUE;
}

//------------------------------------------------------------------
// Function: doPlayerInput
// 
// Updates the player character based on info read from key_poll()
//------------------------------------------------------------------
extern void doPlayerInput()
{
    struct Entity *player = getEntity(PLAYER_INDEX);

    if ((KEY_EQ(key_hit, KI_LEFT) || KEY_EQ(key_held, KI_LEFT)) && !KEY_EQ(key_held, KI_A)) // Left Key
    {
        #ifdef DEBUG_PLAYER
            mgba_printf(MGBA_LOG_INFO, "pressed LEFT");
        #endif

        // If player walking left doesn't fail
        if (entityWalk(player, DIR_LEFT))
        {
            playerSightId++;

            playerMoveOffsetX = dirX[DIR_RIGHT] * TILE_SIZE;
            screenOffsetX += dirX[DIR_LEFT] * TILE_SIZE;
        }
    }
    else if ((KEY_EQ(key_hit, KI_RIGHT) || KEY_EQ(key_held, KI_RIGHT)) && !KEY_EQ(key_held, KI_A)) // Right Key
    {
        #ifdef DEBUG_PLAYER
            mgba_printf(MGBA_LOG_INFO, "pressed RIGHT");
        #endif

        // If player walking right doesn't fail
        if (entityWalk(player, DIR_RIGHT))
        {
            playerSightId++;

            playerMoveOffsetX = dirX[DIR_LEFT] * TILE_SIZE;
            screenOffsetX += dirX[DIR_RIGHT] * TILE_SIZE;
        }
    }
    else if ((KEY_EQ(key_hit, KI_UP) || KEY_EQ(key_held, KI_UP)) && !KEY_EQ(key_held, KI_A)) // Up Key
    {
        #ifdef DEBUG_PLAYER
            mgba_printf(MGBA_LOG_INFO, "pressed UP");
        #endif

        // If player walking up doesn't fail
        if (entityWalk(player, DIR_UP))
        {
            playerSightId++;

            playerMoveOffsetY = dirY[DIR_DOWN] * TILE_SIZE;
            screenOffsetY += dirY[DIR_UP] * TILE_SIZE;
        }
    }
    else if ((KEY_EQ(key_hit, KI_DOWN) || KEY_EQ(key_held, KI_DOWN)) && !KEY_EQ(key_held, KI_A)) // Down Key
    {
        #ifdef DEBUG_PLAYER
            mgba_printf(MGBA_LOG_INFO, "pressed DOWN");
        #endif

        // If player walking down doesn't fail
        if (entityWalk(player, DIR_DOWN))
        {
            playerSightId++;

            playerMoveOffsetY = dirY[DIR_UP] * TILE_SIZE;
            screenOffsetY += dirY[DIR_DOWN] * TILE_SIZE;
        }
    }
    if (KEY_EQ(key_held, KI_A))
    {
        // Player may change facing direction without using turn
        if (KEY_EQ(key_hit, KI_LEFT))
            setEntityFacing(player, DIR_LEFT);
        else if (KEY_EQ(key_hit, KI_RIGHT))
            setEntityFacing(player, DIR_RIGHT);
        else if (KEY_EQ(key_hit, KI_UP))
            setEntityFacing(player, DIR_UP);
        else if (KEY_EQ(key_hit, KI_DOWN))
            setEntityFacing(player, DIR_DOWN);
    }
    if (KEY_EQ(key_hit, KI_B))
    {
        #ifdef DEBUG_PLAYER
            mgba_printf(MGBA_LOG_INFO, "pressed B");
        #endif

        entityEarthBend(player);
        playerSightId++;
    }
    if (KEY_EQ(key_hit, KI_SELECT))
    {
        #ifdef DEBUG_PLAYER
            mgba_printf(MGBA_LOG_INFO, "pressed SELECT");
        #endif
        doStateTransition(STATE_TITLE_SCREEN);
    }
    if (KEY_EQ(key_hit, KI_START))
    {
        #ifdef DEBUG_PLAYER
            mgba_printf(MGBA_LOG_INFO, "pressed START");
        #endif
        doStateTransition(STATE_MENU);
    }
    if (KEY_EQ(key_hit, KI_R))
    {
        
    }
    if (KEY_EQ(key_hit, KI_L))
    {
        
    }
}
