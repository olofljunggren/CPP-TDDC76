#include <cmath>
#include <SFML/Graphics.hpp>

#include "../include/dynamic_object.h"

using namespace std;

// Constructs a movable object in a level.
DynamicObject::DynamicObject(
    double x_pos, double y_pos, double angle,
    double scale, unsigned int object_id) : object_id{object_id}, angle{angle},
                                            old_angle{angle}, scale{scale}
{
    position.push_back(x_pos);
    position.push_back(y_pos);
    old_position = position;
}

// Updates the object position according to angle and velocity.
void DynamicObject::move(sf::Time delta)
{
    double angle_radians = angle * M_PI / 180;
    old_position = position;
    position.at(0) += velocity * delta.asSeconds() * sin(angle_radians);
    position.at(1) -= velocity * delta.asSeconds() * cos(angle_radians);
    sprite.setPosition(position.at(0), position.at(1));
}

// Rotates the object according to angle and angle velocity.
void DynamicObject::rotate(sf::Time delta)
{
    old_angle = angle;
    angle += angle_velocity * delta.asSeconds();
    sprite.setRotation(angle);
}

// Renders object on screen.
void DynamicObject::render_object(sf::RenderWindow &window)
{
    window.draw(sprite);
}

// Returns health.
int DynamicObject::get_health() const
{
    return health;
}

// Returns position of object.
vector<double> DynamicObject::get_position() const
{
    return position;
}

// Returns angle of object.
double DynamicObject::get_angle() const
{
    return angle;
}

// Returns velocity of object.
double DynamicObject::get_velocity() const
{
    return velocity;
}

// Returns global rectangle bound of object.
sf::FloatRect DynamicObject::get_bound()
{
    return sprite.getGlobalBounds();
}

// Returns identifier of object.
unsigned int DynamicObject::get_object_id() const
{
    return object_id;
}

// Sets current health to argument.
void DynamicObject::set_health(unsigned int new_health)
{
    health = new_health;
}

// Sets current position to arguments.
void DynamicObject::set_position(double new_pos_x, double new_pos_y)
{
    position.at(0) = new_pos_x;
    position.at(1) = new_pos_y;
    sprite.setPosition(new_pos_x, new_pos_y);
}

// Sets current angle to argument [degrees].
void DynamicObject::set_angle(double new_angle)
{
    angle = new_angle;
    sprite.setRotation(angle);
}

// Sets current angle velocity to argument [degrees].
void DynamicObject::set_angle_velocity(double rotation_speed)
{
    angle_velocity = rotation_speed;
}

// Sets current velocity to argument.
void DynamicObject::set_velocity(double movement_speed)
{
    velocity = movement_speed;
}

// Sets common sprite graphics attributes.
void DynamicObject::set_sprite_attributes()
{
    sprite.setTexture(texture);
    sf::Vector2u texture_size{texture.getSize()};
    sprite.setOrigin(texture_size.x / 2, texture_size.y / 2);
    sprite.setPosition(position.at(0), position.at(1));
    sprite.setRotation(angle);
    sprite.setScale(scale, scale);
}