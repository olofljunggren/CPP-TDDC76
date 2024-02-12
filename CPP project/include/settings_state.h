#ifndef SETTINGS_STATE
#define SETTINGS_STATE

#include <SFML/Graphics.hpp>

#include "button.h"
#include "checkbox.h"
#include "state.h"

class Settings_State : public State
{
public:
    Settings_State(sf::RenderWindow &, bool, bool, bool, int, int);
    Settings_State &operator=(const Settings_State &) = delete;
    Settings_State(const Settings_State &) = delete;
    ~Settings_State() override;

    void handle(sf::Event &, sf::RenderWindow &, State *&, std::string *) override;
    void update(sf::Time );
    void render(sf::RenderWindow &) override;

private:
    void read_config_file() override;
    void initialize_number_players_text();
    void initialize_level_id_text();
    void generate_settings_buttons();
    void handle_button_event(Button *, sf::RenderWindow &, State *&);
    void handle_checkbox_event(Checkbox *);

    std::vector<Button *> buttons{};
    std::vector<Checkbox *> checkboxes{};
    unsigned int menu_screen_width{};
    unsigned int menu_screen_height{};

    sf::Text number_players_text{};
    sf::Text level_id_text{};
};

#endif