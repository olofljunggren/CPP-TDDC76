#ifndef BUTTON_H
#define BUTTON_H

#include <SFML/Graphics.hpp>
#include <vector>

class Button
{
public:
    Button();
    Button(double, double, double, std::string, std::string);
    Button(Button const &) = delete;
    Button &operator=(Button const &) = delete;
    ~Button() = default;

    void render_button(sf::RenderWindow &);

    sf::FloatRect get_button_bounds();
    std::string get_on_push_action();

private:
    void load_sprite();
    
    std::vector<double> position{};
    double scale;
    std::string button_string;
    std::string on_push_action;

    sf::Sprite button{};
    sf::Texture button_texture{};
    sf::Text button_text{};
    sf::Font font{};
    sf::FloatRect button_bounds{};
};

#endif