#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <fstream>
#include <sstream>

#include "../include/state.h"

using namespace std;

// Constructs state handler with music and sound effect support, and state ID.
State::State(
    bool display_fps, bool music_enabled,
    bool sound_effects_enabled,
    int level_id, int player_count,
    unsigned int state_id) : display_fps{display_fps},
                             music_enabled{music_enabled},
                             sound_effects_enabled{sound_effects_enabled},
                             level_id{level_id},
                             player_count{player_count}, state_id{state_id} {}

// Sets up parameteters for in game display of Frames Per Second.
void State::initialize_fps_counter(sf::RenderWindow &window, int screen_width,
                                   int screen_height)
{
    if (!font.loadFromFile("./data/fonts/font.ttf"))
    {
        throw std::runtime_error{"Kan inte öppna: font.ttf"};
    }
    fps_counter.setFont(font);
    fps_counter.setString("FPS: 000");
    sf::Vector2u const window_size{window.getSize()};
    sf::FloatRect fps_counter_bounds{fps_counter.getGlobalBounds()};
    fps_counter.setOrigin(fps_counter_bounds.width / 2, 0);
    fps_counter.setPosition(screen_width / 2, 0.03 * screen_height);
    fps_counter.setScale(fps_font_size, fps_font_size);
}

// Calculates Frames Per Second using time since last frame.
void State::update_fps_counter(sf::Time delta)
{
    elapsed_time += delta.asSeconds();
    if ((int(elapsed_time * 100) % 5) == 0 && display_fps)
    {
        int current_fps{int((1 / delta.asSeconds()))};
        fps_counter.setString("FPS: " + to_string(current_fps));
    }
}

// Loads cursor texture and applies to user cursor.
void State::load_cursor()
{
    if (!cursor.loadFromSystem(sf::Cursor::Cross))
    {
        throw std::runtime_error{"Kan inte öppna: Crosshair"};
    }
}

// Returns state ID of active state.
unsigned int State::get_state_id() const
{
    return state_id;
}

// Returns whether or not music is enabled.
bool State::get_music_enabled() const
{
    return music_enabled;
}

// Returns whether or not sound effects are enabled.
bool State::get_sound_effects_enabled() const
{
    return sound_effects_enabled;
}

void State::set_background(unsigned int size_x, unsigned int size_y)
{
    background.setSize(sf::Vector2f(size_x, size_y));
    background.setPosition(0, 0);
    background.setFillColor(background_color);
}