/**
 * @file
 * @brief Contains the implementation of the strategy
 *
 * BOT0 just do nothing,
 * not losing against rage bot though
 *
 */

#include "../bot.h"
#define I_AM_A_LAZY_BOT

static void play(struct bot *b, struct game *g)
{
    
}

static struct bot bot0 = {
    .name = "bot0",
    .desc = "a passive bot",
    .play = play
};

register_bot(bot0);
