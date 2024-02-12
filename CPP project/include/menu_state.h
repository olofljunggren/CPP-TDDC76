#ifndef MENU_STATE
#define MENU_STATE

#include <SFML/Graphics.hpp>

#include "button.h"
#include "state.h"

class Menu_State : public State
{
public:
    Menu_State(sf::RenderWindow &, bool, bool, bool, int, int);
    Menu_State &operator=(const Menu_State &) = delete;
    Menu_State(const Menu_State &) = delete;
    ~Menu_State() override;

    void handle(sf::Event &, sf::RenderWindow &, State *&, std::string *) override;
    void update(sf::Time);
    void render(sf::RenderWindow &) override;

    void read_config_file() override;

private:
    void generate_menu_buttons();
    void load_new_state(std::string &, sf::RenderWindow &, State *&);
    std::vector<Button *> menu_buttons;
    unsigned int menu_screen_width{};
    unsigned int menu_screen_height{};
    unsigned int menu_music_volume{};
    unsigned int menu_sound_volume{};
};

#endif