#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <fstream>
#include <sstream>

#include "../include/button.h"
#include "../include/state.h"
#include "../include/menu_state.h"
#include "../include/control_menu_state.h"

using namespace std;

// Constructs state handler for controls window.
Control_Menu_State::Control_Menu_State(
    sf::RenderWindow &window, bool display_fps,
    bool music_enabled, bool sound_effects_enabled,
    int level_id, int player_count) : State{display_fps, music_enabled,
                                                 sound_effects_enabled,
                                                 level_id, player_count, 4}
{
    read_config_file();
    load_controls_info_texture();
    window.create(sf::VideoMode(menu_screen_width, menu_screen_height),
                  "Clash of Tanks Settings", sf::Style::Close);
    load_cursor();
    initialize_fps_counter(window, menu_screen_width, menu_screen_height);

    if (!font.loadFromFile("./data/fonts/font.ttf"))
    {
        throw std::runtime_error{"Kan inte öppna: font.ttf"};
    }

    buttons.push_back(new Button{145, 0.9 * menu_screen_height, 2.4, "Return",
                                 "menu"});
    buttons.push_back(new Button{double(menu_screen_width) - 150,
                                 0.9 * menu_screen_height, 2.4, "Quit", "quit"});
}

// Destructs state handler for controls window.
Control_Menu_State::~Control_Menu_State()
{
    while (buttons.size() > 0)
    {
        delete buttons.back();
        buttons.pop_back();
    }
}

// Event handler for control menu state.
void Control_Menu_State::handle(sf::Event &event, sf::RenderWindow &window,
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
    for (auto &button : buttons)
    {
        if (!(button->get_button_bounds()).contains(mouse.x, mouse.y))
        {
            continue;
        }
        *sound_to_play = "click.ogg";
        string on_push_action{button->get_on_push_action()};
        if (on_push_action == "menu")
        {
            state = new Menu_State(window, display_fps, music_enabled,
                                   sound_effects_enabled, level_id,
                                   player_count);
        }
        else if (on_push_action == "quit")
        {
            state = nullptr;
        }
    }
}

// Update all parameters for control menu state.
void Control_Menu_State::update(sf::Time delta)
{
    update_fps_counter(delta);
}

// Render all objects on screen.
void Control_Menu_State::render(sf::RenderWindow &window)
{
    window.setMouseCursor(cursor);

    window.draw(background);
    window.draw(control_info);
    for (auto &button : buttons)
    {
        button->render_button(window);
    }
    if (display_fps)
    {
        window.draw(fps_counter);
    }
}

// Read configuration file and apply default values for control menu state.
void Control_Menu_State::read_config_file()
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
    background_color = sf::Color(10, 55, 47);
    set_background(menu_screen_width, menu_screen_height);
}

// Loads texture for game controls.
void Control_Menu_State::load_controls_info_texture()
{
    if (!control_info_texture.loadFromFile("./data/textures/control_rules.png"))
    {
        throw std::runtime_error{"Kan inte öppna: control_rules.png"};
    }
    control_info.setTexture(control_info_texture);
    control_info.setPosition(50, 15);
    control_info.setScale(1, 1);
}