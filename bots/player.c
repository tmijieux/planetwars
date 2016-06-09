#include "player.h"

int player_total_nb_ship(struct player *p)
{
    int nb_ship = 0;

    for (int i = 0; i < p->nb_planet; i++)
        nb_ship += p->planet[i]->nb_ship;
    for (int i = 0; i < p->nb_fleet; i++)
	nb_ship += p->fleet[i]->nb_ship;
    return nb_ship;
}
