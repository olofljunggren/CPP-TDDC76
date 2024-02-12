#ifndef LEVEL_H
#define LEVEL_H

#include <vector>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <cmath>
#include <random>

#include "static_object.h"
#include "dynamic_object.h"
#include "projectile.h"
#include "block.h"
#include "tank.h"

class Level
{
public:
    Level(int, int, int, int);
    Level(Level const &) = delete;
    Level &operator=(Level const &) = delete;
    ~Level();

    void read_config_file();
    void on_collision(std::string *);
    void render_objects(sf::RenderWindow &);
    void handle_keyboard_input(std::string *);
    double get_scale() const;
    bool get_game_over() const;
    void update(sf::Time );

private:
    void on_collision_tank(std::string *);
    void on_collision_tank_static(Tank *);
    void on_collision_tank_powerup(Tank *, std::string *);
    void on_collision_tank_tank(Tank *);
    void on_collision_tank_projectile(Tank *, std::string *);

    void on_collision_projectile(std::string *);
    void on_collision_projectile_block(Projectile *, std::string *);
    void on_collision_projectile_projectile(Projectile *, std::string *);

    void apply_configuration_values();
    void update_object_positions(sf::Time );
    void check_tank_powerups(sf::Time );
    void find_dead_objects();
    void handle_powerup_spawning(sf::Time );
    sf::Vector2i get_level_bounds(std::fstream &) const;
    void load_level_file(std::fstream &, std::fstream &);
    void read_level(int, int);
    void set_speeds(sf::Keyboard::Key, sf::Keyboard::Key,
                    sf::Keyboard::Key, sf::Keyboard::Key, Tank *);
    void shoot(sf::Keyboard::Key, Tank *, std::string *sound_to_play);
    void do_explosion_damage(Explosive *);
    double calculate_distance(double, double, double, double);
    int select_random_coordinate() const;
    int select_random_powerup() const;

    std::vector<DynamicObject *> dynamic_objects;
    std::vector<StaticObject *> static_objects;
    std::vector<Tank *> tank_objects;

    int level_id;
    int player_count;
    double scale;
    std::vector<sf::Vector2f> spawnable_coordinates;

    double powerup_countdown{};
    double powerup_delay_seconds{};
    int spawn_index{};
    int powerup_count{0};
    int max_powerups{};
    int num_projectiles{};
    double shoot_spread_degrees{};

    double block_width{};
    double block_height{};
    bool game_over{false};
};

#endif
