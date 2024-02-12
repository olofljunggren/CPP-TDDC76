#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include <random>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "../include/static_object.h"
#include "../include/dynamic_object.h"
#include "../include/tank.h"
#include "../include/level.h"
#include "../include/block.h"
#include "../include/projectile.h"
#include "../include/powerup.h"

using namespace std;

// Constructs level to manage all objects in game.
Level::Level(
    int level_id, int player_count,
    int screen_width, int screen_height) : dynamic_objects{}, static_objects{},
                                           tank_objects{},
                                           level_id{level_id},
                                           player_count{player_count},
                                           scale{}, spawnable_coordinates{}
{
    read_config_file();
    read_level(screen_width, screen_height);
}

// Removes all objects in game.
Level::~Level()
{
    while (static_objects.size() > 0)
    {
        delete static_objects.back();
        static_objects.pop_back();
    }

    while (dynamic_objects.size() > 0)
    {
        delete dynamic_objects.back();
        dynamic_objects.pop_back();
    }

    while (tank_objects.size() > 0)
    {
        delete tank_objects.back();
        tank_objects.pop_back();
    }
}

// Read configuration file and apply default values for instance of level.
void Level::read_config_file()
{
    string line, value;
    ifstream config_file;
    config_file.open("./data/config/level_config.txt");
    if (config_file.fail())
    {
        throw logic_error("Could not read level_config.txt");
    }
    string config_type, config_value;

    while (getline(config_file, line))
    {
        istringstream str(line);
        str >> config_type;
        str >> config_value;
        if (config_type == "block_width")
        {
            block_width = stod(config_value);
        }
        else if (config_type == "block_height")
        {
            block_height = stod(config_value);
        }
        else if (config_type == "splitter_powerup_num_projectiles")
        {
            num_projectiles = stoi(config_value);
        }
        else if (config_type == "splitter_powerup_angle_spread_degrees")
        {
            shoot_spread_degrees = stod(config_value);
        }
        else if (config_type == "powerup_delay_seconds")
        {
            powerup_delay_seconds = stod(config_value);
            powerup_countdown = powerup_delay_seconds;
        }
        else if (config_type == "max_powerups")
        {
            max_powerups = stoi(config_value);
        }
        else
        {
            throw logic_error("Corrupt file: level_config.txt");
        }
    }
}

// For all pairs of objects in level, check if bounds intersect. If yes,
// call collision methods for objects involved.
void Level::on_collision(std::string *sound_to_play)
{
    on_collision_tank(sound_to_play);
    on_collision_projectile(sound_to_play);
}

// For every tank in level, check for collisions with other objects.
void Level::on_collision_tank(string *sound_to_play)
{
    for (auto &tank : tank_objects)
    {
        on_collision_tank_static(tank);
        on_collision_tank_powerup(tank, sound_to_play);
        on_collision_tank_tank(tank);
        on_collision_tank_projectile(tank, sound_to_play);
    }
}

// Check if tank collided with static objects.
void Level::on_collision_tank_static(Tank *tank)
{
    sf::FloatRect tank_bound = tank->get_hitbox();
    vector<Block *> block_collisions{};

    for (auto &static_object : static_objects)
    {
        Block *block = dynamic_cast<Block *>(static_object);
        if (block != nullptr)
        {
            sf::FloatRect block_bound = block->get_bound();
            if (tank_bound.intersects(block_bound))
            {
                block_collisions.push_back(block);
            }
        }
    }
    if (!block_collisions.empty())
    {
        tank->collides(block_collisions);
    }
}

