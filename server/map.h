#ifndef MAP_H
#define MAP_H

#include "data.h"
#include "player.h"
#include "../util/list.h"

#define INITIAL_NB_PLANET 50
#define LINE_SIZE 80

struct Map {
    struct planet *planets;
	
    // DYNAMIC size: max(distance(p1, p2)) 
    // for (p1,p2) in {planets}
    // index is 'turns remaining - k'
    struct list **fleets; // callocd
	
    int nb_planets;
    int size_fleets;
    int k;	// index with 0 remaing_turns fleets
};

void map_parse(struct Map *m, FILE *mapfile);
void map_initialize_fleets(struct Map *map);
void map_destroy(struct Map *map);
void map_departure(
    struct Map *map, int nb_players, struct Player P[], long max_turn_time);
void map_advancement(struct Map *map);
void map_arrival(struct Map *map);
void map_send(struct Map *map, int nb_players, struct Player P[]);
void map_send_fleets(struct Map *map, int nb_players, struct Player P[]);
void map_send_planets(struct Map *map, int nb_players, struct Player P[]);
void map_print_fleet_logfile(struct fleet *f, int remaining_turns);
void map_print_fleet_vizualizer(struct fleet *f, int remaining_turns);
void map_print_sole_fleet_player(
    struct fleet *f, struct Player *P, int nb_players, int remaining_turns);
void map_print_planet_logfile(struct planet *p);
void map_print_planet_vizualizer(struct planet *p, int *need_comma);
void map_print_sole_planet_player(
    struct planet *p, struct Player *P, int nb_players);

#endif //MAP_H
