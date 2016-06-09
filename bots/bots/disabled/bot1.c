/**
 * @file
 * @brief Contains the implementation of the strategy
 *
 * BOT1 is a random bot
 * he may attack with a lot of planet if he took the
 * advantage
 *
 */

#include <stdlib.h>
#include "strategy.h"
#include "strategy_means.h"
#include "data.h"
#include "print_output.h"

#define ATTACK_TURN_RATE 2

/**
 * @brief This function contains the implementation of the strategy
 * @param[in] pw unique instance of the structure PW
 */
void do_turn(struct PW *pw)
{
    int i;
    int ships;
    struct Planet *src, *dst;
    static int nbAttackingPlanet = 1;
    
    if (pw->turnCount % ATTACK_TURN_RATE == 0) {
	for (i = 0; i < nbAttackingPlanet; i++) {
	    src = random_planet_owned_by(pw, ME);
	    dst = random_foreign_planet(pw);
	    if (src != NULL && dst != NULL) {
		ships = rand() % (src->ships+1);
		src->ships -= ships;
		send_order(src->id, dst->id, ships);
	    }
	    if (pw->nbMyPlanets > nbAttackingPlanet + 4)
	        nbAttackingPlanet += 3;
	}
    }
}

