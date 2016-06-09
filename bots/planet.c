#include <stdio.h>
#include <stdlib.h>

#include "planet.h"

void read_planet(struct planet *p, const char *line)
{
    sscanf(line, "P %f %f %d %d %d",
           &p->x, &p->y, &p->player_id, &p->nb_ship, &p->growth_rate);
}
