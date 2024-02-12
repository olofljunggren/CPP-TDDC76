#include "../include/checkbox.h"

using namespace std;

// Constructs a checkbox to toggle a corresponding boolean option.
Checkbox::Checkbox(
    double x_pos, double y_pos, double scale,
    string checkbox_string, string option_name,
    bool checked) : scale{scale}, checkbox_string{checkbox_string},
                    option_name{option_name}, checked{checked}
{
    position.push_back(x_pos);
    position.push_back(y_pos);

    load_sprite();
}

// Renders the checkbox and its corresponding text on screen.
void Checkbox::render_checkbox(sf::RenderWindow &window)
{
    window.draw(checkbox);
    window.draw(checkbox_text);
}

// Toggles the value of the checkbox.
bool Checkbox::toggle()
{
    checked = !checked;
    if (checked)
    {
        checkbox_texture.loadFromFile("./data/textures/checked_box.png");
    }
    else
    {
        checkbox_texture.loadFromFile("./data/textures/unchecked_box.png");
    }
    return checked;
}

// Returns the global rectangle bound of checkbox.
sf::FloatRect Checkbox::get_checkbox_bounds() const
{
    return checkbox_bounds;
}

// Returns the corresponding option to the button.
string Checkbox::get_option_name() const
{
    return option_name;
}

// Loads texture and and applies on sprite element.
void Checkbox::load_sprite()
{
    // Loads checkbox text font.
    if (!font.loadFromFile("./data/fonts/font.ttf"))
    {
        throw std::runtime_error{"Kan inte öppna: font.ttf"};
    }

    // Loads appropriate checkbox texture.
    if (checked)
    {
        if (!checkbox_texture.loadFromFile("./data/textures/checked_box.png"))
        {
            throw std::runtime_error{"Kan inte öppna: checked_box.png"};
        }
    }
    else
    {
        if (!checkbox_texture.loadFromFile("./data/textures/unchecked_box.png"))
        {
            throw std::runtime_error{"Kan inte öppna: unchecked_box.png"};
        }
    }

    // Sets checkbox position and scale.
    checkbox.setTexture(checkbox_texture);
    sf::Vector2u checkbox_texture_size{checkbox_texture.getSize()};
    checkbox.setOrigin(checkbox_texture_size.x / 2, checkbox_texture_size.y / 2);
    checkbox.setPosition(position.at(0), position.at(1));
    checkbox.setScale(3 * scale, 3 * scale);

    // Sets text position and scale.
    checkbox_text.setFont(font);
    checkbox_text.setString(checkbox_string);
    checkbox_text.setCharacterSize(20);
    checkbox_text.setPosition(
        position.at(0) + checkbox_texture_size.x * scale + 20,
        position.at(1) - 10);
    checkbox_bounds = checkbox.getGlobalBounds();
}