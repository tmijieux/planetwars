#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <error.h>
#include <pthread.h>

#include "departure.h"
#include "map.h"
#include "data.h"
#include "player.h"

#define SQUARE(X)  ((X) * (X))

extern FILE *logfile;

int distance(struct planet *p1, struct planet *p2)
{
    return (int) hypot(p1->x-p2->x, p1->y-p2->y) + 1;
}

/**
 *  allocate a table for sorting fleets
 *  by remaining_turns (MAGIC)
 */
void map_initialize_fleets(struct Map *map)
{
    int max_dis = 0;
    for (int i = 0; i < map->nb_planets; ++i) {
        for (int j = i+1; j < map->nb_planets; ++j) {
            int dis = distance(&map->planets[i],
                               &map->planets[j]);
            if (dis > max_dis)
                max_dis = dis;
        }
    }
    map->size_fleets = max_dis + 1;
    map->fleets = malloc(sizeof*map->fleets * map->size_fleets);
    for (int i = 0; i < map->size_fleets; i++) {
        map->fleets[i] = list_new(LI_FREE);
    }
    map->k = 0;
}

/**
 * Read the map from file and parse planets
 */
void map_parse(struct Map *m, FILE *mapfile)
{
    char line[LINE_SIZE];
    int need_separator = 0;

    m->nb_planets = INITIAL_NB_PLANET; 
    m->planets = malloc(m->nb_planets * sizeof(struct planet));
    int i = 0;
    while ((fgets(line, LINE_SIZE, mapfile)) != NULL) {
        // Parse input
        sscanf(line, "P %f %f %d %d %d\n",  &m->planets[i].x,
               &m->planets[i].y,  &m->planets[i].owner,
               &m->planets[i].nb_ships, &m->planets[i].growth_rate);

        // Output for vizualizer
        if (need_separator)
            printf(":");
        printf("%lf,%lf,%d,%d,%d", m->planets[i].x, m->planets[i].y,
               m->planets[i].owner, m->planets[i].nb_ships,
               m->planets[i].growth_rate);
        need_separator = 1;
        i++;
        if (i == m->nb_planets) { // Resize planet tab if needed
            m->nb_planets *= 2;
            m->planets = realloc(m->planets,
                                 sizeof(struct planet)
                                 * m->nb_planets);
        }
    }
    putc('|', stdout); // delimiter: map's end in vizualizer
    fflush(stdout);
    m->nb_planets = i;
    m->planets = realloc(m->planets, sizeof(struct planet)*i);
}

/**
 * free memory reserved for the map
 */
void map_destroy(struct Map *map)
{
    for (int i = 0; i < map->size_fleets; ++i) {
        list_free(map->fleets[i]);
    }
    free(map->fleets);
    free(map->planets);
}

/**
 * Find out if the order that was parsed in fleet 'f' is a valid order or not
 */
static inline int is_valid_order(
    struct Map *map, struct fleet *f, int player_id)
{
    return (
        !(f->src >= map->nb_planets ||
          f->src < 0 ||
          f->src >= map->nb_planets ||
          f->src < 0)
        &&
        (f->total_turns > 0));
}

/**
 * 1st stage
 * fetch players' orders
 * Add new fleets and remove
 * corresponding ships
 * from source planets
 *
 * TODO : check commands
 *        measure answer time
 */
void map_departure(struct Map *map, int nb_players,
                   struct Player P[], long max_turn_time)
{
    struct thread_data *td = init_threads_data(nb_players, P);
    launch_threads(nb_players, td->threads, &td->attr, td->args);
    set_timeout(&td->time, max_turn_time);
    wait_fetching_or_timer_end(&td->time, &td->stop, &td->mutex, &td->cond);
    join_threads(nb_players, td->threads, td->latecomers);
    kick_latecomers(nb_players, P, td->latecomers);
    check_orders(td, map);
    add_fleets(td, map);
    free_threads_data(td);
}

/**
 * 2nd stage
 * decrement fleets remaining_turns (magic)
 * add growth rate to planet owned by players
 */
void map_advancement(struct Map *m)
{
    // 1: decrement fleets remaining_turns
    ++ m->k; 
    m->k %= m->size_fleets; 
    // remaining turns is computed from the difference between the index
    // od the fleets in the table and the k index, where fleets with 0
    // remaining turns are memorized
    // so this is all what we need to do !

	
    // 2: Make non-neutral grow.
    for (int i = 0; i < m->nb_planets; i++) {
        if (m->planets[i].owner != 0) {
            m->planets[i].nb_ships +=
                m->planets[i].growth_rate;
        }
    }
}

/**
 *          3rd stage
 *    fleet with 0 remaining_turns do their jobs
 *    (mostly consist of fleets killing each other)
 *    (but sometimes you can support your friends)
 *    (love is the space war)
 */
