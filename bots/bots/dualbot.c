/**
 * @file
 * @brief Contains the implementation of the strategy
 *
 * DUAL_BOT is rather clever, but not enough :]
 *
 */

#include <stdlib.h>
#include "../bot.h"

void play(struct bot *b, struct game *g)
{
    int nbFleets = 1, attackMode = 0;
    double score = 0., dstScore = 0., srcScore = 0.;
    struct player *enemy = &g->player[2];
    struct player *alien = &g->player[-1];
    struct player *me = &g->player[1];
    struct planet *src = NULL, *dst = NULL;

    // total nb of ships here i think
    if (me->nb_fleet > enemy->nb_fleet) {
	if (production(me->planet, me->nb_planet) >
	    production(enemy->planet, enemy->nb_planet)) {
	    nbFleets = 1;
	    attackMode = 1;
	} else {
	    nbFleets = 3;
	}
    } else {
        if (production(me->planet, me->nb_planet) >
	    production(enemy->planet, enemy->nb_planet)) {
	    nbFleets = 1;
	} else {
	    nbFleets = 5;
	}
    }
	
    // (1) If we current have more than nbFleets fleets in flight,
    // just do nothing until at least one of the fleets arrives.
    if (me->nb_fleet >= nbFleets)
	return;
	
    // (2) Find my strongest planet.
    for (int i = 0; i < me->nb_planet; i++) {
        struct planet *p = me->planet[i];
	score = (double) p->nb_ship / (1 + p->growth_rate);
	if (score > srcScore) {
	    srcScore = score;
	    src = p;
	}
    }
    
    // (3) Find the weakest enemy or neutral planet.
    struct planet **tab = alien->planet;
    int k = alien->nb_planet;
    if (attackMode) {
	tab = enemy->planet;
        k = enemy->nb_planet;
    }
    
    // (3) Find the weakest enemy or neutral planet.
    for (int i = 0; i < k; i++) {
	score = ((double)tab[i]->growth_rate + 1) /  (tab[i]->nb_ship + 1);
	if (score > dstScore) {
	    dstScore = score;
	    dst = tab[i];
	}
    }
    
    // (4) Send half the ships from my strongest
    // planet to the weakest planet that I do not own.
    if (src != NULL && dst != NULL) {
	int nb_ship = src->nb_ship / 2;
	bot_send_order(src->id, dst->id, nb_ship);
    }
}

static struct bot dual = {
    .name = "dualbot",
    .desc = "a intelligent bot adapting its behaviour to its enemy",
    .play = play
};

register_bot(dual);
