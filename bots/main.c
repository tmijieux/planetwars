#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"
#include "bot.h"
#include "option.h"

#define LINE_SIZE 80

int main(int argc, char *argv[])
{
    struct game g;
    struct conf c = { NULL };

    bot_parse_option(&argc, &argv, &c);
    if (c.bot == NULL) {
        print_help();
        puts("\n---\nBOT LIST:");
        print_bot_list();
        puts ("");
        fprintf(stderr, "You must choose a bot with the '-b' option!\n");
        exit(EXIT_FAILURE);
    }
    srand(time(NULL) + (unsigned long)&argc);
    game_init(&g);
    game_run(&g, c.bot);

    return EXIT_FAILURE;
}
