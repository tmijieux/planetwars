#ifndef DEPARTURE_H
#define DEPARTURE_H


#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <pthread.h>

#include "player.h"
#include "map.h"
#include "../util/nqueue.h"

struct ARGS {
    pthread_mutex_t *mutex;
    pthread_cond_t *cond;
    int *stop;
    int *count;
    int nb_players;
    struct Player *P;
    struct queue *order_queue;
};

struct thread_data {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    struct queue *order_queue;
    struct timespec time;
    int count;
    int stop;
	
    pthread_attr_t attr;
    pthread_t *threads;
    int *latecomers;
    struct ARGS *args;
};

void* fetch_player_orders(void *args);
void wait_fetching_or_timer_end(struct timespec *time, int *stop,
				pthread_mutex_t *mutex, pthread_cond_t *cond);
void launch_threads(int nb_players, pthread_t threads[],
		    pthread_attr_t *attr, struct ARGS *args);
struct thread_data* init_threads_data(int nb_players, struct Player P[]);
void free_threads_data(struct thread_data *td);
void set_timeout(struct timespec *time, long max_turn_time);
void join_threads(int nb_players, pthread_t *threads, int latecomers[]);
void kick_latecomers(int nb_player, struct Player P[], int latecomers[]);
void check_orders(struct thread_data *td, struct Map *map);
void add_fleets(struct thread_data *td, struct Map *map);
void free_threads_data(struct thread_data *td);

#endif //DEPARTURE_H
