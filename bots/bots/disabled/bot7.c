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
#include "list.h"

#define TRIES 400
#define HORIZON_DES_EVENEMENTS 27
// bicoz STARGATE SG-1

#include "list.c"

struct Move {
    int nbFleets;
    char targetID[100];
};

void*
add_fleet(struct PW *pw, int srcPlanetId, int unitFleetShips)
{
    struct Fleet *f = &pw->fleets[pw->nbFleets];
    f->owner = ME;
    f->src = srcPlanetId;
    f->ships = unitFleetShips;

    pw->nbFleets ++;
    return f;
}

void
do_battle(struct PW *pw, struct Fleet *f)
{
    struct Planet *p = &pw->planets[f->dst];
    if (f->owner == p->owner) { // support
	p->ships += f->ships;
	switch (p->owner) {
	case ME:
	    pw->nbMyShips += f->ships;
	    break;
	case ENEMY:
	    pw->nbEnemyShips += f->ships;
	    break;
	}
    } else { // true battle 
	p->ships -= f->ships;
	if (p->ships < 0) { // planet change owner
	    p->ships = - (p->ships);
	    if (p->owner == NEUTRAL) { // battle neutral vs player
		switch(f->owner) {
		case ME:
		    pw->nbMyShips += p->ships;
		    pw->myGrowth += p->growth;
		    break;
		case ENEMY:
		    pw->nbEnemyShips += p->ships;
		    pw->enemyGrowth += p->growth;
		    break;
		}
	    } else { // battle between 2 players
		switch(f->owner) {
		case ME:
		    pw->nbMyShips += p->ships;
		    pw->myGrowth += p->growth;
		    
		    pw->nbEnemyShips -= p->ships;
		    pw->enemyGrowth -= p->growth;
		    break;
		case ENEMY:
		    pw->nbMyShips -= p->ships;
		    pw->myGrowth -= p->growth;
		    
		    pw->nbEnemyShips += p->ships;
		    pw->enemyGrowth += p->growth;
		    break;
		}	
	    }
	    p->owner = f->owner;
	} else { // planet does not change owner
	    switch (p->owner) {
	    case ME:
		pw->nbMyShips -= f->ships;
		    break;
	    case ENEMY:
		pw->nbEnemyShips -= f->ships;
		    break;
	    }
	}
    }
}

/*
void
undo_battle(struct PW *pw, struct Fleet *f)
{
    struct Planet *p = &pw->planets[f->dst];
    if (f->owner == p->owner) {
	p->ships -= f->ships;
	switch (p->owner) {
	case ME:
	    pw->nbMyShips -= f->ships;
	    break;
	case ENEMY:
	    pw->nbEnemyShips -= f->ships;
	    break;
	}
    } else {
	p->ships -= f->ships;
	if (p->ships < 0) {
	    p->owner = f->owner;
	    p->ships = - (p->ships);
	    switch (p->owner) {
	    case ME:
		pw->myGrowth += p->growth;
		pw->enemyGrowth -= p->growth;
		pw->nbMyShips += (p->ships);
		pw->nbEnemyShips -= f->ships - p->ships;
		break;
	    case ENEMY:
		pw->myGrowth -= p->growth;
		pw->enemyGrowth += p->growth;
		pw->nbMyShips -= f->ships - p->ships;
		pw->nbEnemyShips += p->ships;
		break;
	    }		    
	} else {
	    switch (p->owner) {
	    case ME:
		pw->nbMyShips -= f->ships;
		break;
	    case ENEMY:
		pw->nbEnemyShips -= f->ships;
		break;
	    }
	}
    }
}
*/
/*
void
remove_ships_from_planets(struct PW *pw, struct List *fleets)
{
    for (int i = 1; i <= fleets->nbElement; i++) {
	struct Fleet *f = get_element_list(i, fleets);
	struct Planet *p = &pw->planets[f->src];
	p->ships -= f->ships;
    }
}

void
add_ships_to_planets(struct PW *pw, struct List *fleets)
{
    for (int i = 1; i <= fleets->nbElement; i++) {
	struct Fleet *f = get_element_list(i, fleets);
	struct Planet *p = &pw->planets[f->src];
	p->ships += f->ships;
    }
}
*/