// Check if tank collided with powerup.
void Level::on_collision_tank_powerup(Tank *tank, string *sound_to_play)
{
    sf::FloatRect tank_bound = tank->get_bound();
    for (auto &dynamic_object : dynamic_objects)
    {
        Powerup *powerup = dynamic_cast<Powerup *>(dynamic_object);
        if (powerup == nullptr)
        {
            continue;
        }

        sf::FloatRect powerup_bound = powerup->get_bound();
        if (tank_bound.intersects(powerup_bound))
        {
            int powerup_id = powerup->get_powerup();
            if ((powerup_id == 1 || powerup_id == 4 || powerup_id == 5 ||
                 powerup_id == 6 || powerup_id == 7) &&
                tank->get_powerup() != 0)
            {
                continue;
            }
            *sound_to_play = "pop.ogg";
            tank->collides(powerup);
            powerup->collides();
            sf::Vector2f new_spawnable_coordinate{
                float(powerup->get_position().at(0) -
                      (block_width / 2.0) * scale),
                float(powerup->get_position().at(1) -
                      (block_height / 2.0) * scale)};
            spawnable_coordinates.push_back(new_spawnable_coordinate);
            powerup_count--;
        }
    }
}

// Check if tank collided with another tank.
void Level::on_collision_tank_tank(Tank *tank)
{
    sf::FloatRect tank_bound = tank->get_bound();

    for (auto &second_tank : tank_objects)
    {
        if (second_tank->get_tank_id() == tank->get_tank_id())
        {
            continue;
        }
        sf::FloatRect second_tank_bound = second_tank->get_hitbox();
        if (tank_bound.intersects(second_tank_bound) && tank != second_tank)
        {
            tank->collides();
            second_tank->collides();
        }
    }
}

// Check if tank collided with projectile.
void Level::on_collision_tank_projectile(Tank *tank, string *sound_to_play)
{
    sf::FloatRect tank_bound = tank->get_bound();
    for (auto &dynamic_object : dynamic_objects)
    {
        Projectile *projectile{dynamic_cast<Projectile *>(dynamic_object)};
        if (projectile == nullptr ||
            projectile->get_shooter_id() == tank->get_tank_id() ||
            tank->get_invulnerable())
        {
            continue;
        }
        sf::FloatRect projectile_bounds = projectile->get_bound();
        if (!tank_bound.intersects(projectile_bounds))
        {
            continue;
        }
        if (projectile->get_object_id() == 0)
        {
            Explosive *explosive{dynamic_cast<Explosive *>(projectile)};
            if (!(explosive->get_has_exploded()))
            {
                *sound_to_play = "explosion.ogg";
                explosive->collides();
                do_explosion_damage(explosive);
            }
        }
        else
        {
            *sound_to_play = "impact.ogg";
            projectile->collides();
            tank->collides(projectile);
        }
    }
}

// For every projectile in level, check for collisions with other objects.
void Level::on_collision_projectile(string *sound_to_play)
{
    for (auto &dynamic_object : dynamic_objects)
    {
        Projectile *projectile{dynamic_cast<Projectile *>(dynamic_object)};
        if (projectile == nullptr)
        {
            continue;
        }

        on_collision_projectile_block(projectile, sound_to_play);
        on_collision_projectile_projectile(projectile, sound_to_play);
    }
}

// Check if projectile collided with blocks.
void Level::on_collision_projectile_block(Projectile *projectile,
                                          string *sound_to_play)
{
    sf::FloatRect projectile_bound{projectile->get_bound()};
    vector<Block *> static_collisions{};
    BouncingBullet *bouncing_bullet = dynamic_cast<BouncingBullet *>(projectile);

    for (auto &static_object : static_objects)
    {
        Block *block{dynamic_cast<Block *>(static_object)};
        sf::FloatRect block_bound = block->get_bound();
        if (!projectile_bound.intersects(block_bound))
        {
            continue;
        }
        if (projectile->get_object_id() == 0)
        {
            Explosive *explosive{dynamic_cast<Explosive *>(projectile)};
            if (!(explosive->get_has_exploded()))
            {
                *sound_to_play = "explosion.ogg";
                do_explosion_damage(explosive);
                explosive->collides();
            }
        }
        else if (projectile->get_object_id() == 5)
        {
            static_collisions.push_back(block);
            *sound_to_play = "bounce.ogg";
        }
        else
        {
            projectile->collides();
        }
    }
    if (!static_collisions.empty() && bouncing_bullet != nullptr)
    {
        bouncing_bullet->collides_bounce(static_collisions);
    }
}

