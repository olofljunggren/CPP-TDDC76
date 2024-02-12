#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <fstream>
#include <sstream>

#include "../include/button.h"
#include "../include/level.h"
#include "../include/state.h"
#include "../include/menu_state.h"
#include "../include/pause_state.h"
#include "../include/game_state.h"
#include "../include/control_menu_state.h"

using namespace std;

// Constructs state handler for active game.
Game_State::Game_State(
    sf::RenderWindow &window, bool display_fps,
    bool music_enabled, bool sound_effects_enabled,
    int level_id, int player_count) : State{display_fps, music_enabled,
                                                 sound_effects_enabled,
                                                 level_id, player_count, 1}
{
    read_config_file();
    window.create(sf::VideoMode(game_screen_width, game_screen_height),
                  "Clash of Tanks", sf::Style::Close);
    current_level = new Level(level_id, player_count, game_screen_width,
                              game_screen_height);

    scale = current_level->get_scale();
    game_over_button = new Button{double(game_screen_width) / 2,
                                  double(game_screen_height / 2) + 50, 3,
                                  "Main menu", "menu"};

    initialize_game_over_text();

    if (display_fps)
    {
        initialize_fps_counter(window, game_screen_width, game_screen_height);
    }
    window.setMouseCursorVisible(false);
}

// Destructs game state handler.
Game_State::~Game_State()
{
    delete current_level;
    delete game_over_button;
}

// Event handler for game state.
void Game_State::handle(sf::Event &event, sf::RenderWindow &window,
                        State *&state, std::string *sound_to_play)
{
    if (current_level->get_game_over())
    {
        window.setMouseCursorVisible(true);
        if (event.type == sf::Event::MouseButtonPressed)
        {
            sf::Event::MouseButtonEvent mouse{event.mouseButton};
            if (mouse.button == sf::Mouse::Button::Left &&
                (game_over_button->get_button_bounds()).contains(mouse.x, mouse.y))
            {
                *sound_to_play = "click.ogg";
                state = new Menu_State(window, display_fps, music_enabled,
                                       sound_effects_enabled, level_id,
                                       player_count);
            }
        }
    }
    else
    {
        if (event.key.code == sf::Keyboard::Key::Escape)
        {
            state = new Pause_State(window, state, display_fps, music_enabled,
                                    sound_effects_enabled, level_id,
                                    player_count);
        }
        current_level->handle_keyboard_input(sound_to_play);
    }
}

// Update all parameters for game state.
void Game_State::update(sf::Time delta, std::string *sound_to_play)
{
    if (!current_level->get_game_over())
    {
        current_level->update(delta);
        current_level->on_collision(sound_to_play);

        update_fps_counter(delta);
    }
}

// Render all objects on screen.
void Game_State::render(sf::RenderWindow &window)
{
    window.draw(background);
    current_level->render_objects(window);
    if (display_fps)
    {
        window.draw(fps_counter);
    }
    if (current_level->get_game_over())
    {
        window.draw(game_over);
        game_over_button->render_button(window);
    }
}

// Returns scale for objects in level.
double Game_State::get_scale() const
{
    return scale;
}

// Read configuration file and apply default values for game state.
void Game_State::read_config_file()
{
    string line, value;
    ifstream config_file;
    config_file.open("./data/config/state_config.txt");
    if (config_file.fail())
    {
        throw logic_error("Could not read state_config.txt");
    }
    string config_type, config_value;

    while (getline(config_file, line))
    {
        istringstream str(line);
        str >> config_type;
        str >> config_value;
        if (config_type == "menu_screen_width" ||
            config_type == "menu_screen_height")
        {
        }
        else if (config_type == "game_screen_width")
        {
            game_screen_width = stoi(config_value);
        }
        else if (config_type == "game_screen_height")
        {
            game_screen_height = stoi(config_value);
        }
        else if (config_type == "fps_font_size")
        {
            str >> config_value;
            fps_font_size = stod(config_value);
        }
        else
        {
            throw logic_error("Corrupt file: state_config.txt");
        }
    }

    // Sets texture for game background.
    background_color = sf::Color(35, 55, 47);
    set_background(game_screen_width, game_screen_height);
}

// Initialize text element for when game over.
void Game_State::initialize_game_over_text()
{
    if (!font.loadFromFile("./data/fonts/font.ttf"))
    {
        throw std::runtime_error{"Kan inte öppna: font.ttf"};
    }
    game_over.setFont(font);
    game_over.setString("Game Over");
    sf::FloatRect text_bounds{game_over.getGlobalBounds()};
    game_over.setOrigin(text_bounds.width / 2, text_bounds.height / 2);
    game_over.setPosition(double(game_screen_width) / 2,
                          double(game_screen_height / 2) - 100);
    game_over.setScale(4 * scale, 4 * scale);
}