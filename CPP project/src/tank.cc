#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "../include/tank.h"
#include "../include/static_object.h"
#include "../include/dynamic_object.h"
#include "../include/projectile.h"
#include "../include/powerup.h"

using namespace std;

// Constructs a tank with a tank ID.
Tank::Tank(
    double x_pos, double y_pos, double angle,
    int tank_id, double scale, int game_screen_width,
    int game_screen_height) : DynamicObject{x_pos, y_pos, angle, scale, 1},
                              tank_id{tank_id},
                              game_screen_width{game_screen_width},
                              game_screen_height{game_screen_height}
{
    read_config_file();
    load_sprite();
}

// Read configuration file and apply default values for instance of Tank.
void Tank::read_config_file()
{
    string line, value;
    ifstream config_file;
    config_file.open("./data/config/tank_config.txt");
    if (config_file.fail())
    {
        throw logic_error("Could not read tank_config.txt");
    }
    string config_type, config_value;

    while (getline(config_file, line))
    {
        istringstream str(line);
        str >> config_type;
        str >> config_value;

        if (config_type == "max_tank_health")
        {
            max_tank_health = stoi(config_value);
        }
        else if (config_type == "max_tank_shield_health")
        {
            max_tank_shield_health = stoi(config_value);
        }
        else if (config_type == "tank_health")
        {
            health = stoi(config_value);
        }
        else if (config_type == "tank_movement_speed")
        {
            movement_speed = stod(config_value) * scale;
            default_movement_speed = stod(config_value);
        }
        else if (config_type == "tank_rotation_speed")
        {
            default_rotation_speed = stod(config_value);
            rotation_speed = default_rotation_speed;
        }
        else if (config_type == "tank_max_shoot_delay")
        {
            default_max_shoot_delay = stod(config_value);
            max_shoot_delay_seconds = default_max_shoot_delay;
        }
        else if (config_type == "tank_shield_health")
        {
            shield_health = stoi(config_value);
        }
        else if (config_type == "default_powerup_duration")
        {
            default_powerup_duration = stod(config_value);
            powerup_duration_seconds = default_powerup_duration;
        }
        else if (config_type == "speed_powerup_speed")
        {
            speed_powerup_movement_speed = stod(config_value);
        }
        else if (config_type == "speed_powerup_rotation_speed")
        {
            speed_powerup_rotation_speed = stod(config_value);
        }
        else if (config_type == "health_powerup_increase")
        {
            health_powerup_increase = stoi(config_value);
        }
        else if (config_type == "shield_powerup_increase")
        {
            shield_powerup_increase = stoi(config_value);
        }
        else if (config_type == "rapidfire_powerup_max_shoot_delay")
        {
            rapidfire_powerup_max_shoot_delay = stod(config_value);
        }
        else if (config_type == "max_invulnerable_duration")
        {
            max_invulnerable_duration = stod(config_value);
        }
        else
        {
            throw logic_error("Corrupt file: tank_config.txt");
        }
    }
}

// Handles tank collision with other tank.
void Tank::collides()
{
    set_position(old_position.at(0), old_position.at(1));
    set_angle(old_angle);
}

// Handles tank collision with powerup.
void Tank::collides(Powerup *powerup)
{
    int powerup_id{powerup->get_powerup()};
    bool is_health_or_shield{false};

    set_powerup_effect(powerup_id, is_health_or_shield);

    if (!is_health_or_shield)
    {
        current_powerup_texture = powerup->get_powerup_texture();
        current_powerup_sprite.setTexture(current_powerup_texture);
    }
    update_hp_and_shield_bars();
}

// Handles tank collision with blocks.
void Tank::collides(vector<Block *> &block_collisions)
{
    bool x_allowed_move{true};
    bool y_allowed_move{true};

    find_allowed_movement_directions(x_allowed_move, y_allowed_move,
                                     block_collisions);

    if (x_allowed_move)
    {
        set_position(position.at(0), old_position.at(1));
        hitbox.setPosition(position.at(0), old_position.at(1));
    }
    else
    {
        position.at(0) = old_position.at(0);
    }

    if (y_allowed_move)
    {
        set_position(old_position.at(0), position.at(1));
        hitbox.setPosition(old_position.at(0), position.at(1));
    }
    else
    {
        position.at(1) = old_position.at(1);
    }
}

// Handles tank collision with normal or bouncing bullet.
void Tank::collides(Projectile *projectile)
{
    calculate_and_take_damage(projectile->get_damage());
}

