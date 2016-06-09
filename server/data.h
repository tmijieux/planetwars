#ifndef DATA_H
#define DATA_H

struct planet {
    float x;
    float y;
    int owner;
    int nb_ships;
    int growth_rate;
};

struct fleet {
    int owner;
    int src;
    int dst;
    int nb_ships;
    int total_turns;
    int remaining_turns;
};

#endif //DATA_H
