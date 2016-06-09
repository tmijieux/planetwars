#define _POSIX_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

#include "player.h"

/**
 *  start a player's strategy bot
 */
void player_start_client(struct Player *P)
{
    pipe(P->from);
    pipe(P->to);
    pipe(P->error);
	
    P->pid = fork();
    if (P->pid == -1){
        fprintf(stderr, "Error while forking player %d\n",
                P->id);
        player_free(P);
        exit(EXIT_FAILURE);
    }
		
    if (P->pid == 0) {
        dup2(P->to[0], STDIN_FILENO);
        dup2(P->from[1], STDOUT_FILENO);
        dup2(P->error[1], STDERR_FILENO);
		
        close(P->from[0]);		
        close(P->error[0]);
        close(P->to[1]);

        execvp(P->cmd[0], P->cmd);
		
    } else {
        P->in = fdopen(P->from[0], "r");
        P->in_err = fdopen(P->error[0], "r");
        P->out = fdopen(P->to[1], "w");

        close(P->from[1]);		
        close(P->error[1]);
        close(P->to[0]);
    }
}

void player_free(struct Player *P)
{
    free(P->cmd);
    fclose(P->in);
    fclose(P->out);
}

void player_destroy(struct Player *P)
{
    kill(P->pid, SIGKILL);
    player_free(P);
}

/**
 *  Tokenize the command (of a bot)
 *  so that it could be launched by execvp
 */
void player_parse_command(struct Player *P, char *cmd)
{
    int i = 1;
    int size = 5;
    char *tmp;

    P->cmd = calloc(size, sizeof(char*));
    P->cmd[0] = strtok(cmd, " ");
    while ((tmp = strtok(NULL, " ")) != NULL) {
        if (i == size) {
            size <<= 1; // *2
            P->cmd =realloc(P->cmd,
                            sizeof(char*)*size);
        }
        P->cmd[i] = tmp;
        i++;
    }
    i++;
    P->cmd = realloc(P->cmd, sizeof(char*)*i);
    P->cmd[i-1] = NULL;
}