// Moves hitbox location according to tank position.
void Tank::move_hitbox()
{
    hitbox.setPosition(position.at(0), position.at(1));
}

// Rotates hitbox according to tank rotation.
void Tank::rotate_hitbox()
{
    hitbox.setRotation(angle);
}

// Renders breathing effect for tank when invulnerable.
void Tank::update_invulnerability_indication()
{
    double current_time{invulnerable_duration.getElapsedTime().asSeconds()};

    if (current_time >= max_invulnerable_duration)
    {
        is_invulnerable = false;
    }

    if (is_invulnerable)
    {
        double alpha{255 * abs(sin(current_time * 3))};
        sprite.setColor(sf::Color(255, 255, 255, alpha));
    }
    else
    {
        sprite.setColor(sf::Color(255, 255, 255, 255));
    }
}

// Shakes the healthbar left and right.
void Tank::healthbar_damage_shake_indication()
{
    double current_time{healthbar_shake_duration.getElapsedTime().asSeconds()};

    if (current_time >= max_healthbar_shake_duration)
    {
        healthbar_is_shaking = false;
    }
    if (healthbar_is_shaking)
    {
        double current_time{invulnerable_duration.getElapsedTime().asSeconds()};
        healthbar.move(2 * sin(current_time * 50), 0.0);
        current_hp_sprite.move(2 * sin(current_time * 50), 0.0);
        shieldbar_sprite.move(2 * sin(current_time * 50), 0.0);
    }
}

// Renders HP and shield bars on screen.
void Tank::draw_hp_and_shield(sf::RenderWindow &window)
{
    // Defines border for bars.
    sf::FloatRect hp_bounds{healthbar.getGlobalBounds()};
    sf::RectangleShape hp_and_shield_border(
        sf::Vector2f(hp_bounds.width + border_width_healthbar * 2,
                     hp_bounds.height * 2 + border_width_healthbar * 3));
    hp_and_shield_border.setPosition(healthbar.getPosition().x -
                                         border_width_healthbar,
                                     healthbar.getPosition().y -
                                         border_width_healthbar);
    hp_and_shield_border.setFillColor(sf::Color(30, 30, 30));

    // Draw bar elements
    window.draw(hp_and_shield_border);
    window.draw(healthbar);
    window.draw(current_hp_sprite);
    window.draw(shieldbar_sprite);
}

// Renders tank icon on screen.
void Tank::draw_tank_icon(sf::RenderWindow &window)
{
    window.draw(tank_icon);
}

// Renders current powerup icon on screen.
void Tank::draw_current_powerup(sf::RenderWindow &window)
{
    window.draw(current_powerup_sprite);
}

// Returns tank ID.
int Tank::get_tank_id() const
{
    return tank_id;
}

// Returns global bounds of tank.
sf::FloatRect Tank::get_hitbox()
{
    return hitbox.getGlobalBounds();
}

// Returns maximal value for shoot cooldown.
double Tank::get_max_shoot_delay()
{
    return max_shoot_delay_seconds;
}

// Returns movement speed for tank.
double Tank::get_movement_speed() const
{
    return movement_speed;
}

// Returns rotation movement speed for tank.
double Tank::get_rotation_speed() const
{
    return rotation_speed;
}

// Returns time remaining until next possible shot [sec].
float Tank::get_shoot_cooldown() const
{
    return shoot_cooldown.getElapsedTime().asSeconds();
}

// Returns whether or not tank is invulnerable.
bool Tank::get_invulnerable() const
{
    return is_invulnerable;
}

// Returns remaining duration of active powerup.
double Tank::get_powerup_duration() const
{
    return powerup_duration_seconds;
}

// Returns ID of active powerup.
int Tank::get_powerup() const
{
    return current_powerup;
}

// Returns shield health.
int Tank::get_shield() const
{
    return shield_health;
}

// Decreases remaining duration of active powerup.
double Tank::decrease_powerup_duration(sf::Time delta)
{
    return powerup_duration_seconds -= delta.asSeconds();
}

// Sets ID to active powerup.
void Tank::set_powerup(int powerup)
{
    if (powerup != 0)
    {
        powerup_duration_seconds = default_powerup_duration;
    }
    else
    {
        reset_powerup();
        reset_powerup_icon();
    }
    current_powerup = powerup;
}

// Resets time remaining until next possible shot.
void Tank::reset_shoot_cooldown()
{
    shoot_cooldown.restart();
}

