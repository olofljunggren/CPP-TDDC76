#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <fstream>
#include <sstream>

#include "../include/button.h"
#include "../include/checkbox.h"
#include "../include/state.h"
#include "../include/menu_state.h"
#include "../include/settings_state.h"

using namespace std;

// Constructs state handler for settings menu.
Settings_State::Settings_State(
    sf::RenderWindow &window, bool display_fps,
    bool music_enabled, bool sound_effects_enabled,
    int level_id, int player_count) : State{display_fps, music_enabled,
                                                 sound_effects_enabled,
                                                 level_id, player_count, 3}
{
    read_config_file();

    window.create(sf::VideoMode(menu_screen_width, menu_screen_height),
                  "Clash of Tanks Settings", sf::Style::Close);
    load_cursor();
    if (!font.loadFromFile("./data/fonts/font.ttf"))
    {
        throw std::runtime_error{"Kan inte öppna: font.ttf"};
    }

    initialize_fps_counter(window, menu_screen_width, menu_screen_height);

    generate_settings_buttons();
    initialize_number_players_text();
    initialize_level_id_text();
}

// Destructs state handler for settings state.
Settings_State::~Settings_State()
{
    while (buttons.size() > 0)
    {
        delete buttons.back();
        buttons.pop_back();
    }
    while (checkboxes.size() > 0)
    {
        delete checkboxes.back();
        checkboxes.pop_back();
    }
}

// Event handler for settings state.
void Settings_State::handle(sf::Event &event, sf::RenderWindow &window,
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
        handle_button_event(button, window, state);
    }

    for (auto &checkbox : checkboxes)
    {
        if ((checkbox->get_checkbox_bounds()).contains(mouse.x, mouse.y))
        {
            *sound_to_play = "click.ogg";
            handle_checkbox_event(checkbox);
        }
    }
}

// Update all parameters for settings state.
void Settings_State::update(sf::Time delta)
{
    update_fps_counter(delta);
}

// Render all objects on screen.
void Settings_State::render(sf::RenderWindow &window)
{
    window.setMouseCursor(cursor);

    window.draw(background);
    if (display_fps)
    {
        window.draw(fps_counter);
    }
    for (auto &button : buttons)
    {
        button->render_button(window);
    }
    for (auto &checkbox : checkboxes)
    {
        checkbox->render_checkbox(window);
    }
    window.draw(number_players_text);
    window.draw(level_id_text);
}

// Read configuration file and apply default values for settings state.
void Settings_State::read_config_file()
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

// Initialize text element for player count chosen.
void Settings_State::initialize_number_players_text()
{
    if (!font.loadFromFile("./data/fonts/font.ttf"))
    {
        throw std::runtime_error{"Kan inte öppna: font.ttf"};
    }
    number_players_text.setFont(font);
    number_players_text.setString("Player count: " + to_string(player_count));
    sf::FloatRect text_bounds{number_players_text.getGlobalBounds()};
    number_players_text.setOrigin(0, text_bounds.height / 2);
    number_players_text.setPosition(0.02 * menu_screen_width,
                                    0.5 * menu_screen_height);
    number_players_text.setScale(0.7, 0.7);
}

// Initialize text element for level ID chosen.
void Settings_State::initialize_level_id_text()
{
    if (!font.loadFromFile("./data/fonts/font.ttf"))
    {
        throw std::runtime_error{"Kan inte öppna: font.ttf"};
    }
    level_id_text.setFont(font);
    level_id_text.setString("Level: " + to_string(level_id));
    sf::FloatRect text_bounds{level_id_text.getGlobalBounds()};
    level_id_text.setOrigin(0, text_bounds.height / 2);
    level_id_text.setPosition(0.02 * menu_screen_width, 0.6 * menu_screen_height);
    level_id_text.setScale(0.7, 0.7);
}

// Creates Button and checkbox elements for all instances in settings menu.
void Settings_State::generate_settings_buttons()
{
    checkboxes.push_back(new Checkbox{100, 0.15 * menu_screen_height, 0.05,
                                      "Show frames per second (FPS)", "fps",
                                      display_fps});
    checkboxes.push_back(new Checkbox{100, 0.25 * menu_screen_height, 0.05,
                                      "Enable music", "music", music_enabled});
    checkboxes.push_back(new Checkbox{100, 0.35 * menu_screen_height, 0.05,
                                      "Enable sound effects", "sound_effects",
                                      sound_effects_enabled});

    buttons.push_back(new Button{0.5 * menu_screen_width,
                                 0.5 * menu_screen_height, 1.2,
                                 "2 players", "2players"});
    buttons.push_back(new Button{0.7 * menu_screen_width,
                                 0.5 * menu_screen_height, 1.2,
                                 "3 players", "3players"});
    buttons.push_back(new Button{0.9 * menu_screen_width,
                                 0.5 * menu_screen_height, 1.2,
                                 "4 players", "4players"});

    buttons.push_back(new Button{0.30 * menu_screen_width,
                                 0.6 * menu_screen_height, 1.2, "Level 1",
                                 "level1"});
    buttons.push_back(new Button{0.5 * menu_screen_width,
                                 0.6 * menu_screen_height, 1.2, "Level 2",
                                 "level2"});
    buttons.push_back(new Button{0.7 * menu_screen_width,
                                 0.6 * menu_screen_height, 1.2, "Level 3",
                                 "level3"});
    buttons.push_back(new Button{0.9 * menu_screen_width,
                                 0.6 * menu_screen_height, 1.2, "Level 4",
                                 "level4"});

    buttons.push_back(new Button{150, 0.9 * menu_screen_height, 2.4, "Return",
                                 "menu"});
    buttons.push_back(new Button{double(menu_screen_width) - 150,
                                 0.9 * menu_screen_height, 2.4, "Quit", "quit"});
}

// Handles actions for when button clicked.
void Settings_State::handle_button_event(Button *button,
                                         sf::RenderWindow &window, State *&state)
{
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
    else if (on_push_action.at(0) == '2' || on_push_action.at(0) == '3' ||
             on_push_action.at(0) == '4')
    {
        player_count = on_push_action.at(0) - '0';
    }
    else if (on_push_action.at(5) == '1' || on_push_action.at(5) == '2' ||
             on_push_action.at(5) == '3' || on_push_action.at(5) == '4')
    {
        level_id = on_push_action.at(5) - '0';
    }

    number_players_text.setString("Player count: " + to_string(player_count));
    level_id_text.setString("Level: " + to_string(level_id));
}

// Handles actions for when checkbox clicked.
void Settings_State::handle_checkbox_event(Checkbox *checkbox)
{
    if (checkbox->get_option_name() == "fps")
    {
        display_fps = checkbox->toggle();
    }
    else if (checkbox->get_option_name() == "music")
    {
        music_enabled = checkbox->toggle();
    }
    else if (checkbox->get_option_name() == "sound_effects")
    {
        sound_effects_enabled = checkbox->toggle();
    }
}