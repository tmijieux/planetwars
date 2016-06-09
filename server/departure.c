#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "departure.h"
#include "player.h"

/**
 * fetch a players order
 * and creates corresponding fleets
 * This function is a possible exit point if orders from players are incorrects
 */
void *fetch_player_orders(void *args)
{
    char line[LINE_SIZE];
    struct ARGS *arg = args;
	
    fgets(line, LINE_SIZE, arg->P->in);
    while (strcmp(line, "go\n") != 0) {
        struct fleet *f = malloc(sizeof(*f));
        fgets(line, LINE_SIZE, arg->P->in);
        sscanf(line, "%d %d %d", &f->src, &f->dst, &f->nb_ships);
        pthread_mutex_lock(arg->mutex);
        if (*(arg->stop)) {
            pthread_mutex_unlock(arg->mutex);
            free(f);
            pthread_exit((void*) 1);
        }
        nqueue_push(arg->order_queue, f);
        pthread_mutex_unlock(arg->mutex);
    }

    pthread_mutex_lock(arg->mutex);
    (*(arg->count))++;
    if (*(arg->count) >= arg->nb_players)
        pthread_cond_signal(arg->cond);
    pthread_mutex_unlock(arg->mutex);
    pthread_exit((void*) 0);
}

void wait_fetching_or_timer_end(
    struct timespec *time, int *stop,
    pthread_mutex_t *mutex, pthread_cond_t *cond)
{
    pthread_mutex_lock(mutex);
    int rc = pthread_cond_timedwait(cond, mutex, time);
    if (rc == ETIMEDOUT)
        *stop = 1;
    pthread_mutex_unlock(mutex);	
}

void launch_threads(int nb_players, pthread_t threads[],
                    pthread_attr_t *attr, struct ARGS *args)
{
    for (int i = 0; i < nb_players; ++i) {
        pthread_create(&threads[i], attr, fetch_player_orders, &args[i]);
    }
}

struct thread_data* init_threads_data(int nb_players, struct Player P[])
{
    struct thread_data *td = malloc(sizeof(*td));
    td->order_queue = nqueue_create();

    td->args = malloc(sizeof(*td->args) * nb_players);
    td->threads = malloc(sizeof(*td->threads) * nb_players);
    td->latecomers = malloc(sizeof(*td->latecomers) * nb_players);
    pthread_attr_init(&td->attr);
    for (int i = 0; i < nb_players; i++) {
        td->args[i].P = &P[i];
        td->args[i].stop = &td->stop;
        td->args[i].count = &td->count;
        td->args[i].nb_players = nb_players;
        td->args[i].mutex = &td->mutex;
        td->args[i].cond = &td->cond;
        td->args[i].order_queue = td->order_queue;
    }
    return td;
}

void free_threads_data(struct thread_data *td)
{
    nqueue_destroy(td->order_queue);
    free(td->args);
    free(td->threads);
    free(td->latecomers);
    free(td);
}

void set_timeout(struct timespec *time, long max_turn_time)
{
    time->tv_sec = max_turn_time / 1000;
    time->tv_nsec = 1000000 * (max_turn_time % 1000);
}

void join_threads(int nb_players, pthread_t *threads, int latecomers[])
{
    for (int i = 0; i < nb_players; i++) {
        void *res;
        pthread_join(threads[i], &res);
        latecomers[i] = (int)(long) res;
        //  ou comment éviter les warnings en beauté
    }
}

void kick_latecomers(int nb_player, struct Player P[], int latecomers[])
{
    for (int i = 0; i < nb_player; i++) {
        if (latecomers[i]) {
            player_destroy(&P[i]);
            fprintf(stderr, "Player %u timed out", P[i].id);
        }
    }
}

void check_orders(struct thread_data *td, struct Map *map)
{
    
}

void add_fleets(struct thread_data *td, struct Map *map)
{
    
}