// Sets active powerup icon to empty.
void Tank::reset_powerup_icon()
{
    if (!current_powerup_texture.loadFromFile(
            "./data/textures/empty_powerup.png"))
    {
        throw std::runtime_error{"Kan inte öppna: empty_powerup.png"};
    }
    current_powerup_sprite.setTexture(current_powerup_texture);
}

// Removes active powerup from tank.
void Tank::reset_powerup()
{
    movement_speed = default_movement_speed * scale;
    rotation_speed = default_rotation_speed;
    max_shoot_delay_seconds = default_max_shoot_delay;
}

// Sets location for health and shield bars based on Tank ID.
void Tank::set_hp_and_shield_location()
{
    int x_pos_hp{0}, y_pos_hp{0};
    int x_pos_shield{0}, y_pos_shield{0};
    sf::Vector2u hp_bar_size{healthbar_texture.getSize()};
    sf::Vector2u shield_bar_size{healthbar_texture.getSize()};
    double hp_scale{double(health) / double(max_tank_health)};
    double shield_scale{double(shield_health) / double(max_tank_shield_health)};
    bool pos_on_left{true};

    load_bar_location(pos_on_left, x_pos_hp, y_pos_hp, x_pos_shield,
                      y_pos_shield);

    if (pos_on_left)
    {
        current_hp_sprite.setPosition(x_pos_hp, y_pos_hp);
        shieldbar_sprite.setPosition(x_pos_shield, y_pos_shield);
    }
    else
    {
        current_hp_sprite.setPosition(x_pos_hp +
                                          hp_bar_size.x * (1 - hp_scale),
                                      y_pos_hp);
        shieldbar_sprite.setPosition(x_pos_shield +
                                         shield_bar_size.x * (1 - shield_scale),
                                     y_pos_shield);
    }
    healthbar.setPosition(x_pos_hp, y_pos_hp);
    current_hp_sprite.setScale(hp_scale, 1);
    shieldbar_sprite.setScale(shield_scale, 1);
}

// Sets location for tank icon based on Tank ID.
void Tank::set_tank_icon_location()
{
    unsigned int hp_bar_width{healthbar_texture.getSize().x};
    int x_pos{0}, y_pos{0};
    double spacing_constant = hp_bar_width +
                              2 * border_width_healthbar +
                              4 * healthbar_padding_space;
    bool pos_on_left{true};
    switch (tank_id)
    {
    case 1:
        x_pos = spacing_constant;
        y_pos = 2.7 * healthbar_padding_space;
        break;
    case 2:
        x_pos = game_screen_width - spacing_constant;
        y_pos = 2.7 * healthbar_padding_space;
        pos_on_left = false;
        break;
    case 3:
        x_pos = spacing_constant;
        y_pos = game_screen_height - 2.4 * healthbar_padding_space;
        break;
    case 4:
        x_pos = game_screen_width - spacing_constant;
        y_pos = game_screen_height - 2.4 * healthbar_padding_space;
        pos_on_left = false;
        break;
    default:
        throw logic_error("Invalid tank ID.");
        break;
    }

    if (pos_on_left)
    {
        tank_icon.setRotation(90);
    }
    else
    {
        tank_icon.setRotation(270);
    }

    sf::Vector2u icon_size{texture.getSize()};
    tank_icon.setOrigin(icon_size.x / 2, icon_size.y / 2);
    tank_icon.setPosition(x_pos, y_pos);
}

// Sets location for current powerup icon based on Tank ID.
void Tank::set_current_powerup_location()
{
    unsigned int hp_bar_width{healthbar_texture.getSize().x};
    unsigned int tank_icon_width{tank_icon_texture.getSize().x};
    int x_pos{0}, y_pos{0};
    double spacing_constant = hp_bar_width + 2 * border_width_healthbar +
                              7 * healthbar_padding_space + tank_icon_width;
    switch (tank_id)
    {
    case 1:
        x_pos = spacing_constant;
        y_pos = 1.6 * healthbar_padding_space;
        break;
    case 2:
        x_pos = game_screen_width - 1.12 * spacing_constant;
        y_pos = 1.6 * healthbar_padding_space;
        break;
    case 3:
        x_pos = spacing_constant;
        y_pos = game_screen_height - 3.5 * healthbar_padding_space;
        break;
    case 4:
        x_pos = game_screen_width - 1.12 * spacing_constant;
        y_pos = game_screen_height - 3.5 * healthbar_padding_space;
        break;

    default:
        throw logic_error("Invalid tank ID.");
        break;
    }

    current_powerup_sprite.setPosition(x_pos, y_pos);
    current_powerup_sprite.setScale(1.5, 1.5);
}

