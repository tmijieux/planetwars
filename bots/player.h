#ifndef PLAYER_H
#define PLAYER_H

struct player;

#include "planet.h"
#include "fleet.h"

#define IS_NEUTRAL(p_id) ((p_id) == 0)
#define IS_ALLY(p_id) ((p_id) == 1)
#define IS_ENEMY(p_id) ((p_id) < 0 || (p_id) > 1)
#define IS_ALIEN(p_id) ((p_id) != 1)

struct player {
    int id;
    const char *name;
    int nb_planet;
    int nb_fleet;
    struct planet *planet[500];
    struct fleet *fleet[20000];
};

int player_total_nb_ship(struct player *p);

#endif //PLAYER_H
