#ifndef GAME_H
#define GAME_H

struct game;

#include "planet.h"
#include "fleet.h"
#include "player.h"
#include "bot.h"

struct game {
    int turn_count;
    int nb_player;
    int nb_planet;
    int nb_fleet;
    struct player *player;
    struct planet planet[500];
    struct fleet fleet[20000];
};

void game_init(struct game *g);
void game_turn_reinit(struct game *g);
void game_run(struct game *g, struct bot *b);

#endif //GAME_H
