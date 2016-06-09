#ifndef OPTION_H
#define OPTION_H

struct conf;

#include "bot.h"

struct conf {
    struct bot *bot;
};

void bot_parse_option(int *argc, char **argv[], struct conf *conf);
void print_bot_list(void);
void print_help(void);

#endif //OPTION_H
