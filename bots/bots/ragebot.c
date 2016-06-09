/**
 * @file
 * @brief Contains the implementation of the strategy
 * RAGE_BOT is strong ! a bit retarded though ...
 */

#include <stdlib.h>
#include <limits.h>
#include "../bot.h"

static void play(struct bot *b, struct game *g)
{
    struct player *me = &g->player[1];
    struct player *enemy = &g->player[2];
    struct planet *src = NULL, *dst = NULL;

    for (int i = 0; i < me->nb_planet; i++) {
	src = me->planet[i];

        // 1. select planets which have 'lots of ships'
	if (src->nb_ship < src->growth_rate * 10)
	    continue;
        // 2. find the closest enemy planet
        int dstDist = INT_MAX;
	for (int j = 0; j < enemy->nb_planet; j++) {
	    int dist = euc_distance(src, enemy->planet[j]);
	    if (dist < dstDist) {
		dstDist = dist;
		dst = enemy->planet[j];
	    }
	}
	if (src && dst) {
            // 3. send all the ships
	    bot_send_order(src->id, dst->id, src->nb_ship);
	}
    }
}

static struct bot rage = {
    .name = "ragebot",
    .desc = "a rather coleric bot",
    .play = play
};

register_bot(rage);
