#include "bh.h"

#define PHYSICS_MAX_OBJECTS

struct Physics_s {
    fix16 x;
    fix16 y;
};

extern Physics **ALL_PHYSICS;

Physics *Physics_init(void);
void Physics_del(Physics *p);
