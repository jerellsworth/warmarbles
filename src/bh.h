#ifndef BH_H
#define BH_H

#include <genesis.h>
#include <maths.h>
#include <memory.h>

typedef enum GameState_e GameState;
typedef enum PhysicsType_e PhysicsType;
typedef struct Board_s Board;
typedef struct Game_s Game;
typedef struct Physics_s Physics;
typedef struct Player_s Player;
typedef struct Guy_s Guy;
typedef struct Menu_Item_s Menu_Item;
typedef struct Menu_s Menu;
typedef int (*Menu_Select_CB)(Menu_Item *);

#include "resources.h"

#include "board.h"
#include "game.h"
#include "physics.h"
#include "player.h"
#include "guy.h"
#include "menu.h"
#include "utils.h"

#endif
