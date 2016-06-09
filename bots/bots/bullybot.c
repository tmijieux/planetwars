/**
 * @file
 * @brief Contains the implementation of the strategy
 *
 * BULLY_BOT is a bad guy
 *
 */

#include <stdlib.h>
#include "../bot.h"

static void play(struct bot *b, struct game *g)
{
    double score = 0., srcScore = 0., dstScore = 0.;
    struct player *alien = &g->player[-1];
    struct player *me = &g->player[1];
    struct planet *src = NULL, *dst = NULL;
	
    // (1) If we current have a fleet in flight,
    // just do nothing.
    if (me->nb_fleet >= 1)
	return;

    // (2) Find my strongest planet.
    for (int i = 0; i < me->nb_planet; i++) {
	score = (double) me->planet[i]->nb_ship;
	if (score > srcScore) {
	    srcScore = score;
	    src = me->planet[i];
	}
    }
	
    // (3) Find the weakest enemy or neutral planet.
   for (int i = 0; i < alien->nb_planet; i++) {
	score = (double) 1.0 / alien->planet[i]->nb_ship;
	if (score > dstScore) {
	    dstScore = score;
	    dst = alien->planet[i];
	}
    }
	
    // (4) Send half the ships from my strongest
    // planet to the weakest planet that I do not own.
    if (src != NULL && dst != NULL) {
	int nb_ship = src->nb_ship / 2;
	bot_send_order(src->id, dst->id, nb_ship);
    }
}

static struct bot bully = {
    .name = "bully",
    .desc = "a bad villain bully",
    .play = play
};

register_bot(bully);
