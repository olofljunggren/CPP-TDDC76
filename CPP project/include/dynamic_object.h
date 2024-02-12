#ifndef DYNAMIC_OBJECT_H
#define DYNAMIC_OBJECT_H

#include <SFML/Graphics.hpp>
#include <vector>

#include "static_object.h"

class DynamicObject
{
public:
    DynamicObject(double, double, double, double, unsigned int);
    DynamicObject(DynamicObject const &) = delete;
    DynamicObject &operator=(DynamicObject const &) = delete;
    virtual ~DynamicObject() = default;

    virtual void read_config_file() = 0;

    void move(sf::Time );
    void rotate(sf::Time );
    void render_object(sf::RenderWindow &);

    int get_health() const;
    std::vector<double> get_position() const;
    double get_angle() const;
    double get_velocity() const;
    sf::FloatRect get_bound();
    unsigned int get_object_id() const;

    void set_health(unsigned int);
    void set_position(double, double);
    void set_angle(double);
    void set_angle_velocity(double);
    void set_velocity(double);
    void set_sprite_attributes();

private:
    virtual void load_sprite() = 0;

protected:
    int health{};
    const unsigned int object_id;

    std::vector<double> old_position{};
    std::vector<double> position{};
    double velocity{};
    double angle_velocity{};

    double angle;
    double old_angle;
    double scale;

    sf::Texture texture{};
    sf::Sprite sprite{};
};

#endif
