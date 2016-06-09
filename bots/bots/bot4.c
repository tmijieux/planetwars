#include <stdlib.h>
#include "../bot.h"

static void play(struct bot *b, struct game *g)
{
    int i, k, dis, ships;
    struct player *me = &g->player[1];
    struct planet *src, *dst, *p;
    float score = 0., bestScore = 0.;

    if (me->nb_planet == 0)
        return;
    int nbMyShips = player_total_nb_ship(me);
    int nbEnemyShips =  player_total_nb_ship(&g->player[2]);
    
    if (nbMyShips >(int) 4. * nbEnemyShips / 3)
        k = 13;
    else if (nbMyShips >= nbEnemyShips)
        k = 10;
    else if (nbEnemyShips > 5*nbMyShips)
        k = 2;
    else if (nbEnemyShips > 2*nbMyShips)
        k = 5;
    else if (nbEnemyShips > nbMyShips)
        k = 7;

    int nbAttackingplanet;
    if (me->nb_planet < g->nb_planet / 2)
        nbAttackingplanet = me->nb_planet;
    else
        nbAttackingplanet = (int) ((5./8) * me->nb_planet);

    for (i = 0; i < me->nb_planet && nbAttackingplanet > 0; i++) {
        src = me->planet[i];
        if (src->nb_ship < k * src->growth_rate)
            continue;
        nbAttackingplanet --;
        bestScore = 0.;
        dst = &g->planet[0];
        if (dst == src)
            dst = &g->planet[1];

        struct player *pl = &g->player[-1];
        for (int j = 0; j < pl->nb_planet; j++) {
            p = pl->planet[j];
            dis = euc_distance(p, src);
            score = 500. * p->growth_rate / (dis * dis) / (p->nb_ship+1);
            if (score > bestScore) {
                bestScore = score;
                dst = p;
            }
        }
        if (src && dst) {
            ships = dst->nb_ship + (!IS_NEUTRAL(dst->player_id)) *
                (turn_distance(dst, src) * dst->growth_rate) + 1;
            ships = ships > src->nb_ship ? src->nb_ship : ships;
            if (is_already_fleet(g, src, dst, ships))
                return;
            bot_send_order(src->id, dst->id, ships);
        }
    }
}

static struct bot bot4 = {
    .name = "bot4",
    .desc = "not so bad bot",
    .play = play
};

register_bot(bot4);