void
do_avancement(struct PW *pw)
{
    int turnCount = HORIZON_DES_EVENEMENTS;
    while (turnCount > 0) {
	for (int i = 0; i < pw->nbPlanets; i++) {
	    struct Planet *p = &pw->planets[i];
	    if (p->owner != NEUTRAL) {
		p->ships += p->growth;
		switch(p->owner) {
		case ME:
		    pw->nbMyShips += p->growth;
		    break;
		case ENEMY:
		    pw->nbEnemyShips += p->growth;
		    break;
		}
	    }
	}
	for (int i = 0; i < pw->nbFleets; i++) {
	    int k = -- pw->fleets[i].remainingTurns;
	    if (k == 0) {
		do_battle(pw, &pw->fleets[i]);
	    }
	}
	turnCount --;
    }
}
/*
void
undo_avancement(struct PW * pw)
{
    int turnCount = HORIZON_DES_EVENEMENTS;
    while (turnCount > 0) {
	for (int i = 0; i < pw->nbPlanets; i++) {
	    	    struct Planet *p = &pw->planets[i];
	    if (p->owner != NEUTRAL) {
		p->ships -= p->growth;
		switch(p->owner) {
		case ME:
		    pw->nbMyShips -= p->growth;
		    break;
		case ENEMY:
		    pw->nbEnemyShips -= p->growth;
		    break;
		}
	    }
	}
	for (int i = 0; i < pw->nbFleets; i++) {
	    int k = ++ pw->fleets[i].remainingTurns;
	    if (k == 0) {
		undo_battle(pw, &pw->fleets[i]);
	    }
	}
	turnCount --;
    }
}
*/
void
do_move(struct Move *bestMove, struct List *fleets)
{
    for (int i = 1; i <= fleets->nbElement; i++) {
	struct Fleet *f = get_element_list(i, fleets);
	send_order(f->src, bestMove->targetID[i], f->ships);
    }
}

void
get_random_move(struct PW *pw,
		struct Move *move,
		struct List* fleets)
{
    move->nbFleets = fleets->nbElement;
    for (int i = 1; i <= fleets->nbElement; i++){
	struct Fleet *f = get_element_list(i, fleets);
	f->dst = rand() % (pw->nbPlanets);
	f->remainingTurns = i_distance(&pw->planets[f->src],
				       &pw->planets[f->dst]);    
	move->targetID[i] = f->dst;
    }
}

double
evaluate_game(struct PW *pw)
{
    return ((double)(pw->myGrowth * pw->nbMyShips) -
	    (pw->enemyGrowth * pw->nbEnemyShips));
}

struct List*
create_fleets(struct PW *pw,
	      struct Planet **planets, int nbPlanets,
	      int nbShipsToSend, int unitFleetShips)
{
    // at random
    struct List *fleets = empty_list(F_DEFAULT);
    
    while (nbShipsToSend > 10) {
	int i, c =0;
	do {
	    c++;
	    i = rand() % nbPlanets;
	} while (planets[i]->ships < unitFleetShips && c < 20);
	if (c == 20)
	    break;
	planets[i]->ships -= unitFleetShips;
	nbShipsToSend -= unitFleetShips;
	struct Fleet *f = add_fleet(pw, planets[i]->id,
				    unitFleetShips);
	add_element_list(f, fleets);
    }
    return fleets;
}

void
copy_move(struct Move *dst, struct Move *src)
{
    if (dst && src) {
	dst->nbFleets = src->nbFleets;
	for (int i = 1; i <= src->nbFleets; i++) {
	    dst->targetID[i] = src->targetID[i];
	}
    }
}

struct Move*
find_good_move(struct PW *pw, struct List *fleets)
{

    static struct PW instantGameSnapShot;
    struct Move *bestMove = malloc(sizeof(*bestMove));
    //    unsigned long long moveRepresentation;
    
    double bestScore = -9999999;
    for (int i = 0; i < TRIES; i++) {
	struct Move move;
	
	get_random_move(pw, &move, fleets);
	//set_move(pw, fleets, moveRepresentation);
	//	remove_ships_from_planets(pw, fleets);
	
	instantGameSnapShot = *pw;
	// save the game state
	
	do_avancement(pw);
	double tmp = evaluate_game(pw);

	*pw = instantGameSnapShot;
	// restore it
	
	//	undo_avancement(pw);
	//	add_ships_to_planets(pw, fleets);
	
	if (tmp > bestScore) {
	    copy_move(bestMove, &move);
	    bestScore = tmp;
	}
    }
    return bestMove;
}

/**
 * @brief This function contains BOT5's strategy !
 * @param[in] pw unique instance of the structure PW
 */
void
do_turn(struct PW *pw)
{
    if (pw->nbMyShips <= 60)
	return;
    
    int nbShipsToSend = pw->nbMyShips * (5./8);
    int unitFleetShips = nbShipsToSend / 10;
    
    struct List *fleets = create_fleets(pw,
					pw->myPlanets,
					pw->nbMyPlanets,
					nbShipsToSend,
					unitFleetShips);

    struct Move *bestMove = find_good_move(pw, fleets);
    
    do_move(bestMove, fleets);

    free(bestMove);
    destruct_list(fleets);
}
