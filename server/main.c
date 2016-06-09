#include <stdio.h>
#include <stdlib.h>

#include "data.h"
#include "player.h"
#include "map.h"
#include "game.h"

FILE *logfile;

/*
  stdout is for the game 'play back' for the visualizer
  stderr is for printing every other messages.
  
  TODO
  -verify is an order is correct
  -find out who is the winner
*/

static inline void usage(const char *s)
{
    fprintf(stderr, "usage:\n"
            "%s map max_turn_time "
            "max_nb_turns log_filename "
            "player_one player_two [more players (max 4)]\n", s);
}

int main(int argc, char *argv[])
{
    long max_turn_time;
    int max_nb_turns;
    int turn_count = 0;

    FILE *mapfile;

    // struct GAME g;
    struct Map map;
    int nb_players;
    struct Player P[MAX_PLAYERS];

    // PARSE ARGS
    if (argc < 7) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((logfile = fopen(argv[4], "w+")) == NULL) {
        fprintf(stderr, "error opening logfile %s\n",
                argv[4]);
        exit(EXIT_FAILURE);
    }
    if ((mapfile = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "error opening mapfile %s\n",
                argv[1]);
        exit(EXIT_FAILURE);
    }

    max_turn_time = atol(argv[2]);
    max_nb_turns = atoi(argv[3]);
    nb_players = argc - 5;
    if (nb_players > 4) {
        fprintf(stderr, "No more than %d players !\n",
                MAX_PLAYERS);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < nb_players; i++) {
        P[i].id = i+1;
        player_parse_command(&P[i], argv[5+i]);
    }

    // PARSE map
    map_parse(&map, mapfile);
    fclose(mapfile);
	
    // INITIALIZE TABLE of LIST FLEET
    map_initialize_fleets(&map);

    // LAUNCH PLAYERS in FORK CHILDs
    for (int i = 0; i < nb_players; i++) {
        player_start_client(&P[i]);
    }

    // MAIN GAME LOOP
    int is_live;
    while ((is_live = game_is_live(nb_players, P)) &&
           turn_count < max_nb_turns) {

        map_send(&map, nb_players, P);
        // we take advantage of the fact that we need
        // to go through the whole map in this function
        // to check if the game is finished
	
        map_departure(&map, nb_players, P, max_turn_time);
        // this 'fetches' orders

        map_advancement(&map);
        map_arrival(&map);

        ++ turn_count;
        fprintf(stderr, "Turn %d\n", turn_count);
        /*
          print_player_err(nb_players);
        */
    }
    printf("\n");
    if (is_live) {
        fprintf(stderr, "Draw !\n");
    } else {
        fprintf(stderr, "Player %d won!\n",
                game_winner(nb_players, P));
    }

    map_destroy(&map);
    for (int i = 0; i < nb_players; i++) {
        player_destroy(&P[i]);
    }
    return EXIT_SUCCESS;
}

/*
  void
  print_player_err(int nb_players)
  {
  for(i = 0; i < nb_players; i++)
  {
  if (c = fgetc(p[i].in_err != EOF))
  {
  fprintf(stderr, "Player %d:");
  ungetc(c, P[i].in_err);
  }
  while ((c = fgetc(P[i].in_err)) != EOF)
  fputc(c, stderr);
  }
  }
*/
