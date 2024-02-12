#ifndef POWERUP_H
#define POWERUP_H

#include <string>
#include <SFML/Graphics.hpp>

class Powerup : public DynamicObject
{
public:
    Powerup(const int, double, double, double);
    Powerup(Powerup const &) = delete;
    Powerup &operator=(Powerup const &) = delete;
    ~Powerup() override = default;

    void read_config_file() override;
    void collides();
    int get_powerup() const;
    sf::Texture get_powerup_texture() const;

private:
    void load_sprite() override;
    const int powerup_id;
    sf::Sprite hitbox{};
};

#endif
