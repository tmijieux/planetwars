#include <stdlib.h>
#include "../bot.h"

static void play(struct bot *b, struct game *g)
{
    struct player *me = &g->player[1];
    double score = 0., srcScore = 0., dstScore = 0.;
    struct planet *src, *dst;

    // (1) If we current have a fleet in flight,
    // just do nothing.
    if (me->nb_fleet >= 1)
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
    for (int p_id = 0; p_id < g->nb_player; ++p_id) {
        if (p_id == 1)
            continue;
        struct player *pl = &g->player[p_id];
        for (int i = 0; i < pl->nb_planet; ++i) {
            struct planet *p = pl->planet[i];
            
            score = ((double)p->growth_rate + 1) / (p->nb_ship + 1);
            if (score > dstScore) {
                dstScore = score;
                dst = p;
            }
        }
    }

    // (4) Send half the ships from my strongest
    // planet to the weakest planet that I do not own.
    if (src != NULL && dst != NULL) {
	int nbShips = src->nb_ship / 2;
	bot_send_order(src->id, dst->id, nbShips);
    }
}

static struct bot prospector_bot = {
    .name = "prospector_bot",
    .desc = "a prospecting bot",
    .play = play
};

register_bot(prospector_bot);
