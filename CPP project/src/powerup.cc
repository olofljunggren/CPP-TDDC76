#include <string>
#include <fstream>
#include <sstream>

#include "../include/static_object.h"
#include "../include/dynamic_object.h"
#include "../include/powerup.h"

using namespace std;

// Constructs a powerup with set type ID.
Powerup::Powerup(
    const int powerup_id, double x_pos,
    double y_pos, double scale) : DynamicObject{x_pos, y_pos, 0, scale, 3},
                                  powerup_id{powerup_id}
{
    load_sprite();
    read_config_file();
}

// Read configuration file and apply default values for instance of powerup.
void Powerup::read_config_file()
{
    string line, value;
    ifstream config_file;
    config_file.open("./data/config/powerup_config.txt");
    if (config_file.fail())
    {
        throw logic_error("Could not read powerup_config.txt");
    }
    string config_type, config_value;

    while (getline(config_file, line))
    {
        istringstream str(line);
        str >> config_type;
        str >> config_value;
        if (config_type == "health")
        {
            health = stoi(config_value);
        }
        else if (config_type == "velocity")
        {
            velocity = stoi(config_value);
        }
        else if (config_type == "angle_velocity")
        {
            angle_velocity = stod(config_value);
        }
        else
        {
            throw logic_error("Corrupt file: powerup_config.txt");
        }
    }
}

// Handles collision with Tank.
void Powerup::collides()
{
    health = 0;
}

// Returns powerup ID.
int Powerup::get_powerup() const
{
    return powerup_id;
}

// Returns powerup texture.
sf::Texture Powerup::get_powerup_texture() const
{
    return texture;
}

// Loads texture and applies on sprite element.
void Powerup::load_sprite()
{
    vector<string> powerup_filenames{"invalid", "speed_powerup", "health_powerup",
                                     "shield_powerup", "splitter_powerup",
                                     "rapidfire_powerup", "explosive_powerup",
                                     "bounce_powerup"};
    if (!texture.loadFromFile("./data/textures/" +
                              powerup_filenames.at(powerup_id) + ".png"))
    {
        throw std::runtime_error{"Could not load: " +
                                 powerup_filenames.at(powerup_id) + ".png"};
    }

    position.at(0) += (64 / 2) * scale;
    position.at(1) += (45 / 2) * scale;
    set_sprite_attributes();
    sprite.setScale(1.3 * scale, 1.3 * scale);

    sf::Vector2u texture_size{texture.getSize()};
    hitbox.setOrigin(texture_size.x / 2, texture_size.y / 2);
    hitbox.setPosition(position.at(0), position.at(1));
    hitbox.setScale(scale, scale);
}
