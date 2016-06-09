#include "game.h"

/**
 * boolean that tells if there are still more than 2 players
 * alive in  the game
 */
int game_is_live(int nb_players, struct Player P[])
{
    int nb = 0;
    for (int i = 0; i < nb_players; ++i) {
	if (P[i].is_alive)
	    ++ nb;
    }
    return (nb >= 2);
}

/**
 *  return id of the winner
 *  PRECONDITION : is that game_is_alive returned false (0)
 */
int game_winner(int nb_players, struct Player P[])
{
    int i;
    for (i = 0; i < nb_players; ++i) {
	if (P[i].is_alive)
	    break;
    }
    return i+1;
}
