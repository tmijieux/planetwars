/**
 * @file
 * @brief BOT5 for the lose !
 *
 * BOT5
 *
 */

#include <stdlib.h>
#include <math.h>
#include "strategy.h"
#include "strategy_means.h"
#include "data.h"
#include "print_output.h"

void
compute_GFG_SL(struct PW *pw,
	       double growthFastGain[],
	       int shipsLoss[],
	       struct Planet *planets[],
	       int nbPlanets,
	       struct Planet *baryPlanet);

int best_growth_gain(struct Planet *planets[],
		     double growthFastGain[],
		     int shipsLoss[],
		     int nbPlanets,
		     int maxShipsLoss,
		     int *nbTargetPlanet,
		     struct Planet **targetPlanets);

void baryships_planet(struct Planet *baryPlanet,
		 struct Planet *planets[],
		 int nbPlanets);

int nb_ships_planets(struct Planet *planets[],
		     int nbPlanets)
{
    int i, s = 0;
    for (i = 0; i < nbPlanets; i++) {
	s += planets[i]->ships;
    }
    return s;
}

 
/**
 * @brief This function contains BOT5's strategy !
 * @param[in] pw unique instance of the structure PW
 */
void do_turn(struct PW *pw)
{
    int i, j, nbShips, extraGrowth, a, b;
    int nbMyShips, /*nbEnemyShips,*/ *shipsLoss = NULL, maxShipsLoss;
    int/* myTotalGrowth, enemyTotalGrowth,*/ nbTargetPlanets;
    double *growthFastGain = NULL; /*randRatio,*/
    struct Planet baryPlanet = {.x = 0, .y = 0.}, **targetPlanets = NULL;
    struct Planet *src = NULL, *dst = NULL, *p;
    struct FlyingShips fs;

    if (pw->nbMyPlanets <= 0)
	return;

    if (pw->turnCount % 5 == 0) {
	// computing time :| 
	growthFastGain = calloc(sizeof(*growthFastGain),
				pw->nbNotMyPlanets);
	shipsLoss = calloc(sizeof(*shipsLoss), pw->nbNotMyPlanets);
	targetPlanets = calloc(pw->nbNotMyPlanets, sizeof(*targetPlanets));
	
	baryships_planet(&baryPlanet, pw->myPlanets, pw->nbMyPlanets);
	compute_GFG_SL(pw,
		       growthFastGain,
		       shipsLoss,
		       pw->notMyPlanets,
		       pw->nbNotMyPlanets,
		       &baryPlanet);
	//randRatio = (((double) rand()/RAND_MAX) * (1./2.)) + (1./3.);
	nbMyShips = nb_ships_planets(pw->myPlanets, pw->nbMyPlanets);
	maxShipsLoss = (int) ((3./4) * nbMyShips);
	best_growth_gain(pw->notMyPlanets,
			 growthFastGain,
			 shipsLoss,
			 pw->nbNotMyPlanets,
			 maxShipsLoss,
			 &nbTargetPlanets,
			 targetPlanets);
    
	// targetPlanets = realloc(sizeof(*targetPlanets)
	//			    * (nbTargetPlanets+1));

    	// attack time !!! :)
   
	for (i = 0; i < nbTargetPlanets; i++){
	    dst = targetPlanets[i];
	    while (dst->ships >= 0) {
		// Find my strongest planet close to targetPlanet[i]
		double srcScore = 0.;
		for (j = 0; j < pw->nbMyPlanets; j++) {
		    p = pw->myPlanets[j];
		    double dis = distance(p, dst);
		    double score = 5000. * p->ships * p->growth
			/ (dis + 1);
		    if (score > srcScore) {
			srcScore = score;
			src = p;
		    }
		}
		//src = my_strongest_planet(pw);
		// ATTACK !
		double dis = distance(src, dst);
		extraGrowth = 0;
		if (dst->owner != NEUTRAL)
		    extraGrowth = (int) (ceilf(dis) * dst->growth);
		flying_ships_to_planet(pw, dst, 2 * (int) dis, &fs);
		a =  dst->ships + extraGrowth + 1 + fs.enemy;
		b =  (int)((5./6) * src->ships);
		nbShips = a < b ? a : b;
		send_order(src->id, dst->id, nbShips);

		src->ships -= nbShips;
		dst->ships -= nbShips;
	    }
	}
	free(growthFastGain);
	free(shipsLoss);
	free(targetPlanets);
    }
    /*
    else if (pw->turnCount % 2 != 0) {
	nbMyShips = total_nb_ships(pw, ME);
	nbEnemyShips = total_nb_ships(pw, ENEMY);;
	myTotalGrowth = production(pw->my_planets,
				   pw->nb_my_planets);
	enemyTotalGrowth = production(pw->enemy_planets,
				      pw->nb_enemy_planets);


	for (i = 0; i < pw->nbMyPlanets; i++) {
	    src = pw->myPlanets[i];
	    if (src->ships < 10 * src->growth)
		continue;
		
	    double dstScore = 0.;
	    dst = &pw->planets[0];
		
	    for (j = 0; j < pw->nbNotMyPlanets; j++) {
		p = pw->notMyPlanets[j];
		double dis = distance(p, src);
		double score = 500. * p->growth
		    / (dis * dis)
		    / (p->ships+1);
		if (score > dstScore) {
		    dstScore = score;
		    dst = p;
		}
	    }
	    send_order(src->id, dst->id, (int) 5.*src->ships / 8);
	    }
	}
    	*/
}

