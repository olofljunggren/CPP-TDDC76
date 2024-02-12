#include <SFML/Graphics.hpp>
#include <fstream>
#include <sstream>

#include "../include/block.h"

using namespace std;

// Constructs a single block in level with a health.
Block::Block(double x_pos, double y_pos, string block_type, double scale): 
    StaticObject{x_pos, y_pos, scale, 4}, health{1}, block_type{block_type}
{
    load_sprite();
}

// Renders the block on screen.
void Block::render_object(sf::RenderWindow &window)
{
    window.draw(sprite);
}

// Returns the global rectangle bound of block.
sf::FloatRect Block::get_bound() const
{
    return sprite.getGlobalBounds();
}

// Loads texture depending on block-type and applies on sprite element.
void Block::load_sprite()
{
    if (!texture.loadFromFile("./data/textures/" + block_type + ".png"))
    {
        throw std::runtime_error{"Kan inte öppna " +  block_type + ".png"};
    }
    sprite.setTexture(texture);
    sprite.setPosition(position.at(0), position.at(1));
    sprite.setScale(scale, scale);
}