#include <stdio.h>
#include <string.h>

#include "game.h"
#include "bot.h"

#define BUFSIZE 1024

struct player player_buf[21];

/**
 * @brief Read a line from server
 * @param[out] line the return value
 * @param[in] line_size the max size the bot is allowed to read
 *
 * read a line from standart input
 * and store it in 'line'
 */
static void read_line(char *line, int length)
{
    int c, i = 0;
    do {
	c = getchar();
	line[i++] = (char) c;
    } while (c != '#' && i < length && c != '\n' && c != EOF);

    // make sure to read the entire line (with possible comments)
    while (c != '\n' && c != EOF) {
	c = getchar();
    }

    if (line[i-1] == '#' || line[i-1] == '\n') {
	line[i-1] = '\0';
    } else {
	line[length - 1] = '\0';
    }
}

void game_init(struct game *g)
{
    memset(g, 0, sizeof*g);
    memset(player_buf, 0, sizeof player_buf);
    // maybe set player_id to -1 (!= 0 because 0 is neutral)
    
    g->player = &player_buf[1];
}

void game_turn_reinit(struct game *g)
{
    int tmp = g->turn_count;
    game_init(g);
    g->turn_count = tmp+1;
}

/**
 * @brief Send the terminating sequence to the server.
 */
void game_turn_end(void)
{
    printf("go\n");
    fflush(stdout);
}

void game_read(struct game *g, char *line)
{
    struct player *p;
    struct planet *q;
    struct fleet *f;
    
    if (line[0] == 'P') {
        q = &g->planet[g->nb_planet++];
        q->id = g->nb_planet-1;
        if (q->player_id+1 > g->nb_player)
            g->nb_player = q->player_id+1;
        read_planet(q, line);
        
        p = &g->player[q->player_id];
        p->planet[p->nb_planet++] = q;
        if (IS_ALIEN(q->player_id)) {
            p = &g->player[-1];
            p->planet[p->nb_planet++] = q;
        }
    } else if (line[0] == 'F') {
        f = &g->fleet[g->nb_fleet++];
        f->id = g->nb_fleet-1;
        read_fleet(f, line);
        
        p = &g->player[f->player_id];
        p->fleet[p->nb_fleet++] = f;

        if (IS_ALIEN(f->player_id)) {
            p = &g->player[-1];
            p->fleet[p->nb_fleet++] = f;
        }
    } else {
        fprintf(stderr, "discarding invalid line '%s'\n", line);
    }
}

void game_run(struct game *g, struct bot *b)
{
    char line[BUFSIZE];
    while (1) {
	read_line(line, BUFSIZE);
	if (strcmp(line, "go") != 0) {
	    game_read(g, line);
	} else {
	    b->play(b, g);
            game_turn_end();
	    game_turn_reinit(g);
	}
    }
}
