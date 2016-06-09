#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>
#include <unistd.h>

#define MAX_PLAYERS 4
typedef int pid_t;
struct Player {
    unsigned int id;
    pid_t pid;
    int is_alive;

    //pipes fd
    int from[2];
    int error[2];
    int to[2];

    char **cmd;
    FILE *in;
    FILE *out;
    FILE *in_err;
};

int player_switch(int player_id);
void player_parse_command(struct Player *P, char *cmd);
void player_start_client(struct Player *P);
void player_destroy(struct Player *P);
void player_free(struct Player *P);

#endif //PLAYER_H
