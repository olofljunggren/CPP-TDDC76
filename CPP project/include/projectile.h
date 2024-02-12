#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "static_object.h"
#include "dynamic_object.h"
#include "block.h"

class Projectile : public DynamicObject
{
public:
    Projectile(double, double, double, int, double, unsigned int);
    Projectile(Projectile const &) = delete;
    Projectile &operator=(Projectile const &) = delete;
    ~Projectile() override = default;

    virtual void detonate() = 0;

    void collides();

    int get_damage() const;
    int get_shooter_id() const;

private:
    virtual void load_sprite() = 0;

protected:
    int damage{};
    int shooter_id;
};

class Bullet : public Projectile
{
public:
    Bullet(double, double, double, int, double, double);
    Bullet(Bullet const &) = delete;
    Bullet &operator=(Bullet const &) = delete;
    ~Bullet() = default;

    void detonate() override;
    void read_config_file() override;

private:
    void load_sprite() override;
};

class Explosive : public Projectile
{
public:
    Explosive(double, double, double, int, double, double);
    Explosive(Explosive const &) = delete;
    Explosive &operator=(Explosive const &) = delete;
    ~Explosive() = default;

    void detonate() override;
    void read_config_file() override;

    void render_explosion(sf::RenderWindow &);
    bool get_has_exploded() const;

    double get_explosion_radius() const;
    double get_timer() const;

private:
    void load_sprite() override;
    bool exploded;
    double explosion_radius{};
    sf::Clock explosion_time{};

    sf::Texture explosion_img{};
    sf::Sprite explosion{};
};

class BouncingBullet : public Projectile
{
public:
    BouncingBullet(double, double, double, int, double, double);
    BouncingBullet(BouncingBullet const &) = delete;
    BouncingBullet &operator=(BouncingBullet const &) = delete;
    ~BouncingBullet() = default;

    void detonate() override;
    void read_config_file() override;

    void collides_bounce(std::vector<Block *> &);

    int get_remaining_bounces() const;

private:
    void find_bounce_collision_direction(bool &, bool &, std::vector<Block *> &);
    void load_sprite() override;
    int remaining_bounces{};
};

#endif