// Check if projectile collided with other projectile.
void Level::on_collision_projectile_projectile(Projectile *projectile,
                                               string *sound_to_play)
{
    sf::FloatRect projectile_bound{projectile->get_bound()};
    for (auto &dynamic_object : dynamic_objects)
    {
        Projectile *second_projectile{dynamic_cast<Projectile *>(dynamic_object)};

        if (second_projectile == nullptr || projectile == second_projectile)
        {
            continue;
        }
        sf::FloatRect second_projectile_bound{second_projectile->get_bound()};
        if (projectile_bound.intersects(second_projectile_bound))
        {
            projectile->collides();
            second_projectile->collides();
            if (projectile->get_object_id() != 0)
            {
                continue;
            }

            Explosive *explosive{dynamic_cast<Explosive *>(projectile)};
            if (!(explosive->get_has_exploded()))
            {
                *sound_to_play = "explosion.ogg";
                do_explosion_damage(explosive);
                explosive->collides();
            }
        }
    }
}

// Renders all object in game on screen in correct height order.
void Level::render_objects(sf::RenderWindow &window)
{
    // Renders all static objects (blocks).
    for (auto &object : static_objects)
    {
        object->render_object(window);
    }

    // Renders all powerups.
    for (auto &object : dynamic_objects)
    {
        if (object->get_object_id() == 3)
        {
            object->render_object(window);
        }
    }

    // Renders all tanks.
    for (auto &tank : tank_objects)
    {
        tank->render_object(window);
        tank->draw_hp_and_shield(window);
        tank->draw_tank_icon(window);
        tank->draw_current_powerup(window);
    }

    // Renders remaining dynamic objects (Projectiles).
    for (auto &object : dynamic_objects)
    {
        // Powerups with object ID 3 already rendered above.
        if (object->get_object_id() == 3)
        {
            continue;
        }
        object->render_object(window);

        Explosive *explosive = dynamic_cast<Explosive *>(object);
        if (explosive == nullptr)
        {
            continue;
        }
        if (explosive->get_has_exploded())
        {
            explosive->render_explosion(window);
        }
    }
    if (tank_objects.size() <= 1)
    {
        game_over = true;
    }
}

// Handles keyboard events during gameplay.
void Level::handle_keyboard_input(std::string *sound_to_play)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Home))
    {
        apply_configuration_values();
    }
    for (unsigned int i{0}; i < tank_objects.size(); i++)
    {
        int tank_id = tank_objects.at(i)->get_tank_id();
        if (tank_id == 1)
        {
            set_speeds(sf::Keyboard::Key::W, sf::Keyboard::Key::S,
                       sf::Keyboard::Key::A, sf::Keyboard::Key::D,
                       tank_objects.at(i));
            shoot(sf::Keyboard::Key::E, tank_objects.at(i), sound_to_play);
        }
        else if (tank_id == 2)
        {
            set_speeds(sf::Keyboard::Key::Numpad8, sf::Keyboard::Key::Numpad5,
                       sf::Keyboard::Key::Numpad4, sf::Keyboard::Key::Numpad6,
                       tank_objects.at(i));
            shoot(sf::Keyboard::Key::Numpad9, tank_objects.at(i), sound_to_play);
        }
        else if (tank_id == 3)
        {
            set_speeds(sf::Keyboard::Key::Up, sf::Keyboard::Key::Down,
                       sf::Keyboard::Key::Left, sf::Keyboard::Key::Right,
                       tank_objects.at(i));
            shoot(sf::Keyboard::Key::RControl, tank_objects.at(i), sound_to_play);
        }
        else if (tank_id == 4)
        {
            set_speeds(sf::Keyboard::Key::I, sf::Keyboard::Key::K,
                       sf::Keyboard::Key::J, sf::Keyboard::Key::L,
                       tank_objects.at(i));
            shoot(sf::Keyboard::Key::O, tank_objects.at(i), sound_to_play);
        }
    }
}

