#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "option.h"
#include "bot.h"
#include "uthash.h"

extern char *optarg;
extern int optind, opterr, optopt;

struct option opt[] = {
    { "bot", 1, NULL, 'b' },
    { "list", 0, NULL, 'l' },
    { "help", 0, NULL, 'h' },
    { NULL, 0, NULL, 0 },
};

void print_help(void)
{
    puts("Available option:");
    puts("-l, --list:  list available bots");
    puts("-b \"botname\", --bot  \"botname\":  select bot \"botname\"");
    puts("-h, --help: print this help");
}

void print_bot_list(void)
{
    struct bot *b, *tmp;
    if (HASH_COUNT(bots) == 0)
        printf("No bots!\n");
    HASH_ITER(hh, bots, b, tmp) {
        printf("%s: %s\n", b->name, b->desc);
    }
}

void select_bot(const char *name, struct conf *c)
{
    struct bot *b;
    HASH_FIND_STR(bots, name, b);
    if (b == NULL) {
        fprintf(stderr, "No such bot: %s\n", name);
        exit(EXIT_FAILURE);
    }
    c->bot = b;
}

void bot_parse_option(int *argc, char **argv[], struct conf *conf)
{
    char c;
    while ((c = getopt_long(*argc, *argv, "+lhb:", opt, NULL)) != -1) {
        switch (c) {
        case 'b':
            select_bot(optarg, conf);
            break;
        case 'l':
            print_bot_list();
            exit(EXIT_SUCCESS);
            break;
        case 'h':
            print_help();
            exit(EXIT_SUCCESS);
        default:
            break;
        }
    }
    *argc += optind;
    *argv += optind;
}
