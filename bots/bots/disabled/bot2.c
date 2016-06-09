/**
 * @file
 * @brief Contains the implementation of the strategy
 *
 * BOT2
 *
 */

#include "strategy.h"
#include "strategy_means.h"
#include "data.h"
#include "print_output.h"

/**
 * @brief This function contains the implementation of the strategy
 * @param[in] pw unique instance of the structure PW
 */
void do_turn(struct PW *pw)
{
    struct Planet *src, *dst;
    int ships;

    if (pw->turnCount % 10 == 0) {
	src = random_planet_owned_by(pw, ME);
	dst = random_foreign_planet(pw);
	if (src && dst) {
	    ships = src->ships / 2;
	    send_order(src->id, dst->id, ships);
	}
    }
}
