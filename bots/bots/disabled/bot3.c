/**
 * @file
 * @brief Contains the implementation of the strategy
 *
 * BOT3
 *
 */

#include <stdlib.h>
#include "strategy.h"
#include "strategy_means.h"
#include "data.h"
#include "print_output.h"

#define NB_PLANETS_MIN 3

/**
 * @brief This function contains the implementation of the strategy
 * @param[in] pw unique instance of the structure PW
 */
void do_turn(struct PW *pw)
{
    int expandMode = 0;
    int nbFleetsToSend = 1;
    int nbShipsToSend;
    struct Planet *src, *dst;
  
    if (pw->nbMyPlanets < NB_PLANETS_MIN) {
	expandMode = 1;
    }

    // we decide how many fleets we send
    if (expandMode)
	nbFleetsToSend = 3;
    else
	nbFleetsToSend = 5;
    
    if (pw->nbMyFleets >= nbFleetsToSend) {
	return;
    }
    
    src = my_strongest_planet(pw);

    if (expandMode) {
	dst = nearest_foreign_planet_i_may_capture(pw, src);
    } else {
	dst = weakest_planet_not_owned_by_me(pw);
    }
  
    if (src != NULL && dst != NULL) {
	if (expandMode) {
	    nbShipsToSend = src->ships / 2;
	    if (pw->turnCount % 2 == 0)
		send_order(src->id, dst->id, nbShipsToSend);
	} else {
	    nbShipsToSend = src->ships * 70 / 100;
	    if (pw->turnCount % 5 == 0)
	        send_order(src->id, dst->id, nbShipsToSend);
	}
    }
}
