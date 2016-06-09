#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

#include "bot.h"

struct bot *bots = NULL;

/**
 * @brief Send a single order to the server.
 * @param[in] source id of the source planet
 * @param[in] destination id of the destination planet
 * @param[in] nb_ship amount of ships you want to send
 *
 */
void bot_send_order(int source, int destination, int nb_ship)
{
    if (nb_ship > 0 && source != destination) {
        printf("%d %d %d\n", source, destination, nb_ship);
        fprintf(stderr, "%d %d %d\n", source, destination, nb_ship);
        fflush(stdout);
    }
}

/**
 * @brief Tells if a particular fleet already exists
 * @param[in] g the single main game structure
 * @param[in] src the source planet
 * @param[in] dst the destination planet
 * @param[in] nb_ship number of ships
 * @return boolean telling if a fleet exists
 */
int is_already_fleet(struct game *g, struct planet *src,
                     struct planet *dst, int nb_ship)
{
    for (int i = 0; i < g->nb_fleet; i++) {
	if (g->fleet[i].p_src == src->id &&
	    g->fleet[i].p_dst == dst->id &&
	    g->fleet[i].nb_ship == nb_ship)
	    return 1;
    }
    return 0;
}

/**
 * @brief Computes and returns the euclidean distance between two planets
 * @param[in] p1 first planet
 * @param[in] p2 second planet
 * @return distance between two planets
 */
float euc_distance(struct planet *p1, struct planet *p2)
{
    return sqrt(SQUARE(p1->x - p2->x) + SQUARE(p1->y - p2->y));
}

int turn_distance(struct planet *src, struct planet *dst)
{
    return ((int) ceilf(euc_distance(src, dst)));
}

/**
 * @brief Returns a pointer on a random planet owned by the given player
 * @param[in] g the sole main game structure
 * @param[in] playerId an integer designating the player
 * @return a pointer on the planet which have been choosen randomly
 */ 
struct planet *random_planet_owned_by(struct game *g, struct player *p)
{
    int r, k;
    struct planet **tab;
	
    tab = p->planet;
    k = p->nb_planet;

    if (k > 0) {
	r = rand() % k;
	return tab[r];
    }
    return NULL;
}

/**
 * @brief Returns a pointer on a random enemy or neutral planet
 * @param[in] g the sole main game structure
 * @return a pointer on the planet which have been choosen randomly
 */ 
struct planet *random_foreign_planet(struct game *g)
{
    struct player *p = &g->player[-1];
    if (p->nb_planet != 0) {
	int r = rand() % p->nb_planet;
	return p->planet[r];
    }
    return NULL;
}

/**
 * @brief Returns a pointer on the strongest planet owned by the given player
 * @param[in] g unique instance of the structure game
 * @param[in] playerId identification of the player
 * @return a pointer on the strongest planet owned by the given player
 */
struct planet *strongest_planet_of(struct game *g, struct player *p)
{
    int i, k;
    double score, dstScore = 0.;
    struct planet **tab, *dst = NULL;

    tab = p->planet;
    k = p->nb_planet;
	
    for (i = 0; i < k; ++i) {
	score = (double) tab[i]->nb_ship / (1 + tab[i]->growth_rate);
	if (score > dstScore) {
	    dstScore = score;
	    dst = tab[i];
	}
    }
    return dst;
}

/**
 * @brief Returns a pointer on the weakest planet owned by the given player
 * @param[in] g unique instance of the structure game
 * @param[in] playerId identification of the player
 * @return a pointer on the weakest planet owned by the given player
 */
struct planet *weakest_planet_of(struct game *g, struct player *p)
{
    int i, k;
    double score, dstScore = 0.;
    struct planet **tab;
    struct planet *dst = NULL;

    tab = p->planet;
    k = p->nb_planet;
	
    for (i = 0; i < k; ++i) {
	score = (double)  (1 + tab[i]->growth_rate) / tab[i]->nb_ship;
	if (score < dstScore) {
	    dstScore = score;
	    dst = tab[i];
	}
    }
    return dst;
}

/**
 * @brief Computes and returns the production power of a player
 * @param[in] planet array of pointers on planets
 * @param[in] nbplanets number of planets which are in the array
 *
 * The production power of a player is the sum of the
 * growth_rate rates of all the planet he owns.
 */
int production(struct planet *planets[], int nbplanets)
{
    int i;
    int prod = 0;
    for (i = 0; i < nbplanets; ++i) {
	prod += planets[i]->growth_rate;
    }
    return prod;
}

/**
 * @brief Returns a pointer on the weakest planet not owned by me
 * @param[in] g unique instance of structure game 
 * @return a pointer on the weakest planet not owned by me
 */
struct planet *weakest_alien_planet(struct game *g)
{  
    double score, dstScore = 0.;
    struct planet *dst = NULL;
    struct player *p = &g->player[-1];
	
    for (int i = 0; i < p->nb_planet; ++i) {
        struct planet *q = p->planet[i];
	score = (double) (1 + q->growth_rate) / q->nb_ship;
	if (score > dstScore) {
	    dstScore = score;
	    dst = q;
	}
    }
    return dst;
}
