#ifndef STATIC_OBJECT_H
#define STATIC_OBJECT_H

#include <SFML/Graphics.hpp>
#include <vector>

class StaticObject
{
public:
    StaticObject(double, double, double, int);
    virtual ~StaticObject() = default;

    virtual void load_sprite() = 0;
    virtual void render_object(sf::RenderWindow &window) = 0;
    virtual sf::FloatRect get_bound() const = 0;

    std::vector<double> get_position() const;
    int get_object_id() const;

protected:
    const int id;
    double scale;
    std::vector<double> position{};
    sf::Texture texture{};
    sf::Sprite sprite{};
};

#endif