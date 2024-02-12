#ifndef CONTROL_MENU_STATE
#define CONTROL_MENU_STATE

#include <SFML/Graphics.hpp>

#include "button.h"
#include "state.h"

class Control_Menu_State : public State
{
public:
    Control_Menu_State(sf::RenderWindow &, bool, bool, bool, int, int);
    Control_Menu_State &operator=(const Control_Menu_State &) = delete;
    Control_Menu_State(const Control_Menu_State &) = delete;
    ~Control_Menu_State() override;

    void handle(sf::Event &, sf::RenderWindow &, State *&, std::string *) override;
    void update(sf::Time);
    void render(sf::RenderWindow &) override;

private:
    void read_config_file() override;
    void load_controls_info_texture();
    std::vector<Button *> buttons{};
    unsigned int menu_screen_width{};
    unsigned int menu_screen_height{};

    sf::Sprite control_info{};
    sf::Texture control_info_texture{};
};

#endif