// Returns global level scale.
double Level::get_scale() const
{
    return scale;
}

// Returns boolean value of whether or not game has ended.
bool Level::get_game_over() const
{
    return game_over;
}

// Updates level attributes. Called every frame.
void Level::update(sf::Time delta)
{
    update_object_positions(delta);
    check_tank_powerups(delta);
    find_dead_objects();
    handle_powerup_spawning(delta);
}

// Reload configuration values for Tank, Dynamic Object and Level.
void Level::apply_configuration_values()
{
    read_config_file();
    for (unsigned int i{0}; i < tank_objects.size(); i++)
    {
        tank_objects.at(i)->read_config_file();
    }
    for (unsigned int i{0}; i < dynamic_objects.size(); i++)
    {
        dynamic_objects.at(i)->read_config_file();
    }
}

// Update position and rotation for all objects according to velocity and time step.
void Level::update_object_positions(sf::Time delta)
{
    long unsigned int dynamic_object_count{dynamic_objects.size()};
    long unsigned int tank_count{tank_objects.size()};

    for (long unsigned int i{}; i < dynamic_object_count; i++)
    {
        dynamic_objects.at(i)->move(delta);
        dynamic_objects.at(i)->rotate(delta);
    }

    for (long unsigned int i{}; i < tank_count; i++)
    {
        tank_objects.at(i)->move(delta);
        tank_objects.at(i)->move_hitbox();
        tank_objects.at(i)->rotate(delta);
        tank_objects.at(i)->update_invulnerability_indication();
        tank_objects.at(i)->healthbar_damage_shake_indication();
    }
}

// Handles countdown of powerup duration, and removes effect when appropriate.
void Level::check_tank_powerups(sf::Time delta)
{
    long unsigned int tank_count{tank_objects.size()};
    for (long unsigned int i{}; i < tank_count; i++)
    {
        if (tank_objects.at(i)->get_powerup() == 0)
        {
            continue;
        }

        if (tank_objects.at(i)->decrease_powerup_duration(delta) < 0)
        {
            tank_objects.at(i)->set_powerup(0);
        }
    }
}

// Finds and removes objects in game with zero health.
void Level::find_dead_objects()
{
    long unsigned int dynamic_object_count{dynamic_objects.size()};
    for (long unsigned int i{}; i < dynamic_object_count;)
    {
        Explosive *explosive =
            dynamic_cast<Explosive *>(dynamic_objects.at(i));
        if (explosive != nullptr)
        {
            if (explosive->get_health() == 0 && explosive->get_timer() > 0.8)
            {
                std::swap(dynamic_objects.at(i), dynamic_objects.back());
                delete dynamic_objects.back();
                dynamic_objects.pop_back();
                dynamic_object_count--;
            }
            else
            {
                i++;
            }
        }
        else
        {
            if (dynamic_objects.at(i)->get_health() <= 0)
            {
                std::swap(dynamic_objects.at(i), dynamic_objects.back());
                delete dynamic_objects.back();
                dynamic_objects.pop_back();
                dynamic_object_count--;
            }
            else
            {
                i++;
            }
        }
    }
    long unsigned int tank_count{tank_objects.size()};
    for (long unsigned int i{}; i < tank_count;)
    {
        if (tank_objects.at(i)->get_health() <= 0)
        {
            std::swap(tank_objects.at(i), tank_objects.back());
            delete tank_objects.back();
            tank_objects.pop_back();
            tank_count--;
        }
        else
        {
            i++;
        }
    }
}