void map_arrival(struct Map *m)
{
    struct list *fleet_list = m->fleets[m->k];

    while (list_size(fleet_list) > 0) {
        struct fleet *f = list_get(fleet_list, 1);
        int nb_ships = f->nb_ships;
        int dst = f->dst;
        int owner = f->owner;
        if (owner == m->planets[dst].owner) {
            m->planets[dst].nb_ships += nb_ships;
        } else {
            m->planets[dst].nb_ships -= nb_ships;
            if (m->planets[dst].nb_ships < 0) {
                m->planets[dst].owner = owner;
                register int *nbs = &m->planets[dst]
                    .nb_ships;
                *nbs = -(*nbs);
            }
        }
        list_remove(fleet_list, 1);
    }
}

/**
 *   send map state to players
 */
void map_send(struct Map *map, int nb_players,
              struct Player P[])
{
    for (int i = 0; i < nb_players; i++) {
        P[i].is_alive = 0;
    }
		
    map_send_planets(map, nb_players, P);
    map_send_fleets(map, nb_players, P);

    for (int i = 0; i < nb_players; i++) {
        fprintf(P[i].out, "go\n");
        fflush(P[i].out);
    }
    // Log
    fprintf(logfile, "go\n");
    fflush(logfile);

    // Visualizer
    printf(":");
    fflush(stdout);
}

void map_send_planets(struct Map *map, int nb_players, struct Player P[])
{
    int need_comma = 0;

    // Print planets
    for (int i = 0; i < map->nb_planets; i++) {
        int owner = map->planets[i].owner;
        if (owner != 0)
            P[owner - 1].is_alive = 1;
        //Players
        for (int j = 0; j < nb_players; ++j) {
            map_print_sole_planet_player(&map->planets[i], &P[j],
                                         nb_players);
        }
        map_print_planet_logfile(&map->planets[i]);
        map_print_planet_vizualizer(&map->planets[i], &need_comma); 
    }
}

void map_send_fleets(struct Map *map, int nb_players, struct Player P[])
{
    // Print fleets
    for (int i = 0; i < map->size_fleets; i++) {
        int remaining_turns = i >= map->k ?
            (i - map->k) :
            i - map->k + map->size_fleets + 1;
			
        struct list *list_f = map->fleets[i];
        for (int j = 1; j < list_size(list_f); j++) {
            struct fleet *f = list_get(list_f, j);
            P[f->owner - 1].is_alive = 1;
            // Players
            for (int k = 0; k < nb_players; k++) {
                map_print_sole_fleet_player(f, &P[k],
                                            nb_players,
                                            remaining_turns);
            }
            // Log
            map_print_fleet_logfile(f, remaining_turns);
            // Visualizer
            map_print_fleet_vizualizer(f, remaining_turns);
        }
    }
}

void
map_print_fleet_logfile(struct fleet *f, int remaining_turns)
{
    fprintf(logfile,
            "F %d %d %d %d %d %d\n",
            f->owner,
            f->nb_ships,
            f->src,
            f->dst,
            f->total_turns,
            remaining_turns);
    fflush(logfile);
}

void map_print_fleet_vizualizer(struct fleet *f, int remaining_turns)
{
    printf(",%d.%d.%d.%d.%d.%d",
           f->owner,
           f->nb_ships,
           f->src,
           f->dst,
           f->total_turns,
           remaining_turns);
    fflush(stdout);
}

void map_print_sole_fleet_player(
    struct fleet *f, struct Player *P, int nb_players, int remaining_turns)
{
    fprintf(P->out,
            "F %d %d %d %d %d %d\n",
            (f->owner + P->id) %
            nb_players + 1,  // player POV
            f->nb_ships,
            f->src,
            f->dst,
            f->total_turns,
            remaining_turns);
    fflush(P->out);
}

void map_print_planet_logfile(struct planet *p)
{
    fprintf(logfile,
            "P %f %f %d %d %d\n",
            p->x,
            p->y,
            p->owner,
            p->nb_ships,
            p->growth_rate);
    fflush(logfile);
}

void map_print_planet_vizualizer(struct planet *p, int *need_comma)
{
    if (*need_comma)
        printf(",");
    printf("%d.%d",
           p->owner,
           p->nb_ships);
    *need_comma = 1;
    fflush(stdout);
}

void map_print_sole_planet_player(
    struct planet *p, struct Player *P, int nb_players)
{
    fprintf(P->out,
            "P %f %f %d %d %d\n",
            p->x,
            p->y,
            p->owner == 0 ? 0 :
            (p->owner + P->id) %
            nb_players + 1, //player POV
            p->nb_ships,
            p->growth_rate);
    fflush(P->out);
}
