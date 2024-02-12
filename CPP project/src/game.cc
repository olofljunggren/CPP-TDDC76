#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include "../include/level.h"
#include "../include/game.h"
#include "../include/menu_state.h"
#include "../include/game_state.h"
#include "../include/pause_state.h"
#include "../include/settings_state.h"
#include "../include/control_menu_state.h"
#include "../include/state.h"

using namespace std;

// Constructs a game instance.
Game::Game()
{
    read_config_file();
    run();
}

// Destructs a game instance.
Game::~Game()
{
    while (sound_players.size() != 0)
    {
        delete sound_players.back();
        sound_players.pop_back();
    }
    while (sound_buffers.size() != 0)
    {
        delete sound_buffers.back();
        sound_buffers.pop_back(); 
    }
    delete sound_to_play;
}

// Main loop of game session, handles logistics for state.
void Game::run()
{
    sf::RenderWindow window{};
    state = new Menu_State(window, true, music_enabled_by_default,
                                  sound_effects_enabled_by_default,
                                  default_level, 4);
    previous_state = state;
    sf::Music music;
    // Loads background music for main menu.
    if (!music.openFromFile("./data/sound/menu.ogg"))
    {
        throw logic_error("Could not load menu.ogg");
    }

    music.setLoop(true);
    music.setVolume(menu_music_volume);
    if (state->get_music_enabled())
    {
        music.play();
        music_was_enabled = true;
    }

    while (window.isOpen())
    {
        sf::Event event;

        if (*sound_to_play != "" && state->get_sound_effects_enabled())
        {
            add_sound_instance(sound_to_play);
        }

        // If any sound effects stopped, remove sound instance.
        update_sound_instances();

        // Handle all window events in queue.
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                delete previous_state;
                delete state;
                window.close();
                break;
            }

            state->handle(event, window, state, sound_to_play);
            // If pressed quit, delete last state and break game loop
            if (state == nullptr)
            {      
                delete previous_state;
                window.setMouseCursor(cursor);
                window.close();
                break;
            }
            // On changed value of state and game not paused
            else if (state != previous_state && (state->get_state_id() != 2))
            {   
                if(previous_state != nullptr)
                {   
                    delete previous_state;
                }
                previous_state = state;

                load_music(state, music);
            }
            else if (state != previous_state && (state->get_state_id() == 2))
            {
                previous_state = state;
                load_music(state, music);
            }


            if (state->get_music_enabled() != music_was_enabled)
            {
                if (state->get_music_enabled())
                {
                    music.play();
                    music_was_enabled = true;
                }
                else
                {
                    music.pause();
                    music_was_enabled = false;
                }
            }
        }

        // Call update method for current state.
        if(state != nullptr)
        {
            update_state(state);
            window.clear();
            state->render(window);
            window.display();
            window.setFramerateLimit(max_game_fps);
        }
    }
}

// Read configuration file and apply default values for instance of game.
void Game::read_config_file()
{
    string line, value;
    ifstream config_file;
    config_file.open("./data/config/game_config.txt");
    if (config_file.fail())
    {
        throw logic_error("Could not read game_config.txt");
    }
    string config_type, config_value;

    while (getline(config_file, line))
    {
        istringstream str(line);
        str >> config_type;
        str >> config_value;
        if (config_type == "max_game_fps")
        {
            max_game_fps = stoi(config_value);
        }
        else if (config_type == "default_level")
        {
            default_level = stoi(config_value);
        }
        else if (config_type == "menu_music_volume")
        {
            menu_music_volume = stoi(config_value);
        }
        else if (config_type == "sound_effect_volume")
        {
            sound_effect_volume = stoi(config_value);
        }
        else if (config_type == "game_music_volume")
        {
            game_music_volume = stoi(config_value);
        }
        else if (config_type == "music_enabled_by_default")
        {
            music_was_enabled = (config_value == "true");
            music_enabled_by_default = music_was_enabled;
        }
        else if (config_type == "sound_effects_enabled_by_default")
        {
            sound_effects_enabled_by_default = (config_value == "true");
        }
        else
        {
            throw logic_error("Corrupt file: game_config.txt");
        }
    }
}

// When new sound effect requested, add sound to sound effect queue.
void Game::add_sound_instance(string *sound_to_play)
{
    sf::SoundBuffer *buffer = new sf::SoundBuffer;
    sf::Sound *sound_effect = new sf::Sound;

    if (!buffer->loadFromFile("./data/sound/" + *sound_to_play))
    {
        throw logic_error("Could not load " + *sound_to_play);
    }

    sound_buffers.push_back(buffer);
    sound_effect->setBuffer(*buffer);
    sound_effect->setVolume(sound_effect_volume);
    *sound_to_play = "";
    sound_effect->play();
    sound_players.push_back(sound_effect);
}

// Check if any sound instances are stopped. If true, remove from queue.
void Game::update_sound_instances()
{
    for (unsigned int i{0}; i < sound_players.size(); i++)
    {
        if (sound_players.at(i)->getStatus() == sf::Sound::Status::Stopped)
        {
            swap(sound_players.at(i), sound_players.back());
            swap(sound_buffers.at(i), sound_buffers.back());
            delete sound_players.back();
            delete sound_buffers.back();
            sound_buffers.pop_back();
            sound_players.pop_back();
        }
    }
}

// Starts music session according to current state.
void Game::load_music(State *&state, sf::Music &music)
{
    string music_path;
    switch (state->get_state_id())
    {
    case 1:
        music_path = "./data/sound/visager_battle.ogg";
        music.setVolume(game_music_volume);
        break;
    default:
        music_path = "./data/sound/menu.ogg";
        music.setVolume(menu_music_volume);
        break;
    }
    if (!music.openFromFile(music_path))
    {
        throw logic_error("Could not load " + music_path);
    }
    if (state->get_music_enabled())
    {
        music.play();
        music_was_enabled = true;
    }
}

// Calls respective update method for current state.
void Game::update_state(State *&state)
{
    switch (state->get_state_id())
    {
    case 0:
    {
        Menu_State *menu_state = dynamic_cast<Menu_State *>(state);
        menu_state->update(clock.restart());
        break;
    }
    case 1:
    {
        Game_State *game_state = dynamic_cast<Game_State *>(state);
        game_state->update(clock.restart(), sound_to_play);
        break;
    }
    case 2:
    {
        Pause_State *pause_state = dynamic_cast<Pause_State *>(state);
        pause_state->update(clock.restart());
        break;
    }
    case 3:
    {
        Settings_State *settings_state = dynamic_cast<Settings_State *>(state);
        settings_state->update(clock.restart());
        break;
    }
    case 4:
    {
        Control_Menu_State *control_state =
            dynamic_cast<Control_Menu_State *>(state);
        control_state->update(clock.restart());
        break;
    }
    default:
        throw logic_error("Invalid state id.");
        break;
    }
}