// Manages the spawning of powerups in level. If cooldown time zero, spawns new powerup.
void Level::handle_powerup_spawning(sf::Time delta)
{
    powerup_countdown -= delta.asSeconds();

    if (powerup_countdown <= 0 && spawnable_coordinates.size() > 0 &&
        powerup_count < max_powerups)
    {
        int powerup_coordinate_index{select_random_coordinate()};
        int powerup_id{select_random_powerup()};

        sf::Vector2f powerup_location =
            spawnable_coordinates.at(powerup_coordinate_index);
        spawnable_coordinates.erase(spawnable_coordinates.begin() +
                                    powerup_coordinate_index);
        dynamic_objects.push_back(new Powerup(powerup_id, powerup_location.x,
                                              powerup_location.y, scale));
        powerup_countdown = powerup_delay_seconds;
        powerup_count++;
    }
}

// Returns maximal width and height of level, measured in block count.
sf::Vector2i Level::get_level_bounds(fstream &level_reader) const
{
    string line, object;
    sf::Vector2i level_bounds{};
    int i{0}, j{0};
    int max_j{0};
    while (getline(level_reader, line))
    {
        j = 0;
        stringstream str(line);
        while (getline(str, object, ';'))
        {
            j++;
        }
        i++;
        if (j > max_j)
        {
            max_j = j;
        }
    }
    level_bounds.x = max_j;
    level_bounds.y = i;
    return level_bounds;
}

// Loads world file for chosen level.
void Level::load_level_file(fstream &level_reader, fstream &bound_reader)
{
    string level_file_path{"./data/levels/level" +
                           to_string(level_id) + ".csv"};
    level_reader.open(level_file_path, ios::in);
    bound_reader.open(level_file_path, ios::in);
}

// Spawns elements in level according to external level file.
void Level::read_level(int screen_width, int screen_height)
{

    fstream level_reader;
    fstream bound_reader;
    load_level_file(level_reader, bound_reader);
    sf::Vector2i level_bounds{get_level_bounds(bound_reader)};

    string line, object;

    // Calculates level scale to fit level on screen.
    scale = min((0.8 * double(screen_height)) /
                    (level_bounds.y * double(block_height)),
                (1.0 * double(screen_width) - 2.0 * block_width) /
                    (level_bounds.x * double(block_width)));

    // Update block size accoding to current scale.
    block_width *= scale;
    block_height *= scale;
    double i{(screen_height - 90) / (10 * block_height)};
    double j{((screen_width / block_width) - level_bounds.x) / 2};

    while (getline(level_reader, line))
    {
        j = ((screen_width / block_width) - level_bounds.x) / 2;
        stringstream str(line);
        while (getline(str, object, ';'))
        {
            // Check if powerup spawnable coordinate.
            if (object == "s")
            {
                spawnable_coordinates.push_back(
                    sf::Vector2f{float(j * block_width),
                                 float(i * block_height)});
            }
            // Check if block should be spawned.
            else if (object == "﻿wall_1_top")
            {
                Block *block = new Block(j * block_width, i * block_height,
                                         "wall_1_top", scale);
                static_objects.push_back(block);
            }
            else if (object == "wall_1_top" || object == "wall_1_bot" ||
                     object == "wall_2_top" || object == "wall_2_bot" ||
                     object == "wall_3_top" || object == "wall_3_bot" ||
                     object == "3d_box")
            {
                Block *block = new Block(j * block_width, i * block_height,
                                         object, scale);
                static_objects.push_back(block);
            }
            // Check if tank should be spawned.
            else if (object == "1" || object == "2" || object == "3" ||
                     object == "4")
            {
                int object_number = stoi(object);
                if (object_number <= player_count)
                {
                    tank_objects.push_back(
                        new Tank(j * block_width, i * block_height, 0,
                                 object_number, scale,
                                 screen_width, screen_height));
                }
            }
            j++;
        }
        i++;
    }
}

