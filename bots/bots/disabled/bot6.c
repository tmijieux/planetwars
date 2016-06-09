#include <stdlib.h>
#include <math.h>
#include "strategy.h"
#include "strategy_means.h"
#include "data.h"
#include "print_output.h"


void do_turn(struct PW* pw)
{
    int i=0;
    int turnsToGet=9999;
    int nbShipsInPlanet;
    //int growthMax = 0;
    struct Planet *p = my_strongest_planet(pw);
    struct Planet *q = enemy_strongest_planet(pw);
    //int planet = random_foreign_planet(pw)->id ;
    int planet = 1;
    //int nb_ships_to_send = 0;
    // case where our bot is superior than the enemy's
    if (total_nb_ships(pw,ME) > (total_nb_ships(pw,ENEMY)+20) &&
	pw->planets[p->id].ships > (pw->planets[q->id].ships + 20) &&
	q != NULL){
	send_order(p->id, q->id, pw->planets[q->id].ships+5);
	return ;
    }
    // defense case
      
    for (i=0; i < pw->nbEnemyFleets;i++) {
	if (pw->planets[pw->enemyFleets[i]->dst].id == p->id) {
	    send_order(p->id, weakest_planet_not_owned_by_me(pw)->id, p->ships);
	    return ;
	    //>>>>>>> .r169
	}
	//<<<<<<< .mine
    }

    // trying to sabotage his moves
    for (i=0; i<pw->nbEnemyFleets; i++) {
	nbShipsInPlanet = pw->enemyFleets[i]->ships -
	    pw->planets[pw->enemyFleets[i]->dst].ships ;
	if (nbShipsInPlanet > 0 &&
	    (i_distance(p, &(pw->planets[pw->enemyFleets[i]->dst])) >
	     pw->enemyFleets[i]->remainingTurns )) {
	    if (i_distance(p, &(pw->planets[pw->enemyFleets[i]->dst])) <
		turnsToGet) {
		turnsToGet = i_distance(p, &(pw->planets
					     [pw->enemyFleets[i]->dst]));
		planet = pw->enemyFleets[i]->dst;
		//>>>>>>> .r169
	    }

	}
	if (nbShipsInPlanet>0 &&
	    i_distance(p, &(pw->planets
			    [pw->enemyFleets[i]->dst])) == turnsToGet) {
	    turnsToGet = i_distance(p, &(pw->planets
					 [pw->enemyFleets[i]->dst]));
	    planet = (pw->planets[planet].growth >
		      (pw->planets[pw->enemyFleets[i]->dst]).growth) ?
		planet : pw->enemyFleets[i]->dst;
	}

    }
    if (p->ships > nbShipsInPlanet+1 && nbShipsInPlanet > 0 &&
	!(p->id == pw->planets[planet].id)){
	send_order(p->id, pw->planets[planet].id, nbShipsInPlanet+1);
	return;
    }

}

