/**
 * @file
 * @brief prototypes of function used in strategies
 */
#ifndef BOT_H
#define BOT_H

#include <stdlib.h>

struct bot;
extern struct bot *bots;

#include "uthash.h"
#include "game.h"

#define SQUARE(X)  ((X)*(X))

struct bot {
    const char *name;
    const char *desc;
    UT_hash_handle hh;
    void (*play)(struct bot *b, struct game*);
};

#define register_bot(bot_v)                                           \
    __attribute__((constructor)) static void register_bot##__COUNTER__(void) \
    { HASH_ADD_STR(bots, name, (&(bot_v))); }

int nb_planets(struct game *pw, int playerId);
int nb_fleets(struct game *pw, int playerId);
int is_already_fleet(struct game *pw, struct planet *src,
                     struct planet *dst, int ships);

float euc_distance(struct planet *pl1, struct planet *pl2);
int turn_distance(struct planet *pSrc, struct planet *pDst);

struct planet *strongest_planet_of(struct game *pw, struct player *p);
struct planet *weakest_planet_of(struct game *pw, struct player *p);

/*
  int my_weakest_planet(struct game *pw, int *ms);
  int enemy_weakest_planet(struct game *pw, int *ms);
  int neutral_weakest_planet(struct game *pw, int *min_ships);
*/

struct planet *nearest_foreign_planet_lower(
    struct game *pw, struct planet *src, int maxShips);
struct planet *nearest_foreign_planet_i_can_capture_from(
    struct game *pw, struct planet *src);
struct planet *nearest_foreign_planet_i_may_capture(
    struct game *pw, struct planet *src);
int fleet_on_planet(struct game *pw, struct planet *dst, int t);
int to_be_captured_soon(struct game *pw, struct planet *dst);
struct planet *random_planet_owned_by(struct game *pw, struct player *);
struct planet *random_foreign_planet(struct game *pw);
int production(struct planet *planets[], int nbplanets);
struct planet *weakest_alien_planet(struct game *pw);
void bot_send_order(int source, int destination, int nb_ship);

#endif //BOT_H