// Sets speed for tank according to user input.
void Level::set_speeds(sf::Keyboard::Key forward, sf::Keyboard::Key backward,
                       sf::Keyboard::Key rotate_left,
                       sf::Keyboard::Key rotate_right, Tank *tank)
{
    // Set movement speed
    if (sf::Keyboard::isKeyPressed(forward))
    {
        tank->set_velocity(tank->get_movement_speed());
    }
    else if (sf::Keyboard::isKeyPressed(backward))
    {
        tank->set_velocity(-(tank->get_movement_speed()));
    }
    else
    {
        tank->set_velocity(0);
    }

    // Set rotation speed
    if (sf::Keyboard::isKeyPressed(rotate_left))
    {
        tank->set_angle_velocity(-(tank->get_rotation_speed()));
    }
    else if (sf::Keyboard::isKeyPressed(rotate_right))
    {
        tank->set_angle_velocity(tank->get_rotation_speed());
    }
    else
    {
        tank->set_angle_velocity(0);
    }
}

// Fire projectiles from tank if shoot button pressed.
void Level::shoot(sf::Keyboard::Key shoot_button, Tank *tank,
                  std::string *sound_to_play)
{
    if (!sf::Keyboard::isKeyPressed(shoot_button) ||
        tank->get_shoot_cooldown() < tank->get_max_shoot_delay())
    {
        return;
    }

    vector<double> tank_position{tank->get_position()};
    double x_pos{tank_position.at(0)};
    double y_pos{tank_position.at(1)};
    double angle{tank->get_angle()};
    double tank_velocity{tank->get_velocity()};
    int shooter_id{tank->get_tank_id()};

    if (tank->get_powerup() == 4)
    {
        *sound_to_play = "splitter.ogg";
    }
    else
    {
        *sound_to_play = "shoot.ogg";
    }

    switch (tank->get_powerup())
    {
    // Current powerup for tank is splitter shot.
    case 4:
    {
        // Spawn the projectiles in a cone shape in front of the tank.
        for (int i{}; i < num_projectiles; i++)
        {
            double angle_offset{angle - shoot_spread_degrees / 2 +
                                i * shoot_spread_degrees /
                                    (num_projectiles - 1)};

            dynamic_objects.push_back(new Bullet(x_pos, y_pos, angle_offset,
                                                 shooter_id, scale, tank_velocity));
        }
        break;
    }
    // Current powerup for tank is explosive ammo.
    case 6:
    {
        // Spawn a exploding bullet
        dynamic_objects.push_back(new Explosive(x_pos, y_pos, angle,
                                                shooter_id, scale, tank_velocity));
        break;
    }
    // Current powerup for tank is bouncing bullet.
    case 7:
    {
        // Spawn a bouncing bullet
        dynamic_objects.push_back(new BouncingBullet(x_pos, y_pos, angle,
                                                     shooter_id, scale, tank_velocity));
        break;
    }
    // Tank has no ammo type related powerup.
    default:
    {
        // Spawn a normal bullet
        dynamic_objects.push_back(new Bullet(x_pos, y_pos, angle,
                                             shooter_id, scale, tank_velocity));
        break;
    }
    }

    tank->reset_shoot_cooldown();
}

// Apply explosion damage to all tanks withing range of explosion.
void Level::do_explosion_damage(Explosive *explosive)
{
    vector<double> explosion_position{explosive->get_position()};
    unsigned int explosion_radius = explosive->get_explosion_radius();
    double distance{};

    for (auto &tank : tank_objects)
    {
        if (tank->get_tank_id() == explosive->get_shooter_id())
        {
            continue;
        }
        vector<double> tank_position = tank->get_position();
        distance = calculate_distance(tank_position.at(0), tank_position.at(1),
                                      explosion_position.at(0),
                                      explosion_position.at(1));
        if (distance < explosion_radius + 20)
        {
            tank->collides(explosive);
        }
    }
}

// Calculate euclidean distance between object and explosion.
double Level::calculate_distance(double object_x, double object_y,
                                 double explosion_x, double explosion_y)
{
    return pow(pow(explosion_x - object_x, 2) +
                   pow(explosion_y - object_y, 2),
               0.5);
}

// Returns random index amongst powerup spawnable coordinates.
int Level::select_random_coordinate() const
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, spawnable_coordinates.size() - 1);
    return distr(gen);
}

// Returns random index amongst types of powerup.
int Level::select_random_powerup() const
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(1, 7);
    return distr(gen);
}