// Updates shield and health bar values and sets invulnerability.
void Tank::update_health_conditions()
{
    update_hp_and_shield_bars();

    is_invulnerable = true;
    invulnerable_duration.restart();

    healthbar_is_shaking = true;
    healthbar_shake_duration.restart();
}

// Updates length of HP and shield bars according to current tank health.
void Tank::update_hp_and_shield_bars()
{
    double hp_scale{double(health) / double(max_tank_health)};
    double shield_scale{double(shield_health) / double(max_tank_shield_health)};
    current_hp_sprite.setScale(hp_scale, 1);
    shieldbar_sprite.setScale(shield_scale, 1);
    set_hp_and_shield_location();
}

// Loads tank texture and applies on sprite element.
void Tank::load_sprite()
{
    load_tank_texture();
    load_bar_textures();
    sprite.setTexture(texture);
    tank_icon = sprite;

    sf::Vector2u texture_size{texture.getSize()};
    sprite.setOrigin(texture_size.x / 2.0, texture_size.y / 2 + 6.5);
    position.at(0) += 32.0 * scale;
    position.at(1) += 45.0 * scale + 6.5;
    sprite.setPosition(position.at(0), position.at(1));
    sprite.setRotation(angle);
    sprite.setScale(scale, scale);

    // Loads hitbox for tank, smaller than tank sprite.
    if (!hitbox_texture.loadFromFile("./data/textures/green_tank.png",
                                     sf::IntRect(0, 25, 25, 25)))
    {
        throw std::runtime_error{"Could not load: green_tank.png"};
    }
    hitbox.setTexture(hitbox_texture);
    sf::Vector2u hitbox_texture_size{hitbox_texture.getSize()};
    hitbox.setOrigin(hitbox_texture_size.x / 2, hitbox_texture_size.y / 2);
    hitbox.setPosition(position.at(0), position.at(1));
    hitbox.setScale(scale, scale);

    set_hp_and_shield_location();
    set_tank_icon_location();
    set_current_powerup_location();
}

// Loads tank texture according to tank ID.
void Tank::load_tank_texture()
{
    switch (tank_id)
    {
    case 1:
        if (!texture.loadFromFile("./data/textures/green_tank.png"))
        {
            throw runtime_error{"Could not load: green_tank.png"};
        }
        break;
    case 2:
        if (!texture.loadFromFile("./data/textures/red_tank.png"))
        {
            throw runtime_error{"Could not load: red_tank.png"};
        }
        break;
    case 3:
        if (!texture.loadFromFile("./data/textures/purple_tank.png"))
        {
            throw runtime_error{"Could not load: purple_tank.png"};
        }
        break;
    case 4:
        if (!texture.loadFromFile("./data/textures/blue_tank.png"))
        {
            throw runtime_error{"Could not load: blue_tank.png"};
        }
        break;
    default:
        throw logic_error("Invalid tank ID.");
        break;
    }
}

// Removes argument from shield and/or health.
void Tank::calculate_and_take_damage(int damage)
{
    if (damage < shield_health)
    {
        shield_health -= damage;
    }
    else
    {
        int remainder{damage - shield_health};
        shield_health = 0;
        health -= remainder;
    }
    update_health_conditions();
}

// Loads textures for health and shield bar and applies to sprite.
void Tank::load_bar_textures()
{
    // Load health bar
    if (!healthbar_texture.loadFromFile("./data/textures/red_hp.png"))
    {
        throw std::runtime_error{"Could not load: red_hp.png"};
    }
    healthbar.setTexture(healthbar_texture);

    if (!current_hp_texture.loadFromFile("./data/textures/green_hp.png"))
    {
        throw std::runtime_error{"Could not load: green_hp.png"};
    }
    current_hp_sprite.setTexture(current_hp_texture);

    // Load shield bar
    if (!shieldbar_texture.loadFromFile("./data/textures/shield_bar.png"))
    {
        throw std::runtime_error{"Could not load: shield_bar.png"};
    }
    shieldbar_sprite.setTexture(shieldbar_texture);
}

