/**
 * @file
 * @brief Contains the implementation of the strategy
 *
 * RANDOM_BOT kinda random! 
 *
 */

#include <stdlib.h>
#include "../bot.h"

static void play(struct bot *b, struct game *g)
{
    struct planet *src = NULL, *dst = NULL;
    struct player *me = &g->player[1];
    
    // (1) If we have more than one fleets actually flying, do nothing
    if (me->nb_fleet >= 1)
	return;

    // (2) Pick one of my planets at random.
    if (me->nb_planet > 0) {
	int k = rand() % me->nb_planet;
	src = me->planet[k];
    }
	
    // (3) Pick a target planet at random.
    if (g->nb_planet > 0) {
	int k = rand() % g->nb_planet;
	dst = &g->planet[k];
    }

    // (4) Send half the ships from source to dest.
    if (src != NULL && dst != NULL) {
	int nb_ship = src->nb_ship / 2;
	bot_send_order(src->id, dst->id, nb_ship);
    }
}

static struct bot randombot = {
    .name = "random",
    .desc = "a random bot",
    .play = play
};

register_bot(randombot);
