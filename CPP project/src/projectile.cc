#include <cmath>
#include <fstream>
#include <sstream>

#include "../include/projectile.h"
#include "../include/static_object.h"
#include "../include/dynamic_object.h"
#include "../include/tank.h"
#include "../include/block.h"

using namespace std;

// Constructs a projectile, storing ID of shooter.
Projectile::Projectile(
    double x_pos, double y_pos, double angle, int shooter_id,
    double scale, unsigned int object_id) : DynamicObject{x_pos, y_pos, angle,
                                                          scale, object_id},
                                            shooter_id{shooter_id}
{
    health = 1;
}

// Handles collision for projectile.
void Projectile::collides()
{
    if (object_id == 0)
    {
        Explosive *explosive = dynamic_cast<Explosive *>(this);
        if (explosive->get_has_exploded())
        {
            return;
        }
    }
    detonate();
}

// Returns damage dealt by projectile.
int Projectile::get_damage() const
{
    return damage;
}

// Returns tank ID of shooter.
int Projectile::get_shooter_id() const
{
    return shooter_id;
}

// Constructs regular bullet and increases speed by tank speed.
Bullet::Bullet(
    double x_pos, double y_pos, double angle, int shooter_id, double scale,
    double tank_velocity) : Projectile{
                                x_pos + 40 * scale * sin(M_PI / 180 * angle),
                                y_pos - 40 * scale * cos(M_PI / 180 * angle),
                                angle, shooter_id, scale, 2}
{
    read_config_file();
    load_sprite();
    velocity += tank_velocity;
}

// Sets health to zero, killing bullet next frame.
void Bullet::detonate()
{
    health = 0;
}

// Read configuration file and apply default values for instance of bullet.
void Bullet::read_config_file()
{
    string line, value;
    ifstream config_file;
    config_file.open("./data/config/projectile_config.txt");
    if (config_file.fail())
    {
        throw logic_error("Could not read projectile_config.txt");
    }
    string config_type, config_value;

    while (getline(config_file, line))
    {
        istringstream str(line);
        str >> config_type;
        str >> config_value;
        // If property does not apply to bullet, read next configuration file line.
        if (config_type == "explosive_ammo_damage" ||
            config_type == "explosive_ammo_range" ||
            config_type == "explosive_ammo_velocity" ||
            config_type == "allowed_bounces")
        {
            continue;
        }
        else if (config_type == "health")
        {
            health = stoi(config_value);
        }
        else if (config_type == "bullet_damage")
        {
            damage = stoi(config_value);
        }
        else if (config_type == "bullet_velocity")
        {
            velocity = stod(config_value) * scale;
        }
        else
        {
            throw logic_error("Corrupt file: projectile_config.txt");
        }
    }
}

// Loads texture and applies on sprite element.
void Bullet::load_sprite()
{
    if (!texture.loadFromFile("./data/textures/bullet.png"))
    {
        throw std::runtime_error{"Kan inte öppna: bullet.png"};
    }

    set_sprite_attributes();
}

// Constructs explosive projectile.
Explosive::Explosive(
    double x_pos, double y_pos, double angle, int shooter_id, double scale,
    double tank_velocity) : Projectile{
                                x_pos + 30 * scale * sin(M_PI / 180 * angle),
                                y_pos - 30 * scale * cos(M_PI / 180 * angle),
                                angle, shooter_id, scale, 0},
                            exploded{false}
{
    read_config_file();
    load_sprite();
    velocity = velocity + tank_velocity;
}

// If not already detonated, detonate explosion.
void Explosive::detonate()
{
    if (!exploded)
    {
        velocity = 0;
        health = 0;
        explosion_time.restart();
        if (!explosion_img.loadFromFile("./data/textures/explosion.png"))
        {
            throw std::runtime_error{"Kan inte öppna: explosion.png"};
        }

        explosion.setTexture(explosion_img);

        sf::Vector2u texture_size{explosion_img.getSize()};
        explosion.setOrigin(texture_size.x / 2, texture_size.y / 2);
        explosion.setPosition(position.at(0), position.at(1));
        explosion.setScale(scale * explosion_radius / 10,
                           scale * explosion_radius / 10);

        exploded = true;
    }
}

// Read configuration file and apply values for instance of explosive projectile.
void Explosive::read_config_file()
{
    string line, value;
    ifstream config_file;
    config_file.open("./data/config/projectile_config.txt");
    if (config_file.fail())
    {
        throw logic_error("Could not read projectile_config.txt");
    }
    string config_type, config_value;

    while (getline(config_file, line))
    {
        istringstream str(line);
        str >> config_type;
        str >> config_value;
        if (config_type == "bullet_damage" || config_type == "bullet_velocity")
        {
        }
        else if (config_type == "health" || config_type == "allowed_bounces")
        {
            health = stoi(config_value);
        }
        else if (config_type == "explosive_ammo_damage")
        {
            damage = stoi(config_value);
        }
        else if (config_type == "explosive_ammo_range")
        {
            explosion_radius = stoi(config_value) * scale;
        }
        else if (config_type == "explosive_ammo_velocity")
        {
            velocity = stod(config_value) * scale;
        }
        else
        {
            throw logic_error("Corrupt file: projectile_config.txt");
        }
    }
}

