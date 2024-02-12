#include "../include/button.h"

using namespace std;

// Creates an empty button without text.
Button::Button() : scale{}, button_string{}, on_push_action{}
{
}

// Creates a button with text and an on-click event.
Button::Button(double x_pos, double y_pos, double scale, string button_string,
               string on_push_action) : scale{scale},
                                        button_string{button_string},
                                        on_push_action{on_push_action}
{
    position.push_back(x_pos);
    position.push_back(y_pos);

    load_sprite();
}

// Renders the button and its corresponding text on screen.
void Button::render_button(sf::RenderWindow &window)
{
    window.draw(button);
    window.draw(button_text);
}

// Returns the global rectangle bound of button.
sf::FloatRect Button::get_button_bounds()
{
    return button_bounds;
}

// Returns the on-click event of button.
string Button::get_on_push_action()
{
    return on_push_action;
}

// Loads texture and applies on sprite element.
void Button::load_sprite()
{
    // Loading button font.
    if (!font.loadFromFile("./data/fonts/font.ttf"))
    {
        throw std::runtime_error{"Kan inte öppna: font.ttf"};
    }

    // Loading button texture.
    if (!button_texture.loadFromFile("./data/textures/menu_bar.png"))
    {
        throw std::runtime_error{"Kan inte öppna: menu_bar.png"};
    }

    // Sets text position and scale.
    button_text.setFont(font);
    button_text.setString(button_string);
    sf::FloatRect button_text_bounds{button_text.getGlobalBounds()};
    button_text.setOrigin(button_text_bounds.width / 2,
                          button_text_bounds.height / 2 +
                              button_bounds.height * 0.2);
    button_text.setPosition(position.at(0), position.at(1));
    button_text.setScale(0.4 * scale, 0.4 * scale);

    // Sets sprite texture and sets position and scale.
    button.setTexture(button_texture);
    sf::Vector2u button_texture_size{button_texture.getSize()};
    button.setOrigin(button_texture_size.x / 2, button_texture_size.y / 2);
    button.setPosition(position.at(0), position.at(1));
    button.setScale(scale, scale);
    button_bounds = button.getGlobalBounds();
}