#include <stdio.h>
#include <stdlib.h>

#include "fleet.h"

void read_fleet(struct fleet *f, const char *line)
{
    sscanf(line, "F %d %d %d %d %d %d",
           &f->player_id, &f->nb_ship,  &f->p_src,
           &f->p_dst, &f->total_length, &f->remaining_turns);
}
