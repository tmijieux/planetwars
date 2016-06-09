#ifndef GAME_H
#define GAME_H

#include "player.h"

struct GAME {
    struct Map *map;
    struct Player *P;
    int nb_players;
};

int game_is_live(int nb_players, struct Player P[]);
int game_winner(int nb_players, struct Player P[]);

#endif //GAME_H
