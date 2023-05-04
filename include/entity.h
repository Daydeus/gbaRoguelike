#ifndef ENTITY_H
#define ENTITY_H

//------------------------------------------------------------------
// Data Structures
//------------------------------------------------------------------
struct Entity
{
    uint8_t posX, posY;
    uint8_t sightRange;
    enum direction facing;
    enum entityAction lastAction;
};
extern struct Entity entity[NUM_MAX_ENTITIES];

//------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------
extern void initEntities();
extern struct Entity* getEntity(const int entityIndex);
extern void setEntityPos(struct Entity *entity, const int positionX, const int positionY);
extern int getEntitySightRange(const struct Entity *entity);
extern void setEntitySightRange(struct Entity *entity, const int sightRange);
extern int getEntityFacing(const struct Entity *entity);
extern void setEntityFacing(struct Entity *entity, const enum direction direction);
extern int getEntityLastAction(struct Entity *entity);
extern void setEntityLastAction(struct Entity *entity, const enum entityAction action);
extern void doPlayerInput();

// Entity Actions
extern boolean entityWalk(struct Entity *entity, const enum direction direction);
extern boolean entityEarthBend(struct Entity *entity);

#endif
