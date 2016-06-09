/**
 * @file
 * @brief Contains the implementation of the strategy
 *
 * RANDOM2_BOT only target its enemy! (still random, but stronk random!)
 *
 */

#include <stdlib.h>
#include "../bot.h"

static void play(struct bot *b, struct game *g)
{
    struct player *me = &g->player[1];
    struct player *enemy = &g->player[2];
    struct planet *src = NULL, *dst = NULL;

    // (1) If we have more than one fleets actually flying,
    // do nothing
    if (me->nb_fleet >= 1)
	return;
    
    // (2) Pick one of my planets at random.
    if (me->nb_planet > 0) {
	int k = rand() % me->nb_planet;
	src = me->planet[k];
    }
    
    // (3) Pick a target enemy planet at random.
    if (enemy->nb_planet > 0) {
	int k = rand() % enemy->nb_planet;
	dst = enemy->planet[k];
    }
	
    // (4) Send half the ships from source to dest.
    if (src != NULL && dst != NULL) {
	int nb_ship = src->nb_ship / 2;
	bot_send_order(src->id, dst->id, nb_ship);
    }
}

static struct bot random2 = {
    .name = "random2",
    .desc = "a random bot only attacking its enemy",
    .play = play
};

register_bot(random2);