// Renders detonated explosion on screen.
void Explosive::render_explosion(sf::RenderWindow &window)
{
    window.draw(explosion);
}

// Returns boolean value of wether or not explosive projectile has detonated.
bool Explosive::get_has_exploded() const
{
    return exploded;
}

// Returns range of explosion [px].
double Explosive::get_explosion_radius() const
{
    return explosion_radius;
}

// Returns time since exploded [sec].
double Explosive::get_timer() const
{
    return explosion_time.getElapsedTime().asSeconds();
}

// Loads texture and applies on sprite element.
void Explosive::load_sprite()
{
    if (!texture.loadFromFile("./data/textures/exploding_bullet.png"))
    {
        throw std::runtime_error{"Kan inte öppna: exploding_bullet.png"};
    }

    set_sprite_attributes();
}

// Constructs a bouncing bullet.
BouncingBullet::BouncingBullet(
    double x_pos, double y_pos, double angle, int shooter_id, double scale,
    double tank_velocity) : Projectile{x_pos, y_pos, angle, shooter_id, scale, 5}
{
    read_config_file();
    load_sprite();
    velocity = velocity + tank_velocity;
}

// Sets health to zero, killing bouncing bullet next frame.
void BouncingBullet::detonate()
{
    health = 0;
}

// Read configuration file and apply values for instance of bouncing bullet.
void BouncingBullet::read_config_file()
{
    string line, value;
    ifstream config_file;
    config_file.open("./data/config/projectile_config.txt");
    if (config_file.fail())
    {
        throw logic_error("Could not read projectile_config.txt");
    }
    string config_type, config_value;

    while (getline(config_file, line))
    {
        istringstream str(line);
        str >> config_type;
        str >> config_value;
        if (config_type == "explosive_ammo_damage" || config_type == "explosive_ammo_range" ||
            config_type == "explosive_ammo_velocity")
        {
        }
        else if (config_type == "health")
        {
            health = stoi(config_value);
        }
        else if (config_type == "bullet_damage")
        {
            damage = stoi(config_value);
        }
        else if (config_type == "bullet_velocity")
        {
            velocity = stod(config_value) * scale;
        }
        else if (config_type == "allowed_bounces")
        {
            remaining_bounces = stoi(config_value);
        }
        else
        {
            throw logic_error("Corrupt file: projectile_config.txt");
        }
    }
}

// Handles updating of direction after bouncing bullet collision.
// Kills bullet if number of allowed bounces zero.
void BouncingBullet::collides_bounce(vector<Block *> &block_collisions)
{
    bool allowed_move_x{true};
    bool allowed_move_y{true};
    find_bounce_collision_direction(
        allowed_move_x, allowed_move_y, block_collisions);

    if (get_remaining_bounces())
    {
        if (allowed_move_x)
        {
            set_position(old_position.at(0), old_position.at(1));
            sprite.setPosition(old_position.at(0), old_position.at(1));
            angle = 180 - angle;
            remaining_bounces--;
        }

        if (allowed_move_y)
        {
            set_position(old_position.at(0), old_position.at(1));
            sprite.setPosition(old_position.at(0), old_position.at(1));
            angle = -angle;
            remaining_bounces--;
        }
    }
    else
    {
        detonate();
    }
}

// Returns remaining number of allowed bounces.
int BouncingBullet::get_remaining_bounces() const
{
    return remaining_bounces;
}

// Check all possible directions for direction of collision.
void BouncingBullet::find_bounce_collision_direction(
    bool &allowed_move_x, bool &allowed_move_y, vector<Block *> &block_collisions)
{
    double dx{20};
    double dy{20};

    // Checks if collision occured in x direction.
    sprite.setPosition(old_position.at(0) + dx, old_position.at(1));
    sf::FloatRect bound_plus_dx = get_bound();
    sprite.setPosition(old_position.at(0) - 2.0 * dx, old_position.at(1));
    sf::FloatRect bound_minus_dx = get_bound();
    sprite.setPosition(old_position.at(0) + dx, old_position.at(1));

    // Checks if collision occured in y direction.
    sprite.setPosition(old_position.at(0), old_position.at(1) + dy);
    sf::FloatRect bound_plus_dy = get_bound();
    sprite.setPosition(old_position.at(0), old_position.at(1) - 2.0 * dy);
    sf::FloatRect bound_minus_dy = get_bound();
    sprite.setPosition(old_position.at(0), old_position.at(1) + dy);

    for (auto &object : block_collisions)
    {
        allowed_move_x = allowed_move_x &&
                         !(bound_plus_dx.intersects(object->get_bound()) ||
                           bound_minus_dx.intersects(object->get_bound()));
        allowed_move_y = allowed_move_y &&
                         !(bound_plus_dy.intersects(object->get_bound()) ||
                           bound_minus_dy.intersects(object->get_bound()));
    }
}

// Loads texture and applies on sprite element.
void BouncingBullet::load_sprite()
{
    if (!texture.loadFromFile("./data/textures/bouncing_bullet.png"))
    {
        throw std::runtime_error{"Kan inte öppna: bouncing_bullet.png"};
    }

    set_sprite_attributes();
    sprite.setScale(scale * 2, scale * 2);
}