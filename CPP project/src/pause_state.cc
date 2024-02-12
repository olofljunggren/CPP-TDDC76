#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <fstream>
#include <sstream>

#include "../include/button.h"
#include "../include/state.h"
#include "../include/menu_state.h"
#include "../include/pause_state.h"
#include "../include/game_state.h"

using namespace std;

// Constructs state handler for when game paused.
Pause_State::Pause_State(
    sf::RenderWindow &window, State *&previous_state,
    bool display_fps, bool music_enabled,
    bool sound_effects_enabled, int level_id,
    int player_count) : State{display_fps, music_enabled, sound_effects_enabled,
                              level_id, player_count, 2},
                        previous_state{previous_state}
{
    read_config_file();
    Game_State *game_state = dynamic_cast<Game_State *>(previous_state);
    scale = game_state->get_scale(); 
    load_cursor();
    window.setMouseCursor(cursor);
    window.create(sf::VideoMode(game_screen_width, game_screen_height),
                  "Clash of Tanks Pause", sf::Style::Close);
    is_paused = true;

    if (!font.loadFromFile("./data/fonts/font.ttf"))
    {
        throw std::runtime_error{"Kan inte öppna: font.ttf"};
    }

    if (display_fps)
    {
        initialize_fps_counter(window, game_screen_width, game_screen_height);
    }

    buttons.push_back(new Button{double(game_screen_width) / 2,
                                 double(game_screen_height / 2) - 150, 3,
                                 "Continue", "continue"});
    buttons.push_back(new Button{double(game_screen_width) / 2,
                                 double(game_screen_height / 2) - 50, 3,
                                 "Main menu", "menu"});
}

// Destructs pause state handler.
Pause_State::~Pause_State()
{
    while (buttons.size() > 0)
    {
        delete buttons.back();
        buttons.pop_back();
    }
}

// Event handler for pause state.
void Pause_State::handle(sf::Event &event, sf::RenderWindow &window,
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
        if (on_push_action == "continue")
        {
            window.setMouseCursorVisible(false);
            State* temp{};
            temp = state;
            state = previous_state;
            previous_state = temp;
        }
        else if (on_push_action == "menu")
        {
            delete previous_state;
            state = new Menu_State(window, display_fps, music_enabled,
                                   sound_effects_enabled, level_id,
                                   player_count);
        }
    }
}

// Update all parameters for pause state.
void Pause_State::update(sf::Time delta)
{
    update_fps_counter(delta);
}

// Render all objects on screen.
void Pause_State::render(sf::RenderWindow &window)
{
    window.setMouseCursor(cursor);
    window.draw(background);
    previous_state->render(window);
    for (auto &button : buttons)
    {
        button->render_button(window);
    }
}

// Read configuration file and apply default values for pause state.
void Pause_State::read_config_file()
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
        else if (config_type == "fps_font_size")
        {
            fps_font_size = stod(config_value);
        }
        else if (config_type == "game_screen_width")
        {
            game_screen_width = stoi(config_value);
        }
        else if (config_type == "game_screen_height")
        {
            game_screen_height = stoi(config_value);
        }
        else
        {
            throw logic_error("Corrupt file: state_config.txt");
        }
    }
    background_color = sf::Color(35, 55, 47);
    set_background(game_screen_width, game_screen_height);
}