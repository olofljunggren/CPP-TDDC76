#ifndef BLOCK_H
#define BLOCK_H

#include <SFML/Graphics.hpp>

#include "static_object.h"

class Block : public StaticObject
{
public:
    Block(double, double, std::string, double);
    Block(Block const &) = delete;
    Block &operator=(Block const &) = delete;
    ~Block() = default;

    void render_object(sf::RenderWindow &) override;
    sf::FloatRect get_bound() const override;

private:
    void load_sprite() override;
    
    int health;
    std::string block_type;
};

#endif