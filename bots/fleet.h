#ifndef FLEET_H
#define FLEET_H

/**
 * @struct fleet
 * @brief fleet structure
 */
struct fleet {
    int id;
    int player_id;
    int nb_ship; 
    int p_src; // source planet
    int p_dst; // destination planet
    int remaining_turns;
    int total_length;
};

void read_fleet(struct fleet *f, const char *line);
    
#endif //FLEET_H
