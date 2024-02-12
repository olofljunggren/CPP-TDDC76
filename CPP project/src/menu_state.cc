#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <fstream>
#include <sstream>

#include "../include/button.h"
#include "../include/state.h"
#include "../include/game_state.h"
#include "../include/settings_state.h"
#include "../include/control_menu_state.h"
#include "../include/menu_state.h"

using namespace std;

// Constructs state handler for game menu.
Menu_State::Menu_State(
    sf::RenderWindow &window, bool display_fps,
    bool music_enabled, bool sound_effects_enabled,
    int level_id, int player_count) : State{display_fps, music_enabled,
                                                 sound_effects_enabled,
                                                 level_id, player_count, 0},
                                           menu_buttons{}
{
    read_config_file();

    window.create(sf::VideoMode(menu_screen_width, menu_screen_height),
                  "Clash of Tanks Menu", sf::Style::Close);
    load_cursor();
    if (!font.loadFromFile("./data/fonts/font.ttf"))
    {
        throw std::runtime_error{"Kan inte öppna: font.ttf"};
    }
    if (display_fps)
    {
        initialize_fps_counter(window, menu_screen_width, menu_screen_height);
    }

    generate_menu_buttons();
}

// Destructs menu state handler.
Menu_State::~Menu_State()
{
    while (menu_buttons.size() > 0)
    {
        delete menu_buttons.back();
        menu_buttons.pop_back();
    }
}

// Event handler for menu state.
void Menu_State::handle(sf::Event &event, sf::RenderWindow &window,
                        State *&state, std::string *sound_to_play)
{
    if (event.type != sf::Event::MouseButtonPressed)
    {
        return;
    }
    sf::Event::MouseButtonEvent mouse{event.mouseButton};
    if (mouse.button != sf::Mouse::Button::Left)
    {
        return;
    }
    for (auto &button : menu_buttons)
    {
        if (!(button->get_button_bounds()).contains(mouse.x, mouse.y))
        {
            continue;
        }

        *sound_to_play = "click.ogg";
        string on_push_action{button->get_on_push_action()};
        load_new_state(on_push_action, window, state);
    }
}

// Update all parameters for menu state.
void Menu_State::update(sf::Time delta)
{
    update_fps_counter(delta);
}

// Render all objects on screen.
void Menu_State::render(sf::RenderWindow &window)
{
    window.setMouseCursor(cursor);

    window.draw(background);
    if (display_fps)
    {
        window.draw(fps_counter);
    }
    for (auto &button : menu_buttons)
    {
        button->render_button(window);
    }
}

// Read configuration file and apply default values for menu state.
void Menu_State::read_config_file()
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
        if (config_type == "game_screen_width" ||
            config_type == "game_screen_height")
        {
        }
        else if (config_type == "menu_screen_width")
        {
            menu_screen_width = stoi(config_value);
        }
        else if (config_type == "menu_screen_height")
        {
            menu_screen_height = stoi(config_value);
        }
        else if (config_type == "fps_font_size")
        {
            fps_font_size = stod(config_value);
        }
        else
        {
            throw logic_error("Corrupt file: state_config.txt");
        }
    }

    // Sets texture for game background.
    background_color = sf::Color(10, 55, 47);
    set_background(menu_screen_width, menu_screen_height);
}

// Creates Button elements for all buttons in menu.
void Menu_State::generate_menu_buttons()
{
    menu_buttons.push_back(new Button{double(menu_screen_width) / 2,
                                      double(menu_screen_height) / 2 - 180, 3,
                                      "Start game", "game"});
    menu_buttons.push_back(new Button{double(menu_screen_width) / 2,
                                      double(menu_screen_height) / 2 - 60, 3,
                                      "Settings", "settings"});
    menu_buttons.push_back(new Button{double(menu_screen_width) / 2,
                                      double(menu_screen_height) / 2 + 60, 3,
                                      "Controls", "control"});
    menu_buttons.push_back(new Button{double(menu_screen_width) / 2,
                                      double(menu_screen_height) / 2 + 180, 3,
                                      "Quit", "quit"});
}

// When button for new state pressed, load corresponding state.
void Menu_State::load_new_state(string &on_push_action, sf::RenderWindow &window,
                                State *&state)
{
    if (on_push_action == "game")
    {
        state = new Game_State(window, display_fps, music_enabled,
                               sound_effects_enabled, level_id,
                               player_count);
    }
    else if (on_push_action == "quit")
    {
        state = nullptr;
    }
    else if (on_push_action == "settings")
    {
        state = new Settings_State(window, display_fps, music_enabled,
                                   sound_effects_enabled, level_id,
                                   player_count);
    }
    else if (on_push_action == "control")
    {
        state = new Control_Menu_State(window, display_fps, music_enabled,
                                       sound_effects_enabled, level_id,
                                       player_count);
    }
}
