#ifndef PLANET_H
#define PLANET_H

/**
 * @struct Planet
 * @brief Planet structure
 */
struct planet {
    int id;   /*!< id of the planet */
    float x;  /*!< abscissa of the planet */
    float y;  /*!< ordinate of the planet */
    int player_id;
    int nb_ship;     /*!< number of ships on the planet */
    int growth_rate; /*!< growth rate of the planet */
};

void read_planet(struct planet *p, const char *line);

#endif //PLANET_H
