#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>
#include <SFML/Audio.hpp>

#include "level.h"
#include "state.h"

class Game
{
public:
    Game();
    Game(Game const &) = delete;
    Game &operator=(Game const &) = delete;
    ~Game();

    void run();

private:
    void read_config_file();
    void add_sound_instance(std::string *);
    void update_sound_instances();
    void load_music(State *&, sf::Music &);
    void update_state(State *&);

    int menu_music_volume{};
    int sound_effect_volume{};
    int game_music_volume{};

    int default_level{};
    bool music_enabled_by_default{};
    bool sound_effects_enabled_by_default{};
    int max_game_fps{};

    bool music_was_enabled{};

    State *previous_state{nullptr};
    State *state{nullptr};

    std::vector<sf::Sound *> sound_players{};
    std::vector<sf::SoundBuffer *> sound_buffers{};

    sf::Clock clock{};
    std::string *sound_to_play = new std::string();
    sf::Cursor cursor{};
};

#endif