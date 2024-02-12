#ifndef TANK_H
#define TANK_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

#include "static_object.h"
#include "dynamic_object.h"
#include "block.h"
#include "powerup.h"
#include "projectile.h"

class Tank : public DynamicObject
{
public:
    Tank(double, double, double, int, double, int, int);
    ~Tank() override = default;

    void read_config_file() override;

    void collides();
    void collides(Powerup *);
    void collides(std::vector<Block *> &);
    void collides(Projectile *);

    void move_hitbox();
    void rotate_hitbox();

    void update_invulnerability_indication();
    void healthbar_damage_shake_indication();

    void draw_hp_and_shield(sf::RenderWindow &);
    void draw_tank_icon(sf::RenderWindow &);
    void draw_current_powerup(sf::RenderWindow &);

    int get_tank_id() const;
    sf::FloatRect get_hitbox();
    double get_max_shoot_delay();
    double get_movement_speed() const;
    double get_rotation_speed() const;
    float get_shoot_cooldown() const;
    bool get_invulnerable() const;
    double get_powerup_duration() const;
    int get_powerup() const;
    int get_shield() const;

    double decrease_powerup_duration(sf::Time );
    void set_powerup(int);

    void reset_shoot_cooldown();
    void reset_powerup_icon();
    void reset_powerup();

private:
    void set_hp_and_shield_location();
    void set_tank_icon_location();
    void set_current_powerup_location();
    void update_health_conditions();
    void update_hp_and_shield_bars();
    void load_sprite() override;
    void load_tank_texture();
    void calculate_and_take_damage(int);
    void load_bar_textures();
    void load_bar_location(bool &, int &, int &, int &, int &);
    void set_powerup_effect(int, bool &);
    void find_allowed_movement_directions(bool &, bool &, std::vector<Block *>);

    int tank_id;
    int current_powerup{};
    int game_screen_width;
    int game_screen_height;
    int shield_health{};
    const int border_width_healthbar{2};
    const int healthbar_padding_space{15};

    double max_shoot_delay_seconds{};
    double powerup_duration_seconds{};

    double speed_powerup_movement_speed{};
    double speed_powerup_rotation_speed{};
    int health_powerup_increase{};
    int shield_powerup_increase{};
    double rapidfire_powerup_max_shoot_delay{};
    int max_tank_health{};
    int max_tank_shield_health{};
    double default_powerup_duration{};
    double default_movement_speed{};
    double default_rotation_speed{};
    double default_max_shoot_delay{};
    double max_invulnerable_duration{};

    double rotation_speed{};
    double movement_speed{};

    bool is_invulnerable{false};
    sf::Clock invulnerable_duration{};
    bool healthbar_is_shaking{false};
    sf::Clock healthbar_shake_duration{};
    const double max_healthbar_shake_duration{0.5}; // Unit: seconds


    sf::Clock shoot_cooldown{};

    sf::Sprite tank_icon{};
    sf::Sprite hitbox{};
    sf::Sprite healthbar{};
    sf::Sprite shieldbar_sprite{};
    sf::Sprite current_hp_sprite{};
    sf::Sprite current_powerup_sprite{};

    sf::Texture tank_icon_texture{};
    sf::Texture hitbox_texture{};
    sf::Texture healthbar_texture{};
    sf::Texture shieldbar_texture{};
    sf::Texture current_hp_texture{};
    sf::Texture current_powerup_texture{};
};

#endif