// Calculates position for health and shield bars.
void Tank::load_bar_location(bool &pos_on_left, int &x_pos_hp, int &y_pos_hp,
                             int &x_pos_shield, int &y_pos_shield)
{
    sf::Vector2u hp_bar_size{healthbar_texture.getSize()};
    sf::Vector2u shield_bar_size{healthbar_texture.getSize()};
    switch (tank_id)
    {
    case 1:
        // Top left location
        x_pos_hp = healthbar_padding_space;
        y_pos_hp = healthbar_padding_space;
        x_pos_shield = x_pos_hp;
        y_pos_shield = y_pos_hp + border_width_healthbar + hp_bar_size.y;
        break;
    case 2:
        // Top right location
        x_pos_hp = game_screen_width - hp_bar_size.x - healthbar_padding_space;
        y_pos_hp = healthbar_padding_space;
        x_pos_shield = x_pos_hp;
        y_pos_shield = healthbar_padding_space + border_width_healthbar + hp_bar_size.y;
        pos_on_left = false;
        break;
    case 3:
        // Bottom left location
        x_pos_hp = healthbar_padding_space;
        y_pos_hp = game_screen_height - 2 * hp_bar_size.y - healthbar_padding_space;
        x_pos_shield = x_pos_hp;
        y_pos_shield = y_pos_hp + border_width_healthbar + hp_bar_size.y;
        break;
    case 4:
        // Bottom right location
        x_pos_hp = game_screen_width - hp_bar_size.x - healthbar_padding_space;
        y_pos_hp = game_screen_height - 2 * hp_bar_size.y - healthbar_padding_space;
        x_pos_shield = x_pos_hp;
        y_pos_shield = y_pos_hp + border_width_healthbar + hp_bar_size.y;
        pos_on_left = false;
        break;
    default:
        throw logic_error("Incorrect tank ID.");
        break;
    }
}

// Modifies tank attributes based on powerup ID.
void Tank::set_powerup_effect(int powerup_id, bool &is_health_or_shield)
{
    switch (powerup_id)
    {
    case 1:
        // Speed powerup
        movement_speed *= speed_powerup_movement_speed;
        rotation_speed *= speed_powerup_rotation_speed;

        break;
    case 2:
        // Health powerup
        is_health_or_shield = true;
        if (health + health_powerup_increase < max_tank_health)
        {
            health += health_powerup_increase;
        }
        else
        {
            health = max_tank_health;
        }
        break;
    case 3:
        // Shield powerup
        is_health_or_shield = true;
        if (shield_health + shield_powerup_increase < max_tank_shield_health)
        {
            shield_health += shield_powerup_increase;
        }
        else
        {
            shield_health = max_tank_shield_health;
        }
        break;
    case 5:
        // Rapid fire powerup
        max_shoot_delay_seconds = rapidfire_powerup_max_shoot_delay;
        break;
    default:
        break;
    }

    if (!is_health_or_shield)
    {
        set_powerup(powerup_id);
    }
    else
    {
        update_hp_and_shield_bars();
    }
}

// For all directions, evaluate if movement is allowed.
void Tank::find_allowed_movement_directions(bool &x_allowed_move,
                                            bool &y_allowed_move,
                                            vector<Block *> block_collisions)
{
    double dx{10};
    double dy{10};

    // Check if collision occured in x direction.
    hitbox.setPosition(old_position.at(0) + dx, old_position.at(1));
    sf::FloatRect tank_bound_plus_dx = get_hitbox();
    hitbox.setPosition(old_position.at(0) - 2.0 * dx, old_position.at(1));
    sf::FloatRect tank_bound_minus_dx = get_hitbox();
    hitbox.setPosition(old_position.at(0) + dx, old_position.at(1));
    // Check if collision occured in y direction.
    hitbox.setPosition(old_position.at(0), old_position.at(1) + dy);
    sf::FloatRect tank_bound_plus_dy = get_hitbox();
    hitbox.setPosition(old_position.at(0), old_position.at(1) - 2.0 * dy);
    sf::FloatRect tank_bound_minus_dy = get_hitbox();
    hitbox.setPosition(old_position.at(0), old_position.at(1) + dy);

    for (auto &block : block_collisions)
    {
        sf::FloatRect block_bound{block->get_bound()};
        x_allowed_move = x_allowed_move &&
                         !(tank_bound_plus_dx.intersects(block_bound) ||
                           tank_bound_minus_dx.intersects(block_bound));
        y_allowed_move = y_allowed_move &&
                         !(tank_bound_plus_dy.intersects(block_bound) ||
                           tank_bound_minus_dy.intersects(block_bound));
    }
}