int best_growth_gain(struct Planet *planets[],
		     double growthFastGain[], int shipsLoss[],
		     int nbPlanets, int maxShipsLoss,
		     int *nbTargetPlanets, struct Planet **targetPlanets)
{
    /* an heuristic for the KnapSack problem
     * where values are growthFastGain[] elements
     * and weights are shipsLoss[] elements
     */
    int i, j, K, r, *keep = NULL;
    double *GrGa = NULL;
    GrGa = calloc(sizeof(double), (nbPlanets+1) * (maxShipsLoss+1));
#define GG(x__, y__) (GrGa[((x__)*(maxShipsLoss+1))+(y__)])
    keep = calloc(sizeof(int), (nbPlanets+1) * (maxShipsLoss+1));
#define KE(x__, y__) (keep[((x__)*(maxShipsLoss+1))+(y__)])

    for (i = 1; i <= nbPlanets; i++) {
	for (j = 0; j <= maxShipsLoss; j++) {
	    if ((shipsLoss[i-1] <= j) &&
		(growthFastGain[i-1] + GG(i-1, j-shipsLoss[i-1]) > GG(i-1, j))
	    ) {
		GG(i, j) = growthFastGain[i-1] + GG(i-1, j-shipsLoss[i-1]);
		KE(i, j) = 1;
	    } else {
		GG(i, j) = GG(i-1, j);
		KE(i, j) = 0;
	    }
	}
    }
    K = maxShipsLoss;
    *nbTargetPlanets = 0;
    for (i = nbPlanets; i >= 1; i--) {
	if (KE(i, K) == 1) {
	    targetPlanets[*nbTargetPlanets] = planets[i-1];
	    ++ (*nbTargetPlanets);
	    K -= shipsLoss[i-1];
	}
    }
    r = GG(nbPlanets, maxShipsLoss);
#undef GG
#undef KE
    free(keep);
    free(GrGa);
    return r;
}

void
compute_GFG_SL(struct PW *pw,
	       double growthFastGain[],
	       int shipsLoss[],
	       struct Planet *planets[],
	       int nbPlanets,
	       struct Planet *baryPlanet)
{
    int i, extraGrowth;
    float dis;
    struct Planet *p;
    struct FlyingShips fs;
    
    for(i = 0; i < nbPlanets; i++) {
	p = planets[i];
	dis = distance(baryPlanet, p);
	extraGrowth = 0;
	flying_ships_to_planet(pw, p, 5 *(int) dis, &fs);
	if (p->owner != NEUTRAL)
	    extraGrowth = (int) ceilf(dis) * p->growth;
	shipsLoss[i] = planets[i]->ships + extraGrowth + fs.enemy + 14;
	growthFastGain[i] = 500. * p->growth
	    / (dis * dis )
	    / (p->ships+1);
	if (fs.ally > planets[i]->ships + fs.enemy + extraGrowth)
	    growthFastGain[i] = 0.;
    }
}

void
baryships_planet(struct Planet *baryPlanet,
		 struct Planet *planets[],
		 int nbPlanets)
{
    int i, total = 0;

    for (i = 0; i < nbPlanets; i++) {
	total += planets[i]->ships;
	baryPlanet->x += planets[i]->ships * planets[i]->x;
	baryPlanet->y += planets[i]->ships * planets[i]->y;
    }
    if (total != 0){
	baryPlanet->x /= total;
	baryPlanet->y /= total;
    }